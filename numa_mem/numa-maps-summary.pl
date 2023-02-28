#!/usr/bin/perl
 
# Copyright (c) 2010, Jeremy Cole <jeremy@jcole.us>
# Modified (c) 2014, Ivan Prisyazhniy <john.koepi@gmail.com>
#   - top [num] - mode to print numa map distribution from mem top
 
# This program is free software; you can redistribute it and/or modify it
# under the terms of either: the GNU General Public License as published
# by the Free Software Foundation; or the Artistic License.
# 
# See http://dev.perl.org/licenses/ for more information.
 
#
# This script expects a numa_maps file as input.  It is normally run in
# the following way:
#
#     # perl numa-maps-summary.pl < /proc/pid/numa_maps
#
# Additionally, it can be used (of course) with saved numa_maps, and it
# will also accept numa_maps output with ">" prefixes from an email quote.
# It doesn't care what's in the output, it merely summarizes whatever it
# finds.
#
# The output should look something like the following:
#
#     N0        :      7983584 ( 30.45 GB)
#     N1        :      5440464 ( 20.75 GB)
#     active    :     13406601 ( 51.14 GB)
#     anon      :     13422697 ( 51.20 GB)
#     dirty     :     13407242 ( 51.14 GB)
#     mapmax    :          977 (  0.00 GB)
#     mapped    :         1377 (  0.01 GB)
#     swapcache :      3619780 ( 13.81 GB)
#
# New memory top mode:
#
#     # perl numa-maps-summary.pl top 10
#
# The output should look something like the following:
#
#     PID: 16821   : N0 :   101349 (  0.39 GB), N1 :   981166 (  3.74 GB)
#     PID: 16829   : N0 :   117047 (  0.45 GB), N1 :   965419 (  3.68 GB)
#     PID: 22632   : N0 :   995014 (  3.80 GB), N1 :    76228 (  0.29 GB)
#     PID: 16813   : N0 :    60841 (  0.23 GB), N1 :   755779 (  2.88 GB)
#     PID: 22561   : N0 :   144255 (  0.55 GB), N1 :      338 (  0.00 GB)
#     PID: 23645   : N0 :    19340 (  0.07 GB), N1 :     5584 (  0.02 GB)
#     PID: 23464   : N0 :    15254 (  0.06 GB), N1 :     3321 (  0.01 GB)
#     PID: 28100   : N0 :    11455 (  0.04 GB), N1 :     6756 (  0.03 GB)
#     PID: 23466   : N0 :    16458 (  0.06 GB), N1 :     1480 (  0.01 GB)
#     PID: 28096   : N0 :    15860 (  0.06 GB), N1 :     2065 (  0.01 GB)
#     PID: 28097   : N0 :    10270 (  0.04 GB), N1 :     7275 (  0.03 GB)
#     PID: 23636   : N0 :    10753 (  0.04 GB), N1 :     6384 (  0.02 GB)
#     PID: 4629    : N0 :     9105 (  0.03 GB), N1 :     8032 (  0.03 GB)
#     PID: 23463   : N0 :    13528 (  0.05 GB), N1 :     3407 (  0.01 GB)
#     PID: 20125   : N0 :     8397 (  0.03 GB), N1 :     8506 (  0.03 GB)
#     Total: N0 : 1548926 (5.90868377685547 GB) N1 : 2831740 (10.8022308349609 GB)
 
use Data::Dumper;
 
sub parse_numa_maps_line($$)
{
  my ($line, $map) = @_;
 
  if($line =~ /^[> ]*([0-9a-fA-F]+) (\S+)(.*)/)
  {
    my ($address, $policy, $flags) = ($1, $2, $3);
 
    $map->{$address}->{'policy'} = $policy;
 
    $flags =~ s/^\s+//g;
    $flags =~ s/\s+$//g;
    foreach my $flag (split / /, $flags)
    {
      my ($key, $value) = split /=/, $flag;
      $map->{$address}->{'flags'}->{$key} = $value;
    }
  }
}
 
sub parse_numa_maps_fd()
{
  my ($fd) = @_;
  my $map = {};
 
  while(my $line = <$fd>)
  {
    &parse_numa_maps_line($line, $map);
  }
  return $map;
}

sub get_sums() 
{
  my ($map) = @_;
  my $sums = {};
 
  foreach my $address (keys %{$map})
  {
    if(exists($map->{$address}->{'flags'}))
    {
      my $flags = $map->{$address}->{'flags'};
      foreach my $flag (keys %{$flags})
      {
        next if $flag eq 'file';
        $sums->{$flag} += $flags->{$flag} if defined $flags->{$flag};
      }
    }
  }
 
  return $sums;
}

if ( -TSTDIN and not @ARGV ) 
{ 
  my $map = &parse_numa_maps_fd(\*STDIN);
  my $sums = &get_sums($map);
 
  foreach my $key (sort keys %{$sums})
  {
    printf "%-10s: %12i (%6.2f GB)\n", $key, $sums->{$key}, $sums->{$key}/262144;
  }
} 
else 
{
  my $top = 1 + 10;
  my $num_args = $#ARGV + 1;
  my $op = $ARGV[0];

  if ($num_args < 1 or $op ne 'top') 
  {
    print "Usage: numa_maps_summary.pl top [num] \n";
    exit;
  }

  if ($num_args > 1) 
  {
    $top = '1' + $ARGV[1];
  }

  my $total = {};
  my @pids = split(' ', `ps -e k-rss -o pid | head -n $top | tail -n +2`);
  foreach my $pid (@pids)
  {
    my $map = {};
    my $src = "/proc/$pid/numa_maps";
    my $data = do {
      open my $fh, "<", $src
        or die "could not open $src: $!";
      $map = &parse_numa_maps_fd($fh);
    };

    die "could not open $src: $!" unless length($map) > 0;

    my $sums = &get_sums($map);
    my $cmd = `ps -p $pid -o cmd | tail -n +2 | tr -d '\n\r'`;
    $cmd =~ s/\s+$//;

    #printf "%-4s: %8i (%s)\n", 'PID', $pid, $cmd;
    printf "PID: %-8s: ", $pid;
    foreach my $key (sort keys %{$sums})
    {
      next if substr($key,0,1) ne 'N';
      printf "%-3s: %8i (%6.2f GB), ", $key, $sums->{$key}, $sums->{$key}/262144;
      $total->{$key} += $sums->{$key};
    }
    print "$cmd\n";
  }
  print "Total:";
  foreach my $key (sort keys %{$total})
  {
    print " ", ${key}, " : ", $total->{$key}, " (", $total->{$key}/262144, " GB)";
  }
  print "\n";
}
