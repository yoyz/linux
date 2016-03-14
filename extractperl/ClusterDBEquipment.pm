# @(#) $Id: ClusterDBEquipment.pm,v 1.9.2.4.2.1 2011/09/12 07:10:16 bessonc Exp $
package Bull::CLM::DB::ClusterDBEquipment;


use strict;
use warnings;
use Bull::CLM::DB::ClusterDB;
use Bull::CLM::Trace;
use Bull::CLM::History;
use Bull::CLM::DB::DNSVIP;


my $query_mngt="
SELECT E.name 
FROM equipment E 
INNER JOIN equipment_ftype EF ON (E.id=EF.equipment_id) 
AND EF.ftype_name='mngt'
;";

#Count node_profile_device
my $query_count_node_profile_device="
SELECT COUNT (EPP.id) 
FROM equipment_port_profile EPP 
WHERE EPP.equipment_adapter_profile_id 
IN (SELECT EAP.id FROM equipment_adapter_profile EAP WHERE EAP.equipment_profile_id 
IN (SELECT EP.id FROM equipment_profile EP INNER JOIN equipment E ON (E.equipment_profile_id = EP.id)
AND E.type='node'))
;";

#Return subnet,function
my $query_network="
SELECT IPN.subnet, IPN.gateway, IPN.role AS function, N.techno, IPN.type, IPN.id, IPN.ip_network_id, IPN.suffix, IPN.name, IPN.vpartition
FROM network N, ip_network IPN 
WHERE IPN.type!='range'
AND IPN.subnet is not null
AND IPN.role IN ('mn','ic','ic-lustre','bb','serial','nfs','lk','lkn')
AND IPN.network_id = N.id
ORDER BY IPN.role, IPN.id, IPN.ip_network_id, IPN.subnet
;"; # where subnet='10.6.0.0/16'";

#GATEWAY
my $query_name_ip_gateway="
SELECT 1,1,'gw-' || IPNET.name as name, IPNET.subnet, IPNET.gateway 
FROM ip_network IPNET, network NET 
WHERE IPNET.type!='range' 
AND IPNET.gateway is not null 
AND IPNET.network_id = NET.id 
AND IPNET.role IN ('mn','ic','ic-lustre','bb','serial','nfs') 
ORDER BY IPNET.name, IPNET.id, IPNET.ip_network_id, IPNET.subnet
;";

#NODE
#Return name,subnet,ipaddr,hwaddr,type,rank_number,port_number
my $query_name_ip_NodePort="
SELECT E.id,EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, EAP.slot, EPP.rank, EPP.iname, EF.ftype_name 
FROM equipment E 
INNER JOIN equipment_port EP ON (E.id=EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON (EP.equipment_port_profile_id=EPP.id)
INNER JOIN equipment_adapter_profile EAP ON (EPP.equipment_adapter_profile_id=EAP.id)
LEFT OUTER JOIN equipment_ftype EF ON (E.id = EF.equipment_id) 
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id=EP.id )
INNER JOIN ip_network IPN ON (IP.ip_network_id=IPN.id)
INNER JOIN network NET ON (IPN.network_id=NET.id)
WHERE E.type='node' AND EF.ftype_name NOT IN ('mds', 'oss', 'nfs')  
ORDER BY IPN.role IN ('mn','ic','ic-lustre','bb','lk','lkn'), IP.address, E.id, E.name, EAP.slot, EAP.rank
;";

my $query_name_ip_NodePort_without_shadow="
SELECT E.id,EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, EAP.slot, EPP.rank, EPP.iname, EF.ftype_name
FROM equipment E
INNER JOIN equipment_port EP ON (E.id=EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON (EP.equipment_port_profile_id=EPP.id)
INNER JOIN  equipment_adapter_profile EAP on (EPP.equipment_adapter_profile_id=EAP.id)
LEFT OUTER JOIN equipment_ftype EF ON (E.id = EF.equipment_id) 
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id=EP.id )
INNER JOIN ip_network IPN ON (IP.ip_network_id=IPN.id)
INNER JOIN network NET ON (IPN.network_id=NET.id)
LEFT OUTER JOIN equipment_status ESTAT ON (ESTAT.equipment_id = E.id AND ESTAT.status_name = 'mode_status' AND ESTAT.value !='cluster_out')
WHERE E.type='node' AND EF.ftype_name NOT IN ('mds', 'oss', 'nfs') 
ORDER BY IPN.role IN ('mn','ic','ic-lustre','bb','lk','lkn'), IP.address, E.id, E.name, EAP.slot, EAP.rank
;";

