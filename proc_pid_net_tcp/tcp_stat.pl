#!/usr/bin/perl
#
# ref https://elixir.bootlin.com/linux/v4.14.42/source/include/net/tcp_states.h
@tcpstates = (
  "DUMMY",
  "TCP_ESTABLISHED",  #1
  "TCP_SYN_SENT",     #2
  "TCP_SYN_RECV",     #3
  "TCP_FIN_WAIT1",    #4
  "TCP_FIN_WAIT2",    #5
  "TCP_TIME_WAIT",    #6
  "TCP_CLOSE",        #7
  "TCP_CLOSE_WAIT",   #8
  "TCP_LAST_ACK",     #9
  "TCP_LISTEN",       #10
  "TCP_CLOSING",      #11
  "TCP_NEW_SYN_RECV", #12
  "TCP_MAX_STATES,"   #13
);

sub ipport() {
  $_[0] =~ m/([0-9A-F]+):([0-9A-F]+)/;
  $hexip = $1; $hexport = $2;
  my @ip = map hex($_), ( $hexip =~ m/../g );
  my $ip = join('.',reverse(@ip));
  my $port = hex($hexport);
  return ($ip . ":" . $port);
}

while ($line = <>) {
  if ($line =~ /(^[ ]*[0-9]+: )([0-9A-F:]+) ([0-9A-F:]+) ([0-9A-F]+)(.*)/) {
    $head = $1;
    $loc = $2;
    $rem = $3;
    $stat = $4;
    $tail = $5;
    if (hex($stat) == 0) {
      print STDERR "hex($stat) is 0\n";
    }
    push @output, sprintf("%s%-21s %-21s %s %s\n", $head, &ipport($loc), &ipport($rem), $tcpstates[hex($stat)]);
  }
}

foreach $line (@output) {
  print $line;
}
