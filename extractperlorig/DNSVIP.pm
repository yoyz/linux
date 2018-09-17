# @(#) $Id: DNSVIP.pm,v 1.19.4.2 2011/03/28 11:13:15 peyrardj Exp $
package Bull::CLM::DB::DNSVIP;

use strict;
use warnings;
@INC=(@INC,".");
use Bull::CLM::DB::ClusterDB;
use Bull::CLM::Trace;
use Bull::CLM::History;
use Bull::CLM::DB::NetworkEquipment;
use Bull::CLM::DB::ClusterDBEquipment;;

my $fill_coalesce="-";

my $query_hw_group="select name from hw_group where comment!=''";

my $query_mngt="
SELECT node.name 
FROM
node_ftype,ftype,node 
WHERE
node.id=node_ftype.node_id 
AND
node_ftype.ftype_id=ftype.id 
AND ftype.label='mngt'
";


#drop ACL_table
my $query_count_acl_view="SELECT count(viewname) from pg_catalog.pg_views where viewname='acl_table'";

#create temporary view ACL_table  as
#";
my $query_acl_drop_view="
drop view ACL_table;
";


my $query_hw_group_NAME="
SELECT hgm.ref_id, hgm.target_table
FROM hw_group hg,hw_group_member hgm
WHERE hgm.hw_group_id=hg.id
AND hg.name='__NAME__'
";

my $query_name_ip_from_target_table="
SELECT __TARGET_TABLE__.name, __TARGET_TABLE___port.ipaddr

FROM   __TARGET_TABLE__, __TARGET_TABLE___port

WHERE  __TARGET_TABLE__.id=__REF_ID__ AND
       __TARGET_TABLE___port.__TARGET_TABLE___id=__TARGET_TABLE__.id AND
       __TARGET_TABLE___port.function='mn'
";

my $query_joint_hwgroup_and_members="
SELECT ref_id, target_table

FROM hw_group_member hgm, hw_group hg

WHERE hgm.hw_group_id = hg.id
";

my $query_cluster_ipv="
SELECT M_member.dns_view,M_port.name,M_port.ipaddr,M_host.hw_group_id 
FROM  metaservice M_serv, metaservice_member M_member, metaservice_host M_host, metaservice_port M_port
WHERE M_host.metaservice_port_id=M_port.id
AND M_host.metaservice_id=M_serv.id
AND M_serv.id=M_member.metaservice_id;";
# AND hw_group.name='__NAME__'";

my $query_Metaservice_host_by_ref="
select MP.id, MP.name, MP.ipaddr, Net.subnet
from metaservice_port MP, network Net
where
MP.id=__ID__
and MP.network_id=Net.id
;";

my $query_dns_domain="
SELECT  dns_domain from cluster
";

my $query_metaservice_list="
select distinct id
from metaservice
order by id;
";

#my $metaservices_aliases="
#select MM.dns_view, MP.ipaddr
#from Metaservice_member MM, metaservice M, metaservice_host MH, Metaservice_port MP
#where m.type like 'alias'
#and mm.metaservice_id=M.id
#and MH.metaservice_id=M.id
#and MH.hw_group_id is null
#and mh.metaservice_port_id=MP.id
#;
#";
my $metaservices_aliases="
select MM.dns_view, MP.ipaddr, M.id
from Metaservice_member MM, metaservice M, metaservice_host MH,
Metaservice_port MP
where m.type like 'alias'
and mm.metaservice_id=M.id
and MH.metaservice_id=M.id
and not (MM.dns_view is null)
and mh.metaservice_port_id=MP.id
order by MM.dns_view;
";



# This request checks that a Meta_service is allocated only once for an interface;
my $ACL_config_check1="
select count (*), hwm.ref_id, hwm.target_table, MS.name  || ' metaservice name' as target
from metaservice MS, hw_group_member hwm,
metaservice_host MH
where 
MS.id=mh.metaservice_id
and mh.hw_group_id=hwm.hw_group_id
and (
select count (*)
from metaservice_host MH1
where 
MS.id=mh1.metaservice_id
and mh1.hw_group_id=hwm.hw_group_id)=2
group by hwm.ref_id, hwm.target_table, target
order by hwm.ref_id, hwm.target_table, target
;
";

