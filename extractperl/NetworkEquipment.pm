# @(#) $Id: NetworkEquipment.pm,v 1.26.2.2 2011/04/15 15:54:52 peyrardj Exp $

package Bull::CLM::DB::NetworkEquipment;
1;

######################################## EquipmentNetwork ########################################

package Bull::CLM::DB::EquipmentNetwork;

use strict;
use warnings;


sub new()
{
    my $class=shift;
    my $self=
    {
	"equipmentList"   => undef,
	"networkList"     => undef,
	"dnsvip"          => undef,
    };
    
    bless $self;
    $self->{"equipmentList"}=Bull::CLM::DB::EquipmentList::new();
    $self->{"networkList"}=  Bull::CLM::DB::NetworkList::new();
    $self->{"dnsvip"}=       Bull::CLM::DB::DNSVIP::new();
    return $self;
}

sub get_eql()
{
    my $self=shift;
    return $self->{"equipmentList"};
}

sub get_nwl()
{
    my $self=shift;
    return $self->{"networkList"};
}

sub get_dnsvip()
{
    my $self=shift;
    return $self->{"dnsvip"};
}

sub get_equipmentList()
{
    my $self=shift;
    return $self->get_eql();
}

sub get_networkList()
{
    my $self=shift;
    return $self->get_nwl();
}



1;



######################################## NetworkInterface ########################################

package Bull::CLM::DB::NetworkInterface;

use strict;
use warnings;

use Bull::CLM::Trace;
use Bull::CLM::Config;
use Bull::CLM::Log;



#my $regex_ipv4="(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)";

#my $regex_ipv4_network="(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?\/[0-9]|[0-9][0-9])";

#my $regex_mac="[a-fA-f0-9][a-fA-f0-9]:[a-fA-f0-9][a-fA-f0-9]:[a-fA-f0-9][a-fA-f0-9]:[a-fA-f0-9][a-fA-f0-9]:[a-fA-f0-9][a-fA-f0-9]:[a-fA-f0-9][a-fA-f0-9]";

#my $regex_name="^[a-zA-Z0-9-_]+";

my $regex_ipv4_network=qr!(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?\/[0-9]|[0-9][0-9])!;
my $regex_ipv4=qr!(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)!;
my $regex_name=qr!^[a-zA-Z0-9-_]+!;
my $regex_mac=qr!^[a-fA-f0-9][a-fA-f0-9]:.+!;


sub new()
{
    my $class=shift;
    my $self=
    {
	"name"            => undef, # equipmentName
	"equipment_name"  => undef, # equipmentName
	"ip"              => undef, # ipv4 
	"mac"             => undef, # mac or guid
	"type"            => undef, # type : host,hardwareManager
	"vip"             => undef, # virtual
	"port_type"       => undef, # ?
	"port_number"     => undef, # for disk_array
	"rank_number"     => undef, # for disk_array
	"iname"           => undef, # for host : eth0, ib1..
	"ftype"		  => undef, # for host: mngt|mws|login...
    };
    bless $self;
    return $self;
}

sub isa()
{
    my $class=shift;
    return "NetworkInterface";
}

sub set($$)
{
  my $self=shift;
  my $key=shift;
  my $value=shift;
  $self->{"$key"}=$value;
}

sub get($)
{
  my $self=shift;
  my $key=shift;
  return $self->{"$key"};
}


sub set_name($)
{
    my $class=shift;
    my $name=shift;
    my $fct = (caller(0))[3];

    if (($name =~ $regex_name) and (!($name =~ /_/ )))
    {
	$class->{"name"}=$name;
	return $class->{"name"};
    }
    elsif ( $name =~ $regex_name )
    {
	clmVerbose($fct,"name $name doesn't respect name convention RFC 952 : 's/_/-/g'");
	$name=~s/_/-/g;
	$class->{"name"}=$name;
	return $class->{"name"};
    }
    else
    {
	print("\033[1m");
	clmError($fct,"name $name doesn't respect name convention RFC 952.");
	print("\033[0m");
	exit 1;
	return undef;
    }
}

