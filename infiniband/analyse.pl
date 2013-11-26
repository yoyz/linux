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

sub open()
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
    my $i=0;
#    for my $switch (@switchList)
#    {
	#print
	#$switch->printme();
	#$i++;
#    }
    #print $i;
    #print "\n";
    #print $#switchList;    
    return $#switchList+1; 
}

sub addSwitch($$)
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

sub printSwitch()
{
    my $self=shift;
    my $switch=shift;
    my $ref_switchList=$self->{switchList};
    my @switchList=@$ref_switchList;    
    for my $switch (@switchList)
    {
	$switch->printme();
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

sub getstr()
{
    my $self=shift;
    return "guid:".$self->{guid}." lid:".$self->{lid}." desc:".$self->{desc};
}

sub printme()
{
    my $self=shift;
    print $self->getstr()."\n";
    #print "guid:".$self->{guid}." lid:".$self->{lid}." desc:".$self->{desc}."\n";
}


1;


my $sw1;
my $sw2;
my $fabric;
my $ibswParser;


$fabric=IBFabric::new();
$ibswParser=ibswitchParser::new();

$sw1=IBSwitch::new();
$sw1->setguid("08003800013b0489");
$sw1->setlid(12);

$sw2=IBSwitch::new();
$sw2->setguid("08003800013b0489");
$sw2->setlid(12);

#print $sw1->equal($sw2);
#exit(0);
$fabric->addSwitch($sw1);
$fabric->addSwitch($sw2);
#$fabric->printSwitch();

$ibswParser->open();
#print "\n";
#print $ibswParser->{ibfab}->numberOfSwitch();
#$ib->printme();

# print "toto";