# This request checks that a service is set only once for an interface;
my $ACL_config_check2="
select count(*), hwm.ref_id, hwm.target_table, MSM.dns_view  || ' dns view' as target
from metaservice_member MSM, hw_group_member hwm,
 metaservice_host MH
 where 
 MSM.metaservice_id=mh.metaservice_id
 and mh.hw_group_id=hwm.hw_group_id 
 and not ( MSM.dns_view = '')
 and (select count(*) 
 from  metaservice_member MSM1,   metaservice_host MH1, hw_group_member hwm1
 where 
 MSM1.dns_view=MSM.dns_view
 and MSM1.metaservice_id=mh1.metaservice_id
 and mh1.hw_group_id=hwm1.hw_group_id and hwm1.ref_id=hwm.ref_id and hwm1.target_table=hwm.target_table 
 )=2
 group by  MSM.dns_view, hwm.ref_id, hwm.target_table
 order by  MSM.dns_view, hwm.ref_id, hwm.target_table
 ;
 ";


# select all metaservice_port linked to a metaservice_host entry
# with a null ipaddr
my $query_ACL_config_check3="
SELECT *            
FROM metaservice_host,metaservice_port
where metaservice_port.id=metaservice_host.metaservice_port_id
AND                 
ipaddr is null      
";                  
                    
                    
# select all the metaservice_host with no metaservice_port associated
my $query_ACL_config_check4="
SELECT *            
FROM                
metaservice_host    
WHERE               
metaservice_port_id is NULL
";                  
                    
# select all metaservice_host
my $query_ACL_config_check5="
SELECT * from metaservice_host
";                  


my $query_acl_view_part0="
create or replace view ACL_table  as 
";

my $query_acl_view_part1="
select distinct __TARGET_TABLE___port.ipaddr as IP
";

my $query_acl_view_part2="
, ( select to_char(MH.metaservice_port_id, 'FM999999')
from metaservice_host MH, hw_group_member hwm2
where
__TARGET_TABLE___port.__TARGET_TABLE___id=hwm2.ref_id 
and hwm2.target_table='__TARGET_TABLE__'
and MH.hw_group_id=hwm2.hw_group_id
and MH.metaservice_id=__ID__) as service__ID__  
";

my $query_acl_view_part3="
from hw_group_member hwm, __TARGET_TABLE___port, network nw
where hwm.target_table='__TARGET_TABLE__'
and __TARGET_TABLE___port.__TARGET_TABLE___id= hwm.ref_id
and __TARGET_TABLE___port.network_id=nw.id
and nw.function='mn'
and hwm.hw_group_id in (select distinct(hw_group_id) from metaservice_host) 
";

my $query_acl_view_read1="
select 
";

my $query_acl_view_read2="
COALESCE (service__ID__, '$fill_coalesce') 
";

my $query_acl_view_read3="
as concat, ip 
"; 

my $query_acl_view_read4="
, COALESCE (service__ID__, '$fill_coalesce') as service__ID__ 
";

my $query_acl_view_read5="
from ACL_table order by 
";

my $query_acl_view_read6="
COALESCE (service__ID__, '$fill_coalesce')
";

my $query_ServiceDnsNames="
SELECT metaservice_id, DNS_view
from Metaservice_member
where not DNS_view like ''
order by metaservice_id, DNS_view
;
";

my $ACL_List;
my $service_nb=0;

##    my $str_service_alias=$self->read_alias_services_view();

sub new()
{
    my $inaddrList=();
    my $self=
    {
	"clusterdb"             => undef,
	"aclEqList"             => undef,
	"inaddrList"            => $inaddrList,
	"service_nb"            => 0,
	"acl_require_fault"     => undef,
        "acl_can_be_created"    => 0,
        "acl_is_created"        => 0,
	"dns_domain_name"       => undef

    };
    bless $self;
    return $self;
}

sub set_dbhandler($)
{
    my $class=shift;
    my $ref_clusterdb=shift;
    $class->{"clusterdb"}=$ref_clusterdb;
}

sub get_list_mngt()
{
    my $self=shift;
    my $clusterDB=$self->{"clusterdb"};
    my $query=$query_mngt;
    my $ref_row_list_mngt=$clusterDB->fetchallArrayref($query);
    my @row_list_mngt=@$ref_row_list_mngt;
    my @ret_list;
    if (@row_list_mngt==-1)
    { die "Can not find mngt node in database"; }
   
    foreach my $row_ref (@row_list_mngt)
    {  
        my ($mngt)=(@$row_ref);
        @ret_list=(@ret_list,$mngt)                                                                                                                                   
    }                                                                                                                                                                 
    return @ret_list;
}