sub get_name()
{
    my $class=shift;
    return $class->{"name"};
}

sub set_ip()
{
    my $class=shift;
    my $ip=shift;
    my $fct = (caller(0))[3];

    $ip = "" if (not defined($ip));

    if ($ip =~ $regex_ipv4)
    {
	$class->{"ip"}=$ip;
	return $class->{"ip"};
    }
    else
    {
	my $bufname = $class->{"name"};
	clmWarning($fct,"'$ip' is not an IPv4 address. The equipment $bufname will not have a DNS record.");
	return undef;
    }
}

sub get_ip()
{
    my $class=shift;
    return $class->{"ip"};
}

sub set_mac($)
{
    my $class=shift;
    my $mac=shift;

    if ($mac =~ $regex_mac)
    {
	$mac=uc($mac);
	$class->{"mac"}=$mac;
	return $class->{"mac"};
    }
    else
    {
	return undef;
    }
}

sub get_mac()
{
    my $class=shift;
    return $class->{"mac"};
}

sub set_type()
{
    my $class=shift;
    my $type=shift;
    $type=uc($type);
    $class->{"type"}=$type;
    return $class->{"type"};
}


sub get_type()
{
    my $class=shift;
    return $class->{"type"};
}

sub set_specific_property($$)
{
    my $class=shift;
    my $key=shift;
    my $value=shift;
    $class->{"$key"}=$value;
}

sub get_specific_property($$)
{
    my $class=shift;
    my $key=shift;
    my $value=$class->{"$key"};
    if (defined($value))  { return $class->{"$key"}; }
    else                  { return ""; }
}

sub to_string()
{
    my $class=shift;
    my $str="[".$class->isa()."] ";
    if (defined($class->{"networkName"})) { $str.="network-name: ".$class->{"network-name"}."\t";   }
    if (defined($class->{"ip"}))          { $str.="ip: ".$class->{"ip"}."\t";   }
    if (defined($class->{"mac"}))         { $str.="mac: ".$class->{"mac"}."\t";  }
    if (defined($class->{"name"}))        { $str.="name: ".$class->{"name"}."\t"; }
    if (defined($class->{"type"}))        { $str.="type: ".$class->{"type"}."\t"; }
    if (defined($class->{"suffix"}))      { $str.="suffix: ".$class->{"suffix"}."\t"; }
    if ($str=~/^$/) { return undef; }
    else            { return $str;  }
}

1;

######################################## Equipment ########################################

package Bull::CLM::DB::Equipment;

use strict;
use warnings;


use Bull::CLM::Trace;
use Bull::CLM::History;


sub new()
{
    my $class=shift;
    my $self=
    {
	"name"         => undef,
	"type"         => undef,
	"interfaces"   => undef,
	"eq_id"        => undef,
	"ftype"	       => undef, # for host: mngt|mws|login...
    };

    bless $self;
    $self->{"interfaces"}=Bull::CLM::DB::EquipmentList::new();
    return $self;
}
sub set($$)
{
  my $self=shift;
  my $key=shift;
  my $value=shift;
  $self->{"$key"}=$value;
}

sub get($)
{
  my $self=shift;
  my $key=shift;
  return $self->{"$key"};
}



sub isa()
{
    my $class=shift;
    return "Equipment";
}

sub set_name($)
{
    my $class=shift;
    my $name=shift;
    my $fct = (caller(0))[3];

    if (($name =~ $regex_name) and (!($name =~ /_/ )))
    {
	$class->{"name"}=$name;
	return $class->{"name"};
    }
    elsif ( $name =~ $regex_name )
    {
	clmVerbose($fct,"name $name doesn't respect name convention RFC 952 : 's/_/-/g'");
	$name=~s/_/-/g;
	$class->{"name"}=$name;
	return $class->{"name"};
    }
    else
    {
	print("\033[1m");
	clmError($fct,"name $name doesn't respect name convention RFC 952.");
	print("\033[0m");
	exit 1;
    }
}


