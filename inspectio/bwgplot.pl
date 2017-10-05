#!/usr/bin/perl
use strict;
use warnings;

#print "toto";
#@toto=(1,2,3);
my $dirname;
my @file_list;
my @time_list;
my @write_list;
my @read_list;
my @writebw_list;
my @readbw_list;

my $numberOfFile=$#ARGV+1;
my $i=0;

if (! -d $ARGV[0])
{
    print $ARGV[0]." is not a directory";
}
$dirname=$ARGV[0];

# construct the list of file
opendir(DIR, $ARGV[0]) or die $!;
while (my $file = readdir(DIR)) 
{
    if ($file =~ /^bwgplot/)
    {
	open(FH,$dirname.$file) or die $!;
	close(FH);
	@file_list=(@file_list,$dirname.$file); # create the filelist
    }
}


# read all file and construct the data in memory
$i=0;
foreach my $file (@file_list)
{
    my $linenumber=0;
    open(FH,$file);
    while (my $line=<FH>)
    {
	if ($line=~/\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/)
	{
	    my $time    = $1;
	    my $write   = $2;
	    my $read    = $3;
	    my $writebw = $4;
	    my $readbw  = $5;
	    
	    if (!defined($time_list[$linenumber]))    { $time_list[$linenumber]    = 0; }
	    if (!defined($write_list[$linenumber]))   { $write_list[$linenumber]   = 0; }
	    if (!defined($read_list[$linenumber]))    { $read_list[$linenumber]    = 0; }
	    if (!defined($writebw_list[$linenumber])) { $writebw_list[$linenumber] = 0; }
	    if (!defined($readbw_list[$linenumber]))  { $readbw_list[$linenumber]  = 0; }
		    
	    $time_list[$linenumber]    = $time;
	    $write_list[$linenumber]   = $write_list[$linenumber]   + $write;
	    $read_list[$linenumber]    = $read_list[$linenumber]    + $read;
	    $writebw_list[$linenumber] = $writebw_list[$linenumber] + $writebw;
	    $readbw_list[$linenumber]  = $readbw_list[$linenumber]  + $readbw;
	}
	$linenumber++;
    }
    close(FH);
}

$i=0;
while ($i < $#time_list)
{
    #print $time_list[$i]." ".$write_list[$i]." ".$read_list[$i]." ".$writebw_list[$i]." ".$readbw_list[$i]."\n";
    printf("%8llu\t%12llu\t%12llu\t%8llu\t%8llu\n",
	   $time_list[$i],
	   $write_list[$i],
	   $read_list[$i],
	   $writebw_list[$i],
	   $readbw_list[$i]);
    $i++;
}
