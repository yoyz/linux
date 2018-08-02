# @(#) $Id: ClusterDBEquipment.pm,v 1.9.4.5 2011/05/24 14:52:58 peyrardj Exp $
package Bull::CLM::DB::ClusterDBEquipment;


use strict;
use warnings;
use Bull::CLM::DB::ClusterDB;
use Bull::CLM::Trace;
use Bull::CLM::History;
use Bull::CLM::DB::DNSVIP;

#Count node_profile_device
my $query_count_node_profile_device="
SELECT 
count(*) 
FROM 
node_profile_device
";

#Return subnet,function
my $query_network="
select
subnet,gateway,function,techno,type,id,network_id,suffix,name,vpartition
from
network
where type!='range'
AND
subnet is not null
order by network.function,id,network_id,network.subnet
"; # where subnet='10.6.0.0/16'";




#GATEWAY
my $query_name_ip_gateway="
SELECT
'gw-' || name as name,subnet,gateway
FROM
network
WHERE
type!='range' 
AND 
gateway is not null
ORDER BY network.name,id,network_id,network.subnet
";



#NODE
#Return name,subnet,ipaddr,hwaddr,type,rank_number,port_number
my $query_name_ip_NodePort="
select node.name,network.subnet,node_port.ipaddr,node_port.hwaddr,node_port.type,network.suffix,network.function,node_port.hba_rank,node_port.port_rank,node_profile_device.interface_name
from
node_port,node,network,node_profile_device
where
node_port.node_id = node.id and network.id=node_port.network_id and node_profile_device.id=node_port.node_profile_device_id
order by
network.function IN ('mn','ic','bb'),node_port.ipaddr,node.id,node.name,node_port.hba_rank,node_port.port_rank
";

#NODE WITHOUT NODE_PROFILE_DEVICE
my $query_name_ip_NodePort_without_node_profile_device="
select node.name,network.subnet,node_port.ipaddr,node_port.hwaddr,node_port.type,network.suffix,network.function,1 as hba_rank,1 as port_rank
from
node_port,node,network
where
node_port.node_id = node.id and network.id=node_port.network_id
order by
node_port.ipaddr,node.id,node.name,node_port.hba_rank,node_port.port_rank
";

my $old_query_name_ip_NodePort="
select node.name,network.subnet,node_port.ipaddr,node_port.hwaddr,node_port.type,network.suffix,network.function,node_port.hba_rank,node_port.port_rank
from
node_port,node,network
where
node_port.node_id = node.id and network.id=node_port.network_id
order by
node.id,node_port.ipaddr,node.name,node_port.hba_rank,node_port.port_rank
";

#HW MANAGER
#Return name,subnet,ipaddr,hwaddr,type
my $query_name_ip_HwManagerPort="
select
hwmanager.name,network.subnet,hwmanager_port.ipaddr,hwmanager_port.hwaddr,hwmanager.type,network.suffix,network.function
from
hwmanager_port,hwmanager,network
where
hwmanager_port.hwmanager_id = hwmanager.id and hwmanager_port.network_id=network.id
order by
hwmanager_port.ipaddr,hwmanager.type,hwmanager.id,hwmanager.name
;";

#DISK ARRAY
#Return name,ctrl_rank,port_rank,subnet,ipaddr,hwaddr,type
my $query_name_ip_daPort="
select
disk_array.name,network.subnet,disk_array_port.ipaddr,disk_array_port.hwaddr,disk_array_port.type,network.suffix,network.function,disk_array_port.ctrl_rank,disk_array_port.port_rank
from
disk_array,disk_array_port,network
where
disk_array_port.network_id=network.id and disk_array_port.disk_array_id=disk_array.id
;";

#SWITCH
#Return name,subnet,ipaddr,hwaddr,type
my $query_name_ip_switch="
select
switch.name, network.subnet, switch_mngt_port.ipaddr, switch_mngt_port.hwaddr, switch.type, network.suffix, network.function
from
switch,switch_mngt_port,network
where
switch_mngt_port.belong_switch_id=switch.id and switch_mngt_port.network_id=network.id
and
fusion_switch_id is null
;";


#CLUSTERIPV_HOST
my $cluster_ipv_host="
select
name, ipaddr
from metaservice_port
where
metaservice_port.type='alias'
;";


#CLUSTERIPV_VM
#Return name,subnet,ipaddr,hwaddr,service???
my $query_cluster_ipv_vm="
select
M_port.name,nw.subnet,M_port.ipaddr,M_port.hwaddr
from metaservice_port M_port,network nw
where
M_port.type='vm' and M_port.network_id=nw.id
order by
M_port.id,M_port.name
;";