sub set_type()
{
    my $class=shift;
    my $type=shift;
    $type=uc($type);
    $class->{"type"}=$type;
    return $class->{"type"};
}

sub get_name()
{
    my $class=shift;
    return $class->{"name"};
}


sub get_type()
{
    my $class=shift;
    return $class->{"type"};
}


sub add_interface($)
{
    my $class=shift;
    my $netint=shift;
    $class->{"interfaces"}->add($netint);
}

sub number_of_interface()
{
    my $class=shift;
    return $class->{"interfaces"}->number_of_equipment();
}

sub get_interface($)
{
    my $class=shift;
    my $interface_number=shift;
    my $max=$class->number_of_interface();
    my $interface=undef;

    if ($interface_number<$max)
    {
	$interface=$class->{"interfaces"}->get($interface_number);
    }
    return $interface;
}


sub to_string()
{
    my $class=shift;
    my $str="[".$class->isa()."] ";
    if (defined($class->{"type"}))      { $str.="type: ".$class->{"type"}."\t"; }
    if (defined($class->{"name"}))      { $str.="name: ".$class->{"name"}."\t"; }


    for (my $i=0; $i<$class->{"interfaces"}->number_of_equipment();$i++)
    {
	if ($i==0) { $str.="\n"; }
	$str.="\t".$class->{"interfaces"}->get($i)->to_string()."\n";
    }
    

    if ($str=~/^$/) { return undef; }
    else            { return $str;  }
}



1;


######################################## EquipmentList ########################################


package Bull::CLM::DB::EquipmentList;

use strict;
use warnings;

sub new()
{
    my $class=shift;
    my @array;
    my %hash;
    
    
    my $self=
    {
	"element"=> \@array,
	"hash" => \%hash,
    };
    bless $self;
    return $self;
}

sub display_hash()
{
    my $class=shift;
    my %hash;
    my $ref_hash;
    
    $ref_hash=$class->{"hash"};
    %hash=%$ref_hash;

    foreach my $key ( keys %hash )
    {
	print "key : ".$key.", value : ".$hash{$key}."\n";
    }
}

sub del_hash_entry($)
{
    my $class=shift;
    my $name=shift;
    my %hash;
    my $ref_hash;

    $ref_hash=$class->{"hash"};
    %hash=%$ref_hash;
    delete($hash{$name});
    $ref_hash=\%hash;
    $class->{"hash"}=$ref_hash;
}


sub add_hash_entry($$)
{
    my $class=shift;
    my $name=shift;
    my $number=shift;
    my %hash;
    my $ref_hash;
    $class->{"hash"}{$name}=$number;



}

sub change_hash_entry($$)
{
    my $class=shift;
    my $current_name=shift;
    my $new_name=shift;
    my $number;
    my %hash;
    my $ref_hash=$class->{hash};
    %hash=%$ref_hash;
    $number=$hash{$new_name};
    $class->del_hash_entry($current_name);
    $class->add_hash_entry($new_name,$number);
}