sub get_dns_domain_from_cluster()
{
    my $self=shift;    
    my $clusterDB=$self->{"clusterdb"};
    my $query=$query_dns_domain;
    my $dns_domain="bullx";
    
    if (defined($self->{dns_domain_name})) { return $self->{dns_domain_name}; }

    my $ref_list=$clusterDB->fetchallArrayref($query);
    foreach my $row_ref (@$ref_list)
    {
        my ($name)=(@$row_ref);
        $dns_domain=$name;
    }
    $self->{dns_domain_name}=$dns_domain;
    return $dns_domain;
}


sub get_ipv_by_refid()
{
    #my ($self, $refId) = @_;
    my $self=shift;
    my $refId=shift;
    my $clusterDB=$self->{"clusterdb"};
    my $query=$query_Metaservice_host_by_ref;
    
    $query=~s/__ID__/$refId/g;
    return $clusterDB->fetchallArrayref($query);
}


1;

# This function check two concistency criteria of the database: 
# - a Meta_service is allocated only once for a network interface;
# - a service (dns view) is set only once for a network interface;
# this function need to return 1 in order to create the acl_table
sub check_acl_prerequirements()
{
    my $self=shift;
    my $fct = (caller(0))[3];

    my $ret_value=1;
    my $result1=undef;
    my $result2=undef;

    my $clusterDB=$self->{"clusterdb"};
    my @result_tab1;
    my @result_tab2;
    my $Mquery=$ACL_config_check1;

    # This request checks that a Meta_service is allocated only once
    # for an interface;
    $result1 = $clusterDB->fetchallArrayref($Mquery);
    
    @result_tab1=@$result1;
    
    if ( $#result_tab1 < 0 ) 
    {
	$Mquery=$ACL_config_check2;

	# This request checks that a service is set only once
        # for an interface;
        $result2 = $clusterDB->fetchallArrayref($Mquery);
	@result_tab2=@$result2;
        if ($#result_tab2 >= 0)
        {          
            $self->{acl_require_fault}=$result2;
            $ret_value=0;
        }
    }
    
    # select all metaservice_port linked to a metaservice_host entry
    # with a null ipaddr
    my $query=$query_ACL_config_check3;
    my $ref_list = $clusterDB->fetchallArrayref($query);
    my @list=@$ref_list;
    if ( $#list != -1)
    {              
        clmOutput($fct,"There Is metaservice_port.ipaddr not set in the database");
        clmOutput($fct,"Check your networkmap file or contact Bull Support");
        $ret_value=0;
    }              
                   
    # select all the metaservice_host with no metaservice_port associated
    $query=$query_ACL_config_check4;
    $ref_list = $clusterDB->fetchallArrayref($query);
    @list=@$ref_list;
                   
    if ( $#list != -1)
    {              
        clmOutput($fct,"metaservice_host table is not clean");
        $ret_value=0;
    }              
                   
    # select all metaservice_host
    $query=$query_ACL_config_check5;
    $ref_list = $clusterDB->fetchallArrayref($query);
    @list=@$ref_list;
                   
    if ( $#list == -1)
    {              
        clmOutput($fct,"metaservice_host table is empty, you have to register your metaservice first");
        $ret_value=0;
    }              
                   
    if ($ret_value)
    { $self->{acl_can_be_created}=1; }
                   
    return $ret_value;
    
}


sub check_metaservice()
{                  
    my $self=shift;
    return $self->{acl_can_be_created};
}                  


sub acl_table_exist()
{                  
    my $self = shift;
    my $fct= (caller(0))[3];
    return $self->{acl_is_created};
}                  


sub getServiceDnsNames()
{
    my $self = shift;
    my $clusterDB=$self->{"clusterdb"};
    my $query=$query_ServiceDnsNames;
    my @list=();
    my $ref_list=$clusterDB->fetchallArrayref($query);
    return $ref_list;
}




sub exist_ACL_view()
{
    my $self=shift;
    my $clusterDB=$self->{"clusterdb"};
    my $ref_count=$clusterDB->fetchallArrayref($query_count_acl_view);
    my $i=0;
    foreach my $ref_list (@$ref_count)
    {
	$i=@$ref_list[0];
    }
    return $i;
}

sub build_ACL_list()
{
    my $self=shift;
    my $fct = (caller(0))[3];
    my $clusterDB=$self->{"clusterdb"};
    my $Mquery=$query_metaservice_list;
    
    my $ACLquery=$query_acl_view_part0;
    my @targetList=("node","hwmanager","disk_array");
    
    my $index=0;
    my $result1=undef;
    my @result_tab1=undef;

    # First step: view creation
    foreach my $target_ref (@targetList)
    {
	if ($index++>0)
	{
	    $ACLquery.="\nUNION\n";
	}
	my $tempquery=$query_acl_view_part1;
	$tempquery=~s/__TARGET_TABLE__/$target_ref/g;
	$ACLquery.=$tempquery;
	
	my $ref_list_metaservice_id=$clusterDB->fetchallArrayref($Mquery);
    	foreach my $meta_ref (@$ref_list_metaservice_id)
    	{
	    my $metaservice_id=@$meta_ref[0];
	    my $tempquery2=$query_acl_view_part2;	
	    $tempquery2=~s/__TARGET_TABLE__/$target_ref/g;
	    $tempquery2=~s/__ID__/$metaservice_id/g;
	    $ACLquery.=$tempquery2;
	}
	
	my $tempquery3=$query_acl_view_part3;
	$tempquery3=~s/__TARGET_TABLE__/$target_ref/g;
	$ACLquery.=$tempquery3;
    }
    $ACLquery.=";\n";

    # We need to drop the view if the view exist
    if ($self->exist_ACL_view())
    {
	$clusterDB->fetchallArrayref($query_acl_drop_view);
    }
    $clusterDB->fetchallArrayref($ACLquery);
    if ($self->exist_ACL_view())
    {
	$self->{acl_is_created}=1;
	return 1;
    }
    else
    {
	clmOutput($fct,"The acl_table view could not be created, something is wrong with the clusterdb metaservice information");       
        exit 1;
    }
    return $self->{acl_is_created};
}


sub read_ACL_view()
{
	my ($self, $domain) = @_ ;
	my $fct = (caller(0))[3];
	my $clusterDB=$self->{"clusterdb"};
	my $Mquery=$query_metaservice_list;
	
	if ($self->{acl_is_created}==0)
	{
	    clmOutput($fct,"read_ACL_view called but there is no acl_table");
	    exit 1;
	}
	
	my $ref_list_metaservice_id=$clusterDB->fetchallArrayref($Mquery);
	
	# second step: view analysis
	my $index=0;
	
	my $ACL_view_query=$query_acl_view_read1;	
   	foreach my $meta_ref (@$ref_list_metaservice_id)
   	{
	    my $metaservice_id=@$meta_ref[0];
	    if ($index++>0)
	    {	
		$ACL_view_query.=" || ";
	    }
	    my $tempquery4=$query_acl_view_read2;
	    $tempquery4=~s/__ID__/$metaservice_id/g;
	    $ACL_view_query.=$tempquery4;
	}
	$ACL_view_query.=$query_acl_view_read3;
	foreach my $meta_ref (@$ref_list_metaservice_id)
	{
	    my $metaservice_id=@$meta_ref[0];
	    my $tempquery5=$query_acl_view_read4;
	    $tempquery5=~s/__ID__/$metaservice_id/g;
	    $ACL_view_query.=$tempquery5;
	}
	$ACL_view_query.=$query_acl_view_read5;
	
	$index=0;
   	foreach my $meta_ref (@$ref_list_metaservice_id)
   	{
	    my $metaservice_id=@$meta_ref[0];
	    if ($index++>0)
	    {	
		$ACL_view_query.=" , ";
	    }
	    my $tempquery6=$query_acl_view_read6;
	    $tempquery6=~s/__ID__/$metaservice_id/g;
	    $ACL_view_query.=$tempquery6;
	}
	$ACL_view_query.=" ;";
	#print "\n ACL_view_query:\n".$ACL_view_query."\n\n";
	
	$self->{"service_nb"}=$index;
	return $clusterDB->fetchallArrayref($ACL_view_query);
}


sub get_metaservice_idList()
{
    my $self=shift;
    my $clusterDB=$self->{"clusterdb"};
    my $Mquery=$query_metaservice_list;
    my $ref_list_metaservice_id=$clusterDB->fetchallArrayref($Mquery);
    return $ref_list_metaservice_id;
}


sub get_metaservice_global()
{
    my $self=shift;
    my $clusterDB=$self->{"clusterdb"};
    my $Mquery=$metaservices_aliases;
    my $ref_list_metaservice_id=$clusterDB->fetchallArrayref($Mquery);
    return $ref_list_metaservice_id;
}





sub get_service_struct()
{
    my $self=shift;
    my $str_ip="";
    my $ACLgroup="";
    my $acl_string="";
    my $group_index=-1;
    my $previous_ACL_config="";
#    my %hash_acl_to_ip=undef;
    my $ref_hash_acl_to_ip;
    my $ref_node_ACL;
    my @list=();
    my $Services_Dns_names=$self->getServiceDnsNames();
    my $ref_list_metaservice_id=$self->get_metaservice_idList();

    
    my $aclEqList=Bull::CLM::DB::EquipmentList::new();
    my $aclEq=Bull::CLM::DB::Equipment::new();
    my $interface=Bull::CLM::DB::NetworkInterface::new();
    
    my $current_ACL_token="TOKEN";
    my $current_group=0;
    
    if ($self->{acl_is_created}==0)
    {
	return $aclEqList;
    }

    if ($self->exist_ACL_view())
    {  $ref_node_ACL=$self->read_ACL_view(); } 
    else
    {   $ref_node_ACL=undef; } 

    foreach my $ref_concat_ip_Nservice (@$ref_node_ACL)
    {
	if ($current_ACL_token eq @$ref_concat_ip_Nservice[0])
	{   next; }
	
	if ($current_group==0) { $current_group++; }

	$aclEq=Bull::CLM::DB::Equipment::new();
	$interface=Bull::CLM::DB::NetworkInterface::new();

	$current_ACL_token=@$ref_concat_ip_Nservice[0];

# Boucle pour prendre en compte tous les services
	for (my $i=0;$i<$self->{"service_nb"};$i++)
	{
	    if (!((@$ref_concat_ip_Nservice)[2+$i] eq $fill_coalesce))
	    {
		my $ServiceIpAddr=$self->get_ipv_by_refid((@$ref_concat_ip_Nservice)[2+$i]);
		foreach my $ServiceDnsEntry_ref (@$Services_Dns_names)
		{
		    my ($MserviceDnsId,$Name) = (@$ServiceDnsEntry_ref);		    
		    if ( $MserviceDnsId eq (@$ref_list_metaservice_id)[$i][0])
		    {
			$interface=Bull::CLM::DB::NetworkInterface::new();
			$interface->set_name($Name);
			$interface->set_ip((@$ServiceIpAddr)[0][2]);
			$aclEq->add_interface($interface);
		    }
		}
	    }
	}

	$aclEq->set_name($current_ACL_token);
	$aclEq->set_type("ACL");
	
	$aclEqList->add($aclEq);
	$current_group++;
    }
    #print $aclEqList->to_string();
    return $aclEqList;  
}


sub get_acl_struct()
{
    my $self=shift;
    my $str_ip="";
    my $ACLgroup="";
    my $acl_string="";
    my $group_index=-1;
    my $previous_ACL_config="";
    my $ref_hash_acl_to_ip;

    my $aclEqList=Bull::CLM::DB::EquipmentList::new();
    my $aclEq=Bull::CLM::DB::Equipment::new();
    my $interface=Bull::CLM::DB::NetworkInterface::new();

    if ($self->{acl_is_created}==0)
    {
	return $aclEqList;
    }
    
    if ($self->check_metaservice()==0) { exit 3; }

    my $ref_list_metaservice_id=$self->get_metaservice_idList();    
    my $ref_node_ACL;

    if ($self->exist_ACL_view())
    { $ref_node_ACL=$self->read_ACL_view();  }
    else
    {  $ref_node_ACL=undef; }

    my $current_ACL_token="TOKEN";
    my $current_group=0;
    
    foreach my $ref_concat_ip_Nservice (@$ref_node_ACL)
    {
	if ($current_ACL_token eq @$ref_concat_ip_Nservice[0])
	{
	    $current_ACL_token=@$ref_concat_ip_Nservice[0];

	    # If the scalar @$ref_concat_ip_Nservice[1] is defined
	    # ip exist, we add it to the interface
	    if (defined(@$ref_concat_ip_Nservice[1]))
	    {
		$interface=Bull::CLM::DB::NetworkInterface::new();
		$interface->set_name($current_ACL_token);
		$interface->set_ip(@$ref_concat_ip_Nservice[1]);	       	    
		$aclEq->add_interface($interface);  
	    }
	    next;
	}
	
	if ($current_group==0) { $current_group++; }

	$aclEq=Bull::CLM::DB::Equipment::new();
	$interface=Bull::CLM::DB::NetworkInterface::new();

	$current_ACL_token=@$ref_concat_ip_Nservice[0];

	# If the scalar @$ref_concat_ip_Nservice[1] is defined
	# ip exist, we add it to the interface
	if (defined(@$ref_concat_ip_Nservice[1]))
	{ 
	    $interface->set_ip(@$ref_concat_ip_Nservice[1]);
	    $interface->set_name($current_ACL_token);
	    
	    $aclEq->set_name($current_ACL_token);
	    $aclEq->set_type("ACL");
	    
	    $aclEq->add_interface($interface);  
	    $aclEqList->add($aclEq);
	}
	$current_group++;
    }
#    print $aclEqList->to_string();
#    exit 0;
    return $aclEqList;   
}

sub get_equipment_list_alias_services_for_view($)
{                  
    my $self = shift;
    my $eql  = shift;
    my $fct  = (caller(0))[3];
                   
    my $ZoneInfo;  
                   
    my $domainName;
    my $zoneFile;  
    my $zoneFilePath;
    my $zoneName = "";
    my $zoneDomain = "";
    my $groupId;   
                   
    my $index=0;   
    my $view_string;
    my $alias_eql=Bull::CLM::DB::EquipmentList::new();
#    my $eql=$clusterDBEquipment->get_equipmentList();
                   
    $view_string=$self->get_service_struct();
                   
    for (my $i=0; $i<$view_string->number_of_equipment();$i++)
    {              
        my $aclEq=$view_string->get($i);
                   
        for (my $j=0;$j<$aclEq->number_of_interface();$j++)
        {          
            my $aclInt=$aclEq->get_interface($j);
            my $ip=$aclInt->get_ip();
            my $name=$aclInt->get_name();
            my $alias_eq=Bull::CLM::DB::NetworkInterface::new();
            $alias_eq->set_name($name);
            $alias_eq->set_ip($ip);
            $alias_eq->set("global_cname","no");
            $alias_eq->set("local_cname","no");
            $alias_eq->set("acl",$aclEq->get_name());
            $alias_eq->set("type",undef);
            for (my $k=0;$k<$eql->number_of_equipment();$k++)
            {      
                my $eq=$eql->get($k);
                for (my $l=0;$l<$eq->number_of_interface();$l++)
                {   
                    my $int=$eq->get_interface($l);
                    if ($ip eq $int->get_ip())
                    {   
                        $alias_eq->set("local_cname","yes");
                        $alias_eq->set("cname",$eq->get_name());
                    }
                }
            }
            $alias_eql->add($alias_eq);
        }
    }
    return $alias_eql;
}


sub get_equipment_list_alias_services($)
{   
    my $self = shift ;
    my $eql  = shift;
    my $view_service_str="";
    my $ref_list_metaservice_id=$self->get_metaservice_global();
    
    my $alias_eql=Bull::CLM::DB::EquipmentList::new();
    #my $eql=$clusterDBEquipment->get_equipmentList();

    foreach my $ref_list_node (@$ref_list_metaservice_id)
    {   
        if ( defined( (@$ref_list_node)[1]) )
        {   
            my $name=@$ref_list_node[0];
            my $ip=@$ref_list_node[1];
            my $type=@$ref_list_node[2];
            my $alias_eq=Bull::CLM::DB::NetworkInterface::new();
            
            $alias_eq->set_name($name);
            $alias_eq->set_ip($ip);
            $alias_eq->set("type",$type);
            $alias_eq->set("global_cname","no");
            for (my $i=0;$i<$eql->number_of_equipment();$i++)
            {   
                my $eq=$eql->get($i);
                for (my $j=0;$j<$eq->number_of_interface();$j++)
                {   
                    my $int=$eq->get_interface($j);
                    if ($ip eq $int->get_ip())
                    {   
                        $alias_eq->set("global_cname","yes");
                        $alias_eq->set("cname",$eq->get_name());
                    }
                }
            }
            $alias_eql->add($alias_eq);
        }
    }
    return $alias_eql;
}





1;
    