my $query_analyze_ports="
select node_port.ipaddr, (select interface_name from node_profile_device n1 where n1.id=node_port.node_profile_device_id) as interf
from
 node_port, node, network, node_ftype nf
where   
 node_port.node_id = node.id
 and network.id=node_port.network_id
 and node.id=nf.node_id 
 and not (node_port.ipaddr is null)
order by
 node_port.ipaddr;
";

# and nf.comment=\'mds\' 
# and node_port.function=\'ic\'


#  and node_port.function='ic'
# and nf.comment='mds' 

#my $regex_ipv4        = "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)";
#my $regex_ipv4_network= "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?\/[0-9]|[0-9][0-9])";
#my $regex_name        = "^[a-zA-Z0-9-_]+";


my $regex_ipv4_network=qr!(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?\/[0-9]|[0-9][0-9])!;

my $regex_ipv4=qr!(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)!;

my $regex_name=qr!^[a-zA-Z0-9-_]+!;

my $regex_mac=qr!^[a-fA-f0-9][a-fA-f0-9]:.+!;



sub new()
{
    my $class=
    {
	"equipmentNetwork"        => undef,
	"clusterdb"               => undef,
	"error_on_mac_address"    => 0,
    };

    bless $class;

    return $class;
}

sub set_dbhandler($)
{
    my $class=shift;
    my $ref_clusterdb=shift;
    $class->{"clusterdb"}=$ref_clusterdb;
}

sub set_clusterdb($)
{
    my $class=shift;
    my $cdb=shift;
    $class->set_dbhandler($cdb);
}

sub set_equipmentNetwork($)
{
    my $class=shift;
    my $ref_equipmentNetwork=shift;
    $class->{"equipmentNetwork"}=$ref_equipmentNetwork;
}



sub get_clusterdb()
{
    my $class=shift;
    return $class->{"clusterdb"};
}


sub get_equipmentNetwork()
{
    my $class=shift;
    return $class->{"equipmentNetwork"};
}




sub get_networkList()
{
    my $class=shift;
    return $class->get_equipmentNetwork()->get_networkList();
}

sub get_equipmentList()
{
    my $class=shift;
    return $class->get_equipmentNetwork()->get_equipmentList();
}



sub warning_could_not_import_equipment()
{
   my $self=shift;
   my $name=shift;
   my $fct = (caller(0))[3];
   my $str_err="";
   if (defined($name))
   {
       $str_err="equipment name : [".$name."] could not be extract, check your database";
       clmHistoryMsg($str_err);
       clmDebug($fct,$str_err);
   }
}



# Return the number of network imported ( positive number )
#        0 in another case
sub import_network()
{
    my $class=shift;

    my $fct = (caller(0))[3];     
#    my $networkList=Bull::CLM::DB::NetworkList::new();
    my $networkList=$class->get_networkList();
    my $clusterDB=$class->get_clusterdb();

    my $ref_networkHash;
    my $ref_list_nw;
    my $row_ref;
    my $str_err="";
    my $current_network_number=0; # to have a unique network number 
    my $current_network_function="";
    my @list_nw=();
    my $info_not_found=0;
    my $network_error="";
    my $dnsvip;
    my $dnsdomain;

    clmOutput($fct,"Reading Network table");
#    clmOutput($fct,"sql : $query_network");
    #clmHistoryMsg("sql query : $query_network");
    $ref_list_nw=$clusterDB->fetchallArrayref($query_network);

    $dnsvip=Bull::CLM::DB::DNSVIP::new();
    $dnsvip->set_dbhandler($clusterDB);
    $dnsdomain=$dnsvip->get_dns_domain_from_cluster();



    @list_nw=@$ref_list_nw;

    if (@list_nw==-1)
    { die "Can not import Network, no network found in database"; }

    foreach $row_ref (@list_nw)
    {
	my ($nw,$gateway,$function,$techno,$type,$id,$network_id,$suffix,$name,$vlan)=(@$row_ref);
	
	if (defined($id)       and
	    defined($type))
	{
	    my $network=Bull::CLM::DB::Network::new();
	    
	    if ($id)       { $network->set_id($id);             }	    
	    if ($nw)       { $network->set_network($nw);        }
	    if ($function) { $network->set_function($function); }
	    if ($techno)   { $network->set_techno($techno);     }
	    if ($gateway)  { $network->set_gateway($gateway);   }
	    if ($type)     { $network->set_type($type);         }
	    if ($name)     { $network->set_name($name);         }
	    if ($vlan)     { $network->set_vlan($vlan);         }
	    if ($suffix)   { $network->set_suffix($suffix);     }
	    #if ($type=~/secondary/ and $network_id) 
	    
	    if ($network_id) 
	    { $network->set_parent_id($network_id); }
	    
	    $network->set_dnsdomain($dnsdomain);
	    
	    $network_error.=$network->check_network($nw);
	    
	    $networkList->add($network);
	}
	else
	{
	    $str_err="Type not defined for this network:$nw , id: $id.";
	    if ($info_not_found==0)
	    {
		clmOutput($fct,$str_err);
		$info_not_found=1;
	    }
	    else {
		clmVerbose($fct,$str_err);
	    }
	    clmHistoryMsg($str_err);
	}
    }
    
    if ( ! ( $network_error eq "" ))
    {
        clmError($fct, "Check these network(s) configuration consistency (netmask and network address): $network_error \n");
        exit 1;
        #die "Check these network(s) configuration consistency (netmask and network address):\n".$network_error."\n";
	
    }
    $class->{"networkList"}=$networkList;
    clmOutput($fct, $networkList->to_string());
    if ($networkList->number_of_network()<1)
    {
	clmOutput($fct, "Number of network : ".$networkList->number_of_network()."\n");
	clmOutput($fct, "No network with CIDR found please apply ipallocation command \n");
	exit 1;
    }
    return $current_network_number;
}