#NODE WITHOUT NODE_PROFILE_DEVICE
my $query_name_ip_NodePort_without_node_profile_device="
SELECT E.id,EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, 1 as hba_rank, 1 as port_rank , EPP.iname 
FROM equipment E 
INNER JOIN equipment_port EP ON (E.id=EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON (EP.equipment_port_profile_id=EPP.id)
INNER JOIN equipment_adapter_profile EAP ON (EPP.equipment_adapter_profile_id=EAP.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id=EP.id )
INNER JOIN ip_network IPN ON (IP.ip_network_id=IPN.id)
INNER JOIN network NET ON (IPN.network_id=NET.id)
WHERE E.type='node'
ORDER BY IPN.role IN ('mn','ic','ic-lustre','bb','lk','lkn'), IP.address, E.id, E.name, EAP.slot, EAP.rank
;";

my $query_name_ip_NodePort_without_node_profile_device_without_shadow="
SELECT E.id,EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, 1 as hba_rank, 1 as port_rank , EPP.iname 
FROM equipment E
INNER JOIN equipment_port EP ON (E.id=EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON (EP.equipment_port_profile_id=EPP.id)
INNER JOIN  equipment_adapter_profile EAP on (EPP.equipment_adapter_profile_id=EAP.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id=EP.id )
INNER JOIN ip_network IPN ON (IP.ip_network_id=IPN.id)
INNER JOIN network NET ON (IPN.network_id=NET.id)
LEFT OUTER JOIN equipment_status ESTAT ON (ESTAT.equipment_id = E.id AND ESTAT.status_name = 'mode_status' AND ESTAT.value !='cluster_out')
WHERE E.type='node' 
ORDER BY IPN.role IN ('mn','ic','ic-lustre','bb','lk','lkn'), IP.address, E.id, E.name, EAP.slot, EAP.rank
;";


#HW MANAGER
#Return name,subnet,ipaddr,hwaddr,type
my $query_name_ip_HwManagerPort="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function 
FROM equipment E
INNER JOIN equipment_port EP ON (EP.equipment_id = E.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id = EP.id)
INNER JOIN ip_network IPN ON (IP.ip_network_id = IPN.id)
INNER JOIN network N on (IPN.network_id=N.id)
WHERE E.type IN ('bmc', 'cmc', 'colddoor','rmc') 
ORDER BY IP.address, E.type, E.id, E.name
;";


my $query_name_ip_HwManagerPort_without_shadow="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function
FROM equipment E
INNER JOIN equipment_port EP ON (EP.equipment_id = E.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id = EP.id)
INNER JOIN ip_network IPN ON (IP.ip_network_id = IPN.id)
INNER JOIN network N on (IPN.network_id=N.id)
LEFT OUTER JOIN equipment_status ESTAT ON (ESTAT.equipment_id = E.id AND ESTAT.status_name = 'mode_status' AND ESTAT.value!='cluster_out')
WHERE E.type IN ('bmc', 'cmc', 'colddoor','rmc') 
ORDER BY IP.address, E.type, E.id, E.name
;";


#DISK ARRAY
#Return name,ctrl_rank,port_rank,subnet,ipaddr,hwaddr,type
my $query_name_ip_daPort="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, EAP.slot, EPP.rank 
FROM equipment E
INNER JOIN equipment_port EP ON(E.id = EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON(EP.equipment_port_profile_id = EPP.id)
INNER JOIN equipment_adapter_profile EAP ON(EPP.equipment_adapter_profile_id = EAP.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id = EP.id)
INNER JOIN ip_network IPN ON (IP.ip_network_id = IPN.id)
INNER JOIN network N on (IPN.network_id=N.id)
WHERE E.type ='disk_array'
;";


my $query_name_ip_daPort_without_shadow="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, EP.type, IPN.suffix, IPN.role AS function, EAP.slot, EPP.rank 
FROM equipment E
INNER JOIN equipment_port EP ON(E.id = EP.equipment_id)
INNER JOIN equipment_port_profile EPP ON(EP.equipment_port_profile_id = EPP.id)
INNER JOIN equipment_adapter_profile EAP ON(EPP.equipment_adapter_profile_id = EAP.id)
LEFT OUTER JOIN ip_address IP ON (IP.equipment_port_id = EP.id)
INNER JOIN ip_network IPN ON (IP.ip_network_id = IPN.id)
INNER JOIN network N on (IPN.network_id=N.id)
LEFT OUTER JOIN equipment_status ESTAT ON (ESTAT.equipment_id = E.id AND ESTAT.status_name = 'mode_status' AND ESTAT.value!='cluster_out')
WHERE E.type ='disk_array'
;";


#SWITCH
#Return name,subnet,ipaddr,hwaddr,type
my $query_name_ip_switch="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function
FROM equipment E LEFT OUTER JOIN equipment_status ES ON (ES.equipment_id=E.id and ES.status_name='fusion_status'), ip_network IPN, ip_address IP, equipment_port EP, network N
WHERE E.type='switch_eth' 
AND (ES.value IS NULL or ES.value!='slave')
AND E.id=EP.equipment_id
AND EP.id=IP.equipment_port_id
AND IP.ip_network_id=IPN.id
AND IPN.network_id=N.id
AND IPN.role IN ('mn','ic','bb','serial','nfs')
UNION SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function
FROM equipment E, ip_network IPN, ip_address IP, equipment_port EP, network N
WHERE E.type IN ('switch_power','switch_ib')
AND E.id=EP.equipment_id
AND EP.id=IP.equipment_port_id
AND IP.ip_network_id=IPN.id
AND IPN.network_id=N.id
AND IPN.role IN ('mn','ic','ic-lustre','bb','serial','nfs')
;";


my $query_name_ip_switch_without_shadow="
SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function
FROM equipment E LEFT OUTER JOIN equipment_status ES ON (ES.equipment_id=E.id and ES.status_name='fusion_status'), ip_network IPN, ip_address IP, equipment_port EP, network N, equipment_status ESTAT
WHERE E.type='switch_eth' 
AND (ES.value IS NULL or ES.value!='slave')
AND E.id=EP.equipment_id
AND EP.id=IP.equipment_port_id
AND IP.ip_network_id=IPN.id
AND IPN.network_id=N.id
AND IPN.role IN ('mn','ic','ic-lustre','bb','serial','nfs')
AND ESTAT.equipment_id = E.id 
AND ESTAT.status_name = 'mode_status' 
AND ESTAT.value != 'cluster_out'
UNION SELECT E.id, EP.id, E.name, IPN.subnet, IP.address, EP.hwaddr, E.type, IPN.suffix, IPN.role AS function
FROM equipment E, ip_network IPN, ip_address IP, equipment_port EP, network N, equipment_status ESTAT
WHERE E.type IN ('switch_power','switch_ib')
AND E.id=EP.equipment_id
AND EP.id=IP.equipment_port_id
AND IP.ip_network_id=IPN.id
AND IPN.network_id=N.id
AND IPN.role IN ('mn','ic','ic-lustre','bb','serial','nfs')
AND ESTAT.equipment_id = E.id 
AND ESTAT.status_name = 'mode_status' 
AND ESTAT.value != 'cluster_out'
;";

#CLUSTERIPV_HOST
my $cluster_ipv_host="
SELECT MP.name, IP.address 
FROM metaservice_port MP, ip_address IP 
WHERE MP.type='alias' 
AND MP.ip_address_id = IP.id
;";


#CLUSTERIPV_VM
#Return name,subnet,ipaddr,hwaddr,service???
my $query_cluster_ipv_vm="
SELECT 1,MP.id, MP.name, IPN.subnet, IP.address, MP.hwaddr 
FROM metaservice_port MP, ip_address IP, ip_network IPN 
WHERE (MP.type='vm' or MP.type='vmgeneric') 
AND MP.ip_address_id = IP.id 
AND IP.ip_network_id = IPN.id 
ORDER BY MP.id, MP.name
;";

my $query_analyze_ports="
SELECT IP.address, EPP.iname 
FROM ip_address IP, equipment_port_profile EPP, equipment_port EP, equipment E 
WHERE E.id = EP.equipment_id 
AND EP.id = IP.equipment_port_id 
AND EP.equipment_port_profile_id = EPP.id 
AND IP.address is not null 
AND E.type='node' ORDER BY IP.address
;";

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
	"warnsum"		  => 0,
	"enable_lk"		  => 0
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

sub set_enable_lk($)
{
    my $class = shift;
    my $opt = shift;
    $class->{"enable_lk"} = $opt;
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
	print("\033[1m");
        clmError($fct, "Check these network(s) configuration consistency (netmask and network address): $network_error \n");
	print("\033[0m");
        exit 1;
        #die "Check these network(s) configuration consistency (netmask and network address):\n".$network_error."\n";
	
    }
    $class->{"networkList"}=$networkList;
    clmVerbose($fct, $networkList->to_string());
    if ($networkList->number_of_network()<1)
    {
	print("\033[1m");
	clmOutput($fct, "Number of network : ".$networkList->number_of_network()."\n");
	clmError($fct, "no network with CIDR found, please apply ipallocation command.\n");
	print("\033[0m");
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
	$self->{'warnsum'}++;
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
    my ($eq_id,$nic_id,$shortname,$cidr,$ip,$mac,$type,$suffix,$network_function,$rank_number,$port_number,$iname)=(@$row_ref);
    my $imported=0;
    my $i=0;
    my $equipment_type=$equipment->get_type();

    my $nic=Bull::CLM::DB::NetworkInterface::new();

    if (defined($mac)) { $mac=~s/ //g; }				
    my $name=$shortname;
    
    # name needs to be disk_array.name + 'c' + disk_array_port.ctrl_rank + 'p' + disk_array_port.port_rank

#    $nic->set("equipment-name",$equipment->get_name());
    $nic->set("function",$network_function);

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
    $nic->set("eq_id",$eq_id);
    $nic->set("nic_id",$nic_id);
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
	my ($eq_id,$nic_id,$shortname,$cidr,$ip,$mac,$type,$suffix,$network_function,$rank_number,$port_number,$iname,$ftype)=(@$row_ref);
	
	# Error handling of an non fully defined equipment
	# ( ip or name or cidr ) not defined !!!
	if (!(defined($ip)   and 
	      defined($shortname) and
	      defined($cidr)))
	{

	    $str_err="equipment ";
	    if (!defined($shortname)) { $str_err.="name: UNKNOWN , "; }
	    else { $str_err.="name: $shortname, "; }
	    if (!defined($ip)) { $str_err.="ip : UNKNOWN , "; }
	    else { $str_err.="ip: $ip, "; }
	    if (!defined($cidr)) { $str_err.="cidr: UNKNOWN  "; }
	    else { $str_err.="cidr: $cidr "; }
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
	    $equipment->set("eq_id",$eq_id);
	}
	else
	{
	    clmOutput($fct,"Equipment without a correct name ? : $sql_query");
	    next;
	}
	
	my $nic=undef;
	if (!($ip=~ $regex_ipv4))
	{
	    clmVerbose($fct,"Error in sql query : $sql_query");
	    clmVerbose($fct,"Error name or ip not defined in clusterdb");
	    clmVerbose($fct,"ip=[$ip] name=[$shortname]");
	    next;
	}
	$nic=$class->create_network_interface($equipment,$row_ref);
	$nic->set("vip",0);

	if (defined($ftype)) {
		$equipment->set("ftype", $ftype);
		$nic->set("ftype", $ftype);
	}

	# LK/LKN networks are ignored by the extract tools
	if ((defined($network_function) and $network_function !~ /lk[n]?/) or $equipment_type =~ /VM|GATEWAY/) {
	    $equipment->add_interface($nic);
	    $class->add_network_interface_to_network_list($nic);
	}
	#print $equipment->to_string()."\n";
    }
    $error_on_mac_address=$class->{error_on_mac_address};
    if ($error_on_mac_address>1)
    {
	clmWarning($fct,"$error_on_mac_address wrong mac addresses were detected. Activate traces to get the entire list.");
	$class->{'warnsum'}++;
    }
}



sub import_host()
{
    my $self=shift;
    my $include_shadow = shift;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_host");

    if ($self->count_node_profile_device())
    {
	if ($include_shadow) {
		$self->import_equipment($query_name_ip_NodePort,"HOST");
	} else {
		$self->import_equipment($query_name_ip_NodePort_without_shadow,"HOST");
	}
    }
    else # There is no node_profile_device, failing back to default behaviour
    {
	clmWarning($fct,"the 'node_profile_device' table is empty");
	$self->{'warnsum'}++;
	if ($include_shadow) {
		$self->import_equipment($query_name_ip_NodePort_without_node_profile_device,"HOST");
	} else {
		$self->import_equipment($query_name_ip_NodePort_without_node_profile_device_without_shadow,"HOST");
	}
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
    my $include_shadow = shift;
    my $import_query = $query_name_ip_HwManagerPort_without_shadow;
    my $fct = (caller(0))[3];
    clmDebug($fct,"import_hardwareManager");
    if ($include_shadow) {
	$import_query = $query_name_ip_HwManagerPort;
    }
    $class->import_equipment($import_query,"HARDWAREMANAGER");
}


sub import_diskArray()
{
    my $class=shift;
    my $include_shadow = shift;
    my $fct = (caller(0))[3];
    my $import_query = $query_name_ip_daPort_without_shadow;
    clmDebug($fct,"import_diskArray");
    if ($include_shadow) {
	$import_query = $query_name_ip_daPort;
    }
    $class->import_equipment($import_query,"DISKARRAY");
}

sub import_switch()
{
    my $class=shift;
    my $include_shadow = shift;
    my $fct = (caller(0))[3];
    my $import_query = $query_name_ip_switch_without_shadow;
    clmDebug($fct,"import_switch");
    if ($include_shadow) {
	$import_query = $query_name_ip_switch;
    }
    $class->import_equipment($import_query,"SWITCH");
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
	clmVerbose($fct,"Network : ".$network->get_network());
	clmVerbose($fct,"\tcidr : ".$network->get_cidr());
	clmVerbose($fct,"\tnetmask : ".$network->get_netmask());
	clmVerbose($fct,"\tfunction : ".$network->get_function());
	clmVerbose($fct,"\tinaddr: ".$network->inaddr());
	clmVerbose($fct,"\tnumber of equipment : ".$network->number_of_equipment());
    }
}

1;
