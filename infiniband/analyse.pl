#!/usr/bin/perl

use strict;
use warnings;

################################################################################
package ibswitchParser;
sub new()
{
    my $class=shift;
    my $self=
    {
	
    };
    bless $self;
    return $self;
}

sub open()
{
    my $file="ibswitches.log";
    open(FH,"<$file") or die "can not open $file\n";
    while (my $line=<FH>)
    {
	#print $line;
	if ($line=~//);
    }
    close(FH);
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
	"guid" => undef,
	"lid"  => undef,
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
sub printme()
{
    my $self=shift;
    print "guid:".$self->{guid}." lid:".$self->{lid}."\n";
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
$fabric->printSwitch();

$ibswParser->open();

#$ib->printme();

# print "toto";