sub add_network_interface_to_network_list($)
{    
    my $self=shift;
    my $nic=shift;
    my $fct = (caller(0))[3];     
    my $cidr=$nic->get("cidr");
    my $name=$nic->get("name");

    my $networkList=$self->get_networkList();
    my $equipmentList=$self->get_equipmentList();
    my $network=undef;
    my $imported=0;
    my $i=0;

    #print "CIDR-eq  : [".$cidr."]\n";
    #print $equipment->to_string()."\n";
    #print $nic->to_string()."\n";
    #print "CIDR:".$cidr."\n";    
    #print "NCIDR:";

#    for ($i=0; $i < $networkList->number_of_network() ; $i++)
    if ($networkList->cidr_exist($cidr))
    {
	#print "import :".$name."\n";
	$i=$networkList->get_number_by_cidr($cidr);
	$network=$networkList->get($i);


	if ($network->get_cidr() and
	    $cidr                and
	    $network->get_cidr() eq $cidr)
	{
	    my $netname=$network->get_name();
	    $netname=~s/_/-/g;		
	    $nic->set("network-name",$netname);
            if ($network->get_suffix()) { $nic->set("network-suffix",$network->get_suffix()); }
#	    $nic->set("equipment-name",$equipment_name);

            #print "eq cidr\n";

	    # check if an equipment of the same name is already in the list of interface.
	    # it could happen when multiple interface for the same host are on the same subnet
	    # in this case, we modify the name of the interface


	    if ( $nic->get_type() eq "HOST")
	    {
		#my $nic_number_in_networkInterfaceList=$network->get_number_by_name($nic->get_name());
		my $nic_number_in_networkInterfaceList=undef;
		if (defined($nic_number_in_networkInterfaceList))
		{
		    print "1\n";
		    my $oldnic=$network->get_equipment($nic_number_in_networkInterfaceList);
		    $oldnic->set_name($oldnic->get_name()."-".$oldnic->get("iname"));
		    
		    $nic->set_name($nic->get_name()."-".$nic->get("iname"));
		    
		}
	    }

	    if ( $network->add_equipment($nic))
	    {
		$imported=1;
	    }
	}
    }
    if ($imported==0) 
    { 
	clmWarning($fct,"$name could not be imported in any network. Is the cidr of the equipment [".$cidr."] valid?"); 
    }
    return $imported;
}


# import_equipment( sql_query, equipmentType )
# 

sub fetch_or_create_equipment($$)
{
    my $class=shift;
    my $shortname=shift;
    my $equipment_type=shift;
    my $equipment=undef;
    my $equipmentList=$class->get_equipmentList();
    

    my $found=$equipmentList->name_exist($shortname);
    #if not prestent in equipment list
    if (!$found)
    {
	$equipment=Bull::CLM::DB::Equipment::new();
	$equipment->set_name($shortname);
	$equipment->set_type($equipment_type);
	$equipmentList->add($equipment);
	#print "Add equipment to list : $i $shortname\n";
    }
    else
    {
	my $number_in_equipmentList=$equipmentList->get_number_by_name($shortname);
	$equipment=$equipmentList->get($number_in_equipmentList);
	#print "fetch from list : $i numberOfInt : ".$equipment->number_of_interface()." ".$shortname."\n";
    }
    return $equipment;
}