sub add($)
{
    my $class=shift;
    my $element=shift;
    my @array;
    my %hash;
    my $ref_hash=$class->{hash};
    my $ref_array=$class->{element};
    push(@$ref_array,$element);
    $class->add_hash_entry($element->get_name(),$#{$ref_array});   
    $class->{element}=$ref_array;
}





sub get($)
{
    my $class=shift;
    my $number=shift;
    my $ref_array;
    $ref_array=$class->{element};
    return ${$ref_array}[$number];
}


sub get_number_by_name($)
{

    my $class=shift;
    my $name=shift;
    return $class->{hash}{$name};
}

sub name_exist($)
{
    my $class=shift;
    my $name=shift;
    my $found=0;
    my %hash;
    if (exists $class->{hash}{$name}) { $found=1; }
    return $found;
}



sub number_of_equipment()
{
    my $class=shift;
    my $ref_array=$class->{element};
    return $#{$ref_array}+1;
}



sub to_string()
{
    my $class=shift;
    my $str="";

    for ( my $i=0 ; $i < $class->number_of_equipment(); $i++)
    {
	my $equipment=$class->get($i);
	$str.=$equipment->to_string()."\n";
    }
    return $str;
}



1;


######################################## Network ########################################

package Bull::CLM::DB::Network;

#@INC=(@INC,".");


use strict;
use warnings;
use Net::Netmask;

#my $regex_ipv4_network="(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?\/[0-9]|[0-9][0-9])";



# $self.{network} is a string like "10.0.0.0/24"
#                       
sub new()
{
    my $class=shift;
    my $equipmentList=Bull::CLM::DB::EquipmentList::new();

    my $self=
    {
	"network"       => undef,
	"equipmentList" => $equipmentList,
	"netmask"       => undef,
	"netmask_class" => undef,
	"function"      => undef,
	"techno"        => undef,
	"cidr"          => undef,
	"gateway"       => undef,
	"broadcast"     => undef,
	"id"            => undef,
	"parent_id"     => undef,
	"type"          => undef,
	"name"          => undef,
	"dnsdomain"     => undef,
	"suffix"        => undef,
	"vlan"          => undef,
    };
    bless $self;
    return $self;
}


sub is_parent()
{
    my $class=shift;
    if ($class->{"parent_id"}==undef)
    { return 1; }
    else
    { return 0; }
}

sub is_son()
{
    my $class=shift;
    if ($class->is_parent()) { return 0; }
    else                     { return 1; }
}


sub set_id($)
{
    my $class=shift;
    my $id=shift;
    $class->{"id"}=$id;
}

sub set_parent_id($)
{
    my $class=shift;
    my $parent_id=shift;
    $class->{"parent_id"}=$parent_id;
}

sub set_type($)
{
    my $class=shift;
    my $type=shift;
    $class->{"type"}=$type;
}

sub set_dnsdomain($)
{
    my $class=shift;
    my $dnsdomain=shift;
    $class->{"dnsdomain"}=$dnsdomain;
}

sub get_type()
{
    my $class=shift;
    return $class->{"type"};
}

sub set_name($)
{
    my $class=shift;
    my $name=shift;
    $class->{"name"}=$name;
}

sub set_vlan($)
{
    my $class=shift;
    my $vlan=shift;
    $class->{"vlan"}=$vlan;
}

sub set_suffix($)
{
    my $class=shift;
    my $suffix=shift;
    $class->{"suffix"}=$suffix;
}


sub get_vlan()
{
    my $class=shift;
    my $vlan=$class->{"vlan"};
    return $vlan; 
}

sub get_pkey()
{
    my $class=shift;
    my $pkey=$class->{"vlan"};
    my $default_pkey="FF:FF";
    if (!defined($pkey)) { $pkey=$default_pkey; }
    if ($pkey=~ /([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])/)
    {
	$pkey="$1".":"."$2";
    }
    else
    {
	$pkey="FF:FF";
    }
    return $pkey;
}


sub get_name()
{
    my $class=shift;
    return $class->{"name"};
}


sub get_id()
{
    my $class=shift;
    return $class->{"id"};
}

sub get_parent_id()
{
    my $class=shift;
    return $class->{"parent_id"};
}

sub get_ip()
{
    my $class=shift;
    my $number=shift;
    my $netmask_class=$class->{"netmask_class"};
    return $netmask_class->nth($number);
}


sub get_last_ip()
{
    my $class=shift;
    return $class->get_ip(-1);
}

sub get_first_ip()
{
    my $class=shift;
    return $class->get_ip(1);
}


sub set_function($)
{
    my $class=shift;
    my $function=shift;
    $class->{"function"}=$function;
}

sub get_function()
{
    my $class=shift;
    return $class->{"function"};
}

sub set_techno($)
	       {
    my $class=shift;
    my $techno=shift;
    $class->{"techno"}=$techno;
}

sub get_techno()
{
    my $class=shift;
    return $class->{"techno"};
}



sub set_gateway()
{
    my $class=shift;
    my $gateway=shift;
    $class->{"gateway"}=$gateway;
}

sub get_gateway()
{
    my $class=shift;
    return $class->{"gateway"};
}

sub get_dnsdomain()
{
    my $class=shift;
    return $class->{"dnsdomain"};
}

sub get_broadcast()
{
    my $class=shift;
    my $broadcast;
    my $netmask_class;
    $netmask_class=$class->{"netmask_class"};
    if ($netmask_class)
    {	return $netmask_class->broadcast(); }
    else
    {   return undef; }
}

#Set network with a cidr                 ex : 10.0.0.0/16
#                 classic notation       ex : 10.0.0.0/255.255.0.0
sub set_network()
{
    my $class=shift;
    my $nw=shift;
    my $netmask;

    if ($nw =~ $regex_ipv4_network)
    {	
	$netmask=new Net::Netmask($nw);
	$class->{"netmask_class"}=$netmask;
	$class->{"netmask"}=$netmask->mask();
	$class->{"network"}=$netmask->base."/".$netmask->mask();
	$class->{"broadcast"}=$netmask->broadcast();
	$class->{"cidr"}=$netmask->desc();
	return $class->{"network"};
    }
    else
    {
	return undef;
    }
}

sub get_network()
{
    my $class=shift;
    return $class->{"network"};
}

sub check_network()
{
    my $class=shift;
    my $nw=shift;
    my $nw_base;
    my $nw_base2;

    if ( $nw =~ /([0-9\.]+)\/([0-9\.]+)/ )
    {
        $nw_base=$1;
    }
    if ( $class->{"network"} =~ /([0-9\.]+)\/([0-9\.]+)/ )
    {
        $nw_base2=$1;
    }
    if ( $nw_base eq $nw_base2 )
    {
	return "";
    }
    else
    {
        return "This subnet address $nw_base does not match the subnet network address in the DB:".$nw_base2.".\n";
    }
}


sub get_cidr()
{
    my $class=shift;
    if ($class->{"cidr"})
    { return $class->{"cidr"}; }
    else
    { return undef; }
}

sub get_suffix()
{
    my $class=shift;
    if ($class->{"suffix"})
    { return $class->{"suffix"}; }
    else
    { return undef; }
}


sub inaddr()
{
    my $class=shift;
    my $netmask=$class->{"netmask_class"};
    my @array;
    @array=$netmask->inaddr();
    foreach my $element ( @array )
    {
	return $element;

    }
}

sub get_netmask()
{
    my $class=shift;
    return $class->{"netmask"};
}


sub get_netmask_class()
{
    my $class=shift;
    return $class->{"netmask_class"};
}


sub check_ip()
{
    my $class=shift;
    my $ip=shift;
    my $netmask=$class->{"netmask_class"};
    if (defined($netmask))
    {
	return $netmask->match($ip);
    }
    else
    {
	return undef;
    }
}


sub get_number_by_name($)
{
    my $class=shift;
    my $name=shift;
    my $equipmentList=$class->{"equipmentList"};
    return $equipmentList->get_number_by_name($name);
}

sub add_equipment($)
{
    my $class=shift;
    my $element=shift;
    my $equipmentList=$class->{"equipmentList"};

    if ($class->check_ip($element->get_ip()))
    {
	$equipmentList->add($element);
        my $ip = $element->get_ip();
    }
    else
    {
	return undef;
    }
}

sub get_equipment($)
{
    my $class=shift;
    my $num=shift;
    my $equipmentList=$class->{"equipmentList"};
    return $equipmentList->get($num);
}

sub number_of_equipment()
{
    my $class=shift;
    my $num;
    my $equipmentList=$class->{"equipmentList"};
    return $equipmentList->number_of_equipment();
}

sub to_string()
{
    my $class=shift;
    my $equipmentList=$class->{"equipmentList"};
    my $str="";
    if ($class->{"network"})   { $str.="network:".$class->{"network"}." "; }
    if ($class->{"netmask"})   { $str.="netmask:".$class->{"netmask"}." "; }
    if ($class->{"cidr"})      { $str.="cidr:".$class->{"cidr"}." "; }
    if ($class->{"broadcast"}) { $str.="broadcast:".$class->{"broadcast"}." "; }
    if ($class->{"gateway"})   { $str.="gateway:".$class->{"gateway"}." "; }

    if ($equipmentList->number_of_equipment()>0)
    {
	$str.="Equipment list:\n";
	$str.=$equipmentList->to_string();
    }
    return $str;
}

######################################## NetworkList ########################################

package Bull::CLM::DB::NetworkList;

use strict;
use warnings;


sub new()
{
    my $class=shift;
    my @array;
    my %hash;
    my $self=
    {
	"networks"=> \@array,
	"hash" => \%hash,
    };
    bless $self;
    return $self;
}

sub display_hash()
{
    my $class=shift;
    my %hash;
    my $ref_hash;
    
    $ref_hash=$class->{"hash"};
    %hash=%$ref_hash;

    foreach my $key ( keys %hash )
    {
	print "key : ".$key.", value : ".$hash{$key}."\n";
    }
}

sub del_hash_entry($)
{
    my $class=shift;
    my $name=shift;
    my %hash;
    my $ref_hash;

    $ref_hash=$class->{"hash"};
    %hash=%$ref_hash;
    delete($hash{$name});
    $ref_hash=\%hash;
    $class->{"hash"}=$ref_hash;
}


sub add_hash_entry($$)
{
    my $class=shift;
    my $name=shift;
    my $number=shift;
    my %hash;
    my $ref_hash;

    $ref_hash=$class->{"hash"};
    %hash=%$ref_hash;
    $hash{$name}=$number;
    $ref_hash=\%hash;
    $class->{"hash"}=$ref_hash;
}

sub change_hash_entry($$)
{
    my $class=shift;
    my $current_name=shift;
    my $new_name=shift;
    my $number;
    my %hash;
    my $ref_hash=$class->{hash};
    %hash=%$ref_hash;
    $number=$hash{$new_name};
    $class->del_hash_entry($current_name);
    $class->add_hash_entry($new_name,$number);
}


sub get_ref()
{
    my $class=shift;
    return $class->{"networks"};
}

sub add($)
{
    my $class=shift;
    my $element=shift;
    my @array;
    my $ref_array;

    $ref_array=$class->{networks};
    @array=@$ref_array;
    @array=(@array,$element);

    $class->add_hash_entry($element->get_cidr(),$#array);

    $ref_array=\@array;
    $class->{networks}=$ref_array;
}

sub get($)
{
    my $class=shift;
    my $number=shift;
    my @array;
    my $ref_array;
    $ref_array=$class->{networks};
    @array=@$ref_array;
    return $array[$number];
}

sub cidr_exist($)
{
    my $class=shift;
    my $cidr=shift;
    my $found=0;

    if (defined($class->get_number_by_cidr($cidr))) { $found=1; }
    return $found;
}



sub get_number_by_cidr($)
{

    my $class=shift;
    my $cidr=shift;
    my $number=undef;
    my %hash;
    my $hash_ref=$class->{hash};
    return ${$hash_ref}{$cidr};
}




sub number_of_network()
{
    my $class=shift;
    my $number=0;
    my @array;
    my $ref_array;
    $ref_array=$class->{networks};
    @array=@$ref_array;
    $number=$#array+1;
    return $number;
}


sub to_string()
{
    my $class=shift;
    my $str="";

    for ( my $i=0 ; $i < $class->number_of_network(); $i++)
    {
	my $network=$class->get($i);
	$str.=$network->to_string()."\n";
    }
    return $str;
}

sub getNetwork_from_cidr($)
{
	my $class=shift;
	my $net_cidr=shift;

	my ($network1, $network2);
	for (my $j=0; $j<$class->number_of_network(); $j++ )
	{
		$network2=$class->get($j);
		if ( $network2->get_cidr() eq $net_cidr ) 
		{
		    $network1=$network2;
		}
	}
	return $network1;
}



1;



