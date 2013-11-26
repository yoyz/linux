#!/usr/bin/perl

use strict;
use warnings;


################################################################################
package ibnetdiscoverParser;
sub new()
{
   my $class=shift;
    my $ibfab=IBFabric::new();
    my $self=
    {
	"ibfab" => $ibfab,
    };
    bless $self;
    return $self;
}

sub getFabric()
{
    my $self=shift;
    return $self->{ibfab};
}


################################################################################
package ibswitchParser;
sub new()
{
    my $class=shift;
    my $ibfab=IBFabric::new();
    my $self=
    {
	"ibfab" => $ibfab,
    };
    bless $self;
    return $self;
}

sub getFabric()
{
    my $self=shift;
    return $self->{ibfab};
}

sub openAndParse()
{
    my $self=shift;
    my $file="ibswitches.log";
    open(FH,"<$file") or die "can not open $file\n";
    while (my $line=<FH>)
    {
	#print $line;
	if ($line=~/^Switch/)
	{
	    if ($line=~/Switch\s+:\s+0x([0-9a-fA-F\s+-]+)\s+ports\s+(\d+)\s+"([0-9a-zA-Z\s+-]+)"\sbase\sport\s(\d+)\s+lid\s(\d+)\s+lmc\s+(\d+)/x)
	    {
		    
		# 1 guid
		# 2 nbport
		# 3 desc
		# 5 base port
		# 6 lid
		# 7 lmc

		my $guid=$1;
		my $numberofport=$2;
		my $desc=$3;
		my $baseport=$4;
		my $lid=$5;
		my $lmc=$6;
		my $switch=IBSwitch::new();

		$switch->setguid($guid);
		$switch->setlid($lid);
		$switch->setdesc($desc);
		$switch->setnbport($numberofport);
		$self->{ibfab}->addSwitch($switch);
	    }
	}
	
    }
    close(FH);
    #$self->{ibfab}->printSwitch();
}

1;
################################################################################
package IBFabric;
sub new()
{
    my $class=shift;
    my @switchList;
    my @caList;
    my $ref_switchList=\@switchList;
    my $ref_caList=\@caList;

    my $self=
    {
	switchList => $ref_switchList,
	caList     => $ref_caList,
    };
    bless $self;
    return $self;
}

sub numberOfSwitch()
{
    my $self=shift;
    my $ref_switchList=$self->{switchList};
    my @switchList=@$ref_switchList;  
    return $#switchList+1; 
}

sub getSwitch($)
{
    my $self=shift;
    my $number=shift;
    my $ref_switchList=$self->{switchList};
    my @switchList=@$ref_switchList;  
    return $switchList[$number]; 
}


sub addSwitch($)
{
    my $self=shift;
    my $switchToAdd=shift;
    my $found=0;
    my $ref_switchList=$self->{switchList};
    my @switchList=@$ref_switchList;  
    foreach my $switch (@switchList)
    {
	if ($switch->equal($switchToAdd))
	{ $found=1; }
    }
    if ($found==0) { @switchList=(@switchList,$switchToAdd); }
    $ref_switchList=\@switchList;
    $self->{switchList}=$ref_switchList;
}

sub getStr()
{
    my $self=shift;
    my $switch=shift;
    my $ref_switchList=$self->{switchList};
    my @switchList=@$ref_switchList;
    my $str="";
    my $i=0;
    for my $switch (@switchList)
    {
	if ($i!=0) { $str.="\n"; }
	$str.=$switch->getStr();
	$i++;
    }
    return $str;
}

sub printMe()
{
    my $self=shift;
    print $self->getStr()."\n";
}

# add switch from the $1 fabric
sub mergeFabric($)
{
    my $self=shift;
    my $ibfab=shift;
    my $ref_switchList=$ibfab->{switchList};
    my @switchList=@$ref_switchList;    
    for my $switch (@switchList)
    {
	$self->addSwitch($switch);
    }
}

1;
################################################################################
package IBLink;
sub new()
{
    my $class=shift;
    my $self=
    {
	"portNumberFrom" => undef,
	"portNumberTo"   => undef,
	"portGuidFrom"   => undef,
	"portGuidTo"     => undef,
    };
    bless  $self;
    return $self;    
}
1;
################################################################################
package IBSwitch;
sub new()
{
    my $class=shift;
    my $self=
    {
	"guid"   => undef,
	"lid"    => undef,
	"desc"   => undef,
	"nbport" => undef,
    };
    bless  $self;
    return $self;
}

sub equal($$)
{
    my $self=shift;
    my $other=shift;
    if ($self->{guid} eq $other->{guid} &&
	$self->{lid}  eq $other->{lid})
    {	return 1; }
    return 0;
}

sub setguid($$)
{
    my $self=shift;
    my $g=shift;
    $self->{guid}=$g;
}

sub setlid($$)
{
    my $self=shift;
    my $l=shift;
    $self->{lid}=$l;
}

sub setdesc($$)
{
    my $self=shift;
    my $d=shift;
    $self->{desc}=$d;
}

sub setnbport($$)
{
    my $self=shift;
    my $nb=shift;
    $self->{nbport}=$nb;
}

sub getStr()
{
    my $self=shift;
    return "guid:".$self->{guid}." lid:".$self->{lid}." desc:".$self->{desc};
}

sub printMe()
{
    my $self=shift;
    print $self->getstr()."\n";
    #print "guid:".$self->{guid}." lid:".$self->{lid}." desc:".$self->{desc}."\n";
}


1;


my $sw1;
my $sw2;
my $sw3;
my $sw4;
my $fabric1;
my $fabric2;
my $ibswParser;


$fabric1=IBFabric::new();
$fabric2=IBFabric::new();
$ibswParser=ibswitchParser::new();

$sw1=IBSwitch::new();
$sw1->setguid("08003800013b0489");
$sw1->setlid(12);
$sw1->setdesc("sw1");


$sw2=IBSwitch::new();
$sw2->setguid("08003800013b0490");
$sw2->setlid(13);
$sw2->setdesc("sw2");

$sw3=IBSwitch::new();
$sw3->setguid("08003800013b0491");
$sw3->setlid(14);
$sw3->setdesc("sw3");

$sw4=IBSwitch::new();
$sw4->setguid("08003800013b0492");
$sw4->setlid(15);
$sw4->setdesc("sw4");
#print $sw1->equal($sw2);
#exit(0);
$fabric1->addSwitch($sw1);
$fabric1->addSwitch($sw2);

$fabric2->addSwitch($sw3);
$fabric2->addSwitch($sw4);


print "fab1\n";
$fabric1->printMe();

print "fab2\n";
$fabric2->printMe();

print "fabmerge\n";
$fabric1->mergeFabric($fabric2);
$fabric1->printMe();

#$ibswParser->openAndParse();
#$fabric2=$ibswParser->getFabric();
#print $fabric2->getSwitch(2)->getStr();


#print "\n";
#print $ibswParser->{ibfab}->numberOfSwitch();
#$ib->printme();

# print "toto";