sub create_network_interface($$)
{
    my $class=shift;
    my $equipment=shift;
    my $row_ref=shift;

#    my $equipment_type=shift;
    my $error_on_mac_address=$class->{error_on_mac_address};
    my $fct = (caller(0))[3];
    my ($shortname,$cidr,$ip,$mac,$type,$suffix,$network_function,$rank_number,$port_number,$iname)=(@$row_ref);
    my $imported=0;
    my $i=0;
    my $equipment_type=$equipment->get_type();

    my $nic=Bull::CLM::DB::NetworkInterface::new();

    if (defined($mac)) { $mac=~s/ //g; }				
    my $name=$shortname;
    
    # name needs to be disk_array.name + 'c' + disk_array_port.ctrl_rank + 'p' + disk_array_port.port_rank

    $nic->set("equipment-name",$equipment->get_name());
    $nic->set("function",$network_function);
#    clmOutput($fct,"function : $network_function");
#    exit 0;

    if ($equipment_type eq "HOST")
    {
	if ( defined($suffix))
	{
	    #$name=sprintf("%s-%s",$name,$suffix);
	    $name=$name."-".$suffix;
	}
	$nic->set("iname",$iname);
    }
    
    
#    elsif ($equipment_type eq "GATEWAY")
#    {
#	$name=~s/_/-/g;
#	$name="gw-".$name;
#    }
    
    elsif ($equipment_type eq "DISKARRAY") 
    {
	if (	defined($rank_number) and 
		defined($port_number))
	{
#	    $name=sprintf("%sc%sp%s",$name,$rank_number,$port_number); 
	    $name=$name."c".$rank_number."p".$port_number;
	}
    }
    
    $nic->set_name($name);
    $nic->set("equipment_name",$shortname);
    $nic->set_ip($ip);
    $nic->set("cidr",$cidr);
    if (defined($equipment_type)) { $nic->set("type",$equipment_type); }
    if (defined($type))           { $nic->set("port_type",$type); }
    if (defined($rank_number))    { $nic->set("rank_number",$rank_number); }
    else                          { $nic->set("rank_number",1); }
    if (defined($port_number))    { $nic->set("port_number",$port_number); }
    else                          { $nic->set("port_number",1); }
    
    if (defined($mac))
    { 
	my $l_mac=$nic->set_mac($mac);
	if (! $l_mac)  
	{
	    # the mac address is not valid	
	    if ($error_on_mac_address)
	    {
		clmDebug($fct,"The mac address [$mac] is not valid [$name].");
	    }
	    else 
	    {
		clmVerbose($fct,"The mac address [$mac] is not valid [$name].");
		clmDebug($fct,"The mac address [$mac] is not valid [$name].");
	    }
	    $error_on_mac_address++;
	    $class->{error_on_mac}=$error_on_mac_address;
	}
    }
    else
    {
	clmDebug($fct,"No mac address defined for [$name]");
    }
    return $nic;
}

sub count_node_profile_device()
{
    my $class=shift;
    my $clusterDB=$class->get_clusterdb();
    my $query=$query_count_node_profile_device;
    my $ref_count;
    my $count=0;
    my $row_ref;
    my @list_count;
    $ref_count=$clusterDB->fetchallArrayref($query);
    @list_count=@$ref_count;
    foreach $row_ref (@list_count)
    {
	($count)=@$row_ref;
    }
    return $count;
}

sub import_equipment($$)
{
    my $class=shift;
    my $sql_query=shift;        # My sql query for an equipment type
    my $equipment_type=shift;   # My equipment type "Host" "Vm"...
    my $fct = (caller(0))[3];
    my $clusterDB=$class->get_clusterdb();

    my $equipmentList=$class->get_equipmentList();
    my $networkList=$class->get_networkList();

    my $type="unknow"; 

    my $equipment=undef;
    my $nic=undef;

    my $info_not_found=0;
    my $error_on_mac_address=0;
    my $i=0;

    my $ref_list_ip_name;
    my $str_err="";

    #clmHistoryMsg("sql query : $sql_query");
    $ref_list_ip_name=$clusterDB->fetchallArrayref($sql_query);
    
    #$networkList->display_hash();
#    exit 0;

    foreach my $row_ref (@$ref_list_ip_name)
    {
	$i++;
#	if ($i==4000) { exit 0; }
	my ($shortname,$cidr,$ip,$mac,$type,$suffix,$network_function,$rank_number,$port_number,$iname)=(@$row_ref);
	
	# Error handling of an non fully defined equipment
	# ( ip or name or cidr ) not defined !!!
	if (!(defined($ip)   and 
	      defined($shortname) and
	      defined($cidr)))
	{
	    #$str_err="Warning, equipment name : [".$name."] could not be exported";
	    $str_err="equipment ";
	    if (!defined($shortname)) { $str_err.="name :** UNKNOWN **, "; }
	    else { $str_err.="name :$shortname, "; }
	    if (!defined($ip)) { $str_err.="ip :** UNKNOWN **, "; }
	    else { $str_err.="ip :$ip, "; }
	    if (!defined($cidr)) { $str_err.="cidr :** UNKNOWN ** "; }
	    else { $str_err.="cidr :$cidr "; }
	    $str_err.="could not be exported";
	    
	    clmHistoryMsg($str_err);
	    if ($info_not_found==0) {
		clmDebug($fct,$str_err);
		#$str_err="Warning, sql query : $sql_query";
		#clmDebug($fct,$str_err);
	    }
	    else 
	    {
		clmDebug($fct,$str_err);
	    }
	    $info_not_found=1;
	    next;
	}
	my $equipment=undef;

	# ip, name, cidr defined
	if ($shortname =~ $regex_name)
	{
	    $equipment=$class->fetch_or_create_equipment($shortname,$equipment_type);
	}
	else
	{
	    clmOutput($fct,"Equipment without a correct name ? : $sql_query");
	    next;
	}
	
	my $nic=undef;
	if ($ip=~ $regex_ipv4)
	{
	    $nic=$class->create_network_interface($equipment,$row_ref);
	    $nic->set("vip",0);
	    $equipment->add_interface($nic);
	    $class->add_network_interface_to_network_list($nic);
	}
	else
	{
	    clmOutput($fct,"Error in sql query : $sql_query");
	    clmOutput($fct,"Error name or ip not defined in clusterdb");
	    clmOutput($fct,"ip=[$ip] name=[$shortname]");
	}

    }
    $error_on_mac_address=$class->{error_on_mac_address};
    if ($error_on_mac_address>1)
    {
	clmWarning($fct,"$error_on_mac_address wrong mac addresses were detected. Activate traces to get the entire list.");
    }
}



sub import_host()
{
    my $self=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_host\n");
    if ($self->count_node_profile_device())
    {
	$self->import_equipment($query_name_ip_NodePort,"HOST");
    }
    else # There is no node_profile_device, failing back to default behaviour
    {
	clmOutput($fct,"Warning : The 'node_profile_device' table is empty");
	$self->import_equipment($query_name_ip_NodePort_without_node_profile_device,"HOST");
    }
}


sub import_VM()
{
    my $class=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_vm");
    $class->import_equipment($query_cluster_ipv_vm,"VM");
}



sub import_hardwareManager()
{
    my $class=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_hardwareManager");
    $class->import_equipment($query_name_ip_HwManagerPort,"HARDWAREMANAGER");
}


sub import_diskArray()
{
    my $class=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_diskArray");
    $class->import_equipment($query_name_ip_daPort,"DISKARRAY");
}

sub import_switch()
{
    my $class=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_switch");
    $class->import_equipment($query_name_ip_switch,"SWITCH");
}

sub import_gateway()
{
    my $class=shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_gateway");
    $class->import_equipment($query_name_ip_gateway,"GATEWAY");
}

sub print_equipment()
{
    my $self=shift;
    my $fct = (caller(0))[3];
    my $equipmentList=$self->get_equipmentList();
    for (my $i=0;$i<$equipmentList->number_of_equipment();$i++)
    {
	my $equipment=$equipmentList->get($i);
	clmDebug($fct,$equipment->to_string());
    }

}

sub print_network()
{
    my $self=shift;
    my $fct = (caller(0))[3];
    my $networkList=$self->{"networkList"};
    my $ref_networkList=$networkList->get_ref();
    
    foreach my $network (@$ref_networkList)
    {
	clmOutput($fct,"Network : ".$network->get_network());
	clmOutput($fct,"\tcidr : ".$network->get_cidr());
	clmOutput($fct,"\tnetmask : ".$network->get_netmask());
	clmOutput($fct,"\tfunction : ".$network->get_function());
	clmOutput($fct,"\tinaddr: ".$network->inaddr());
	clmOutput($fct,"\tnumber of equipment : ".$network->number_of_equipment());
    }
}

1;
