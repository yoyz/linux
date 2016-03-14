from clusterdb_sqlalchemy import clusterdb_alchemy,Cluster,Network,Node,Node_port,Hwmanager,Hwmanager_port,Node_ftype,Ftype,Hw_group,Hw_group_member,Switch,Switch_port,Node_profile,Node_profile_device,Disk_array,Disk_array_port,Metaservice_port
from   xml_element        import memberXML
from   error_code         import error_code
from   equipment          import equipment_list,network_list,network,equipment,network_interface
import misc
import sys,getopt,re,os

################################################################################

class network_sql:

    def __init__(self,cdb):
        self.cdb=cdb


    def delete_range_network(self):
        cdb=self.cdb
        for net in cdb.session.query(Network).filter(Network.type=='range'):
#            if net.type=="range":
            cdb.session.delete(net)
        #cdb.session.commit()


    def delete_son_network(self):
        cdb=self.cdb
        for net in cdb.session.query(Network).filter(Network.network_id!=None):
#            if net.network_id!=None:
            cdb.session.delete(net)
        #cdb.session.commit()
        
    def clean_network_table(self):
        cdb=self.cdb
        for net in cdb.session.query(Network):
            net.subset=None
            net.subnet=None
            net.gateway=None
            net.network_id=None
            net.next_ip=None
            net.vpartition=None
            net.suffix=None
        #cdb.session.commit()


    def fetch_all_network(self):
        """
        Return a list a network from the db
        the list is composed of 'nw' object. 
        The list is of nw_list type.
        """
        cdb=self.cdb
        nwl=network_list()
        for net in cdb.session.query(Network):
            n=network()

            n.set_id(net.id)
            n.set_name(net.name)
            n.set_type(net.type)
            n.set_function(net.function)
            n.set_subset(net.subset)
            n.set_techno(net.techno)
            n.set_subnet(net.subnet)
            n.set_gateway(net.gateway)
            n.set_network_id(net.network_id)
            n.set_next_ip(net.next_ip)
            n.set_comment(net.comment)
            if net.network_id:
                for parentnet in cdb.session.query(Network):
                    if net.network_id==parentnet.id:
                        n.set_parent_name(parentnet.name)
                        n.set_network_id(parentnet.id)
            nwl.add_network(n)
        return nwl





    def update_subset_network(self,network_id,subset):
        """
        update the subset of a network entry
        it need a network_id and a subset
        """
        cdb=self.cdb
        ok=False
        #print "***update_range_network network_id : "+str(network_id)
        #print "***update_range_network subset     : "+subset

        for network in cdb.session.query(Network).filter(Network.id==network_id):
            if network.subset==None:
                network.subset=subset
            else:
                network.subset=network.subset+" "+subset
            ok=True
        return ok


    def remove_subset_network(self,network_id,subset):
        """
        update the subset of a network entry
        it need a network_id and a subset
        """
        cdb=self.cdb
        ok=False
        #print "***update_range_network network_id : "+str(network_id)
        #print "***update_range_network subset     : "+subset

        for network in cdb.session.query(Network).filter(Network.id==network_id):
            #print "network subset"
            #print network.subset
            if network.subset!=None:
                str_subset=network.subset
                new_list_subset=[]
                list_subset=str_subset.split(" ")
                #print list_subset
                #print str_subset
                for sub in list_subset:
                    if (sub != subset and 
                        sub != ''):
                        new_list_subset.append(sub)
                    else:
                        ok=True
                str_subset=""
                for sub in new_list_subset:
                    str_subset=str_subset+" "+sub
                network.subset=str_subset
                #print network.subset
                #print ok
        return ok



    def update_vpartition_network(self,network_id,vpartition):
        """
        update the subset of a network entry
        it need a network_id and a subset
        """
        cdb=self.cdb
        ok=False
        #print "***update_vpartition_network network_id : "+str(network_id)
        #print "***update_vpartition_network vpartition : "+str(vpartition)

        for network in cdb.session.query(Network).filter(Network.id==network_id):
#            if network.id==network_id:
            network.vpartition=vpartition
            ok=True
                #cdb.session.commit()
        return ok

    def update_suffix_network(self,network_id,suffix):
        """
        update the subset of a network entry
        it need a network_id and a subset
        """
        cdb=self.cdb
        ok=False
        #print "***update_vpartition_network network_id : "+str(network_id)
        #print "***update_vpartition_network vpartition : "+str(vpartition)

        for network in cdb.session.query(Network).filter(Network.id==network_id):
#            if network.id==network_id:
            network.suffix=suffix
            ok=True
                #cdb.session.commit()
        return ok


    def get_network_id_from_subnet(self,subnet):
        cdb=self.cdb
        id=-1
        for network in cdb.session.query(Network).filter(Network.subnet==subnet):
            id=network.id
        return id




    def add_network_entry(self,nw,logical_nw):
        cdb=self.cdb
        N=Network()

        N.id=None
        N.name=logical_nw.get_name()
        N.type=logical_nw.get_type()

        N.techno=nw.get_techno()
        N.network_id=nw.get_id()

        N.function=logical_nw.get_function()
        N.subnet=logical_nw.get_subnet()

        N.network_id=nw.get_id()

        cdb.session.add(N)
        #cdb.session.commit()





    def add_logical_network(self,parent_nw,logical_nw):
        """
        Add a logical network in the 'Network' table
        parent_nw and logical_nw are 'nw' object
        The 'Network' class is used to store the object in the database
        """
        ok=True
        cdb=self.cdb
        N=Network()


        N.id=None
        N.network_id=parent_nw.get_id()
        N.techno=parent_nw.get_techno()

        N.name=logical_nw.get_name()
        N.type=logical_nw.get_type()
        N.function=logical_nw.get_function()
        N.subnet=logical_nw.get_subnet()
        N.gateway=logical_nw.get_gateway()
        N.vpartition=logical_nw.get_vpartition()
        N.suffix=logical_nw.get_suffix()


#        N.subfunction=logical_nw.get_subset()
#        N.comment=logical_nw.get_comment()

        cdb.session.add(N)
        #cdb.session.commit()
        return ok


    def delete_logical_network(self,logical_nw):
        """
        Add a logical network in the 'Network' table
        parent_nw and logical_nw are 'nw' object
        The 'Network' class is used to store the object in the database
        """
        ok=False
        cdb=self.cdb

        for network in cdb.session.query(Network).filter(Network.name==logical_nw.get_name()):
            if network.network_id!=None:
                # Deal with logical network son
                cdb.session.delete(network)
                ok=True
            else:
                # Deal with logical network parent
                network.subnet=None
                network.gateway=None
                network.vpartition=None
                network.suffix=None
                ok=True
        return ok






    def add_range_network(self,parent_id,new_name,subset,first_ip):
        cdb=self.cdb
        N=Network()        

        N.id=None
        N.name=new_name
        N.type="range"
        N.network_id=parent_id
        N.subset=subset
        N.next_ip=first_ip
        try:
            cdb.session.add(N)
            return True
        except:
            return False
        #cdb.session.commit()

        
    def update_subnet_network(self,network_id,subnet):
        """
        Set a subnet in a network entry
        """
        cdb=self.cdb
        ok=False
        for network in cdb.session.query(Network).filter(Network.id==network_id):
#            if network.id==network_id:
            network.subnet=subnet
            ok=True
            #cdb.session.commit()
        return ok


    def update_gateway_network(self,network_id,gateway):
        """
        Set a subnet in a network entry
        """
        cdb=self.cdb
        ok=False
        for network in cdb.session.query(Network).filter(Network.id==network_id):
#            if network.id==network_id:
            network.gateway=gateway
            ok=True
                #cdb.session.commit()
        return ok


    def update_next_ip_network(self,network_id,next_ip):
        """
        Set a subnet in a network entry
        """
        cdb=self.cdb
        ok=False
        for network in cdb.session.query(Network).filter(Network.id==network_id):
#            if network.id==network_id:
            network.next_ip=next_ip
            ok=True
                #cdb.session.commit()
        return ok





    def clean_secondary_network(self):
        cdb=self.cdb
        for n in cdb.session.query(Network).filter(Network.type=='secondary'):
#            if n.type=='secondary':
            cdb.session.delete(n)
        #cdb.session.commit()


    def clean_range(self):
        cdb=self.cdb
        for n in cdb.session.query(Network).filter(Network.type=='range'):
#            if n.type=='range':
            cdb.session.delete(n)
        #cdb.session.commit()


    def clean_son_network(self):
        cdb=self.cdb
        for n in cdb.session.query(Network).filter(Network.network_id!=None):
#            if n.network_id!=None:
            cdb.session.delete(n)
        #cdb.session.commit()


    def clean_primary_network(self):
        cdb=self.cdb
        for n in cdb.session.query(Network).filter(Network.subnet!=None):
#            if n.subnet!=None:
            n.subnet=None
            n.gateway=None
            n.vpartition=None
        #cdb.session.commit()







################################################################################


class interface_member_sql:

    def __init__(self,cdb):
        self.cdb=cdb
        self.network_name=None
        self.equipment_name=None
        self.member=member


    def set_network_name(self,network_name):
        self.network_name=network_name

    
    def set_equipment_name(self,equipment_name):
        self.equipment_name=equipment_name

    def set_member(self,member):
        self.member=member


    def get_ip_from_equipment():
        cdb=self.cdb
        
################################################################################

class ftype_sql:
    def __init__(self,cdb):
        self.cdb=cdb

    def fetch_all_ftype(self):
        cdb=self.cdb
        ftl=[]
        for ft in cdb.session.query(Ftype):
            ftl.append(ft.label)
        return ftl

    def exist(self,ftype):
        ok=False
        ftl=self.fetch_all_ftype()
        if ftype in ftl:
            ok=True
        else:
            ok=False
        return ok



################################################################################

class node_profile_device_sql:

    def __init__(self,cdb):
        self.interface_name_list=[]
        self.virtual_interface_name_list=[]
        self.cdb=cdb

    def fetch_all_interface_name(self):
        """
        Return a list of all known interface name from the node_profile_device
        """
        cdb=self.cdb
        for npd in cdb.session.query(Node_profile_device):
            if npd.iname not in self.interface_name_list:
                self.interface_name_list.append(npd.iname)

        return self.interface_name_list



################################################################################

class node_profile_sql:

    def __init__(self,cdb):
        self.cdb=cdb

    def fetch_profile_from_id(self,id):
        """
        Return a profile name from a node_profile_id
        """
        cdb=self.cdb
        profile=None
        for np in cdb.session.query(Node_profile).filter(Node_profile.id==id):
            profile=np.name
        return profile




################################################################################

class member_sql:

    def __init__(self,cdb):
        self.member=None
        self.cdb=cdb
        self.list_node=[]
        self.list_switch=[]
        self.list_hwmanager=[]
        self.list_diskarray=[]
        self.list_metaservice=[]
        self.fetched_element=0

    def get_list_node(self):
        return self.list_node

    def get_list_hwmanager(self):
        return self.list_hwmanager

    def get_list_switch(self):
        return self.list_switch

    def get_list_diskarray(self):
        return self.list_diskarray

    def get_list_metaservice(self):
        return self.list_metaservice

    def set_list_node(self,list_node):
        self.list_node=list_node

    def set_list_hwmanager(self,list_hwmanager):
        self.list_hwmanager=list_hwmanager

    def set_list_switch(self,list_switch):
        self.list_switch=list_switch

    def set_list_diskarray(self,list_diskarray):
        self.list_diskarray=list_diskarray

    def set_list_metaservice(self,list_metaservice):
        self.list_metaservice=list_metaservice

    
    def set_member(self,member):
        self.member=member


    def get_number_of_members(self):
        self.fetched_element=len(self.list_node)+len(self.list_hwmanager)+len(self.list_switch)+len(self.list_diskarray)+len(self.list_metaservice)
        return self.fetched_element

    
    def get_equipment_name_list(self):
        if (self.member.obj=="*"):
            self.list_node=self.get_node_name_from_member()
            self.list_switch=self.get_switch_name_from_member()
            self.list_hwmanager=self.get_hwmanager_name_from_member()
            self.list_diskarray=self.get_disk_array_name_from_member()
            self.list_metaservice=self.get_metaservice_name_from_member()


        if (self.member.obj=="node"):
            self.list_node=self.get_node_name_from_member()

        if (self.member.obj=="switch"):
            self.list_switch=self.get_switch_name_from_member()

        if (self.member.obj=="hwmanager"):
            self.list_hwmanager=self.get_hwmanager_name_from_member()

        if (self.member.obj=="disk_array"):
            self.list_diskarray=self.get_disk_array_name_from_member()

        if (self.member.obj=="metaservice"):
            self.list_metaservice=self.get_metaservice_name_from_member()

        return self.get_number_of_members()



    def _get_metaservice_port_name_list(self):
        cdb=self.cdb
        list=[]
        for metaservice_port in cdb.session.query(Metaservice_port):
            list.append(metaservice_port.name)
        return list



    def _get_metaservice_port_name_list_from_metaservice_port_type(self):
        cdb=self.cdb
        list=[]
        for metaservice_port in cdb.session.query(Metaservice_port).filter(Metaservice_port.type==self.member.type):
            list.append(metaservice_port.name)
        return list



    def get_metaservice_name_from_member(self):
        ltype=[]
        lgroup=[]
        linter=[]
        lret=[]

  #Only on metaservice_port 06/04/2010
  #TO BE CORRECTED
        if (self.member.type=='*' and self.member.group=='*'):
            lret=self._get_metaservice_port_name_list()
            return lret

        if (self.member.type!='*'):
            ltype=self._get_metaservice_port_name_list_from_metaservice_port_type()
            lret=ltype



#        if (self.member.type=='*' and self.member.group=='*'):
#            lret=self._get_metaservice_name_list()
#            return lret

#        if (self.member.type!='*'):
#            ltype=self._get_metaservice_name_list_from_metaservice_type()
#            lret=ltype


  # intersection from hw_group and ftype
#        if (self.member.group!='*' and self.member.type!='*'):
#            linter=misc.intersect_list(ltype,lgroup)
#            lret=linter
        return lret



    def get_hwmanager_name_from_member(self):
        ltype=[]
        lgroup=[]
        linter=[]
        lret=[]
        
        # fetch all the switch
        if (self.member.type=='*' and self.member.group=='*'):
            lret=self._get_hwmanager_name_list()

        if (self.member.type!='*'):
            r=re.compile("([a-zA-Z\*]+)\[([a-zA-Z\-_1-9]+)\]") # match string 'bmc[OSS]'
            if (r.match(self.member.type)):                 
                match=r.match(self.member.type)
                ms=member_sql(self.cdb)
                ns=node_sql(self.cdb)
                m=memberXML()
                m.set_object("node")
                m.set_type(match.group(2))
                ms.set_member(m)
                ms.get_equipment_name_list()
                lnode=ms.list_node
                lhwm1=[]
                #print lnode
                for node in lnode:
                    str=""
                    str=ns.get_hwmanager_from_node_name(node)
                    if len(str)>0:
                        lhwm1.append(str)

                m=memberXML()
                m.set_object("hwmanager")
                m.set_type(match.group(1))
                m.set_group(self.member.group)
                ms.set_member(m)
                ms.get_equipment_name_list()
                lhwm2=ms.list_hwmanager
                linter=misc.intersect_list(lhwm1,lhwm2)
                #print lhwm1
                #print lhwm2
                #print linter
                lret=linter               
                return lret
            else:
                ltype=self._get_hwmanager_name_list_from_hwmanager_type()
            lret=ltype

        if (self.member.group!='*'):
            lgroup=self._get_hwmanager_name_list_from_hwmanager_hwgroup()
            lret=lgroup

        # intersection from hw_group and ftype
        if (self.member.group!='*' and self.member.type!='*'):
            linter=misc.intersect_list(ltype,lgroup)
            lret=linter
        return lret



    def get_disk_array_name_from_member(self):
        ltype=[]
        lgroup=[]
        linter=[]
        lret=[]
        
        # fetch all the disk_array
        if (self.member.type=='*' and self.member.group=='*'):
            lret=self._get_disk_array_name_list()
            return lret

        if (self.member.type!='*'):
            ltype=self._get_disk_array_name_list_from_disk_array_type()
            lret=ltype

        if (self.member.group!='*'):
            lgroup=self._get_disk_array_name_list_from_disk_array_hwgroup()
            lret=lgroup

        
        # intersection from hw_group and ftype
        if (self.member.group!='*' and self.member.type!='*'):
            linter=misc.intersect_list(ltype,lgroup)
            lret=linter

        return lret




    def get_switch_name_from_member(self):
        ltype=[]
        lgroup=[]
        linter=[]
        lret=[]
        
        # fetch all the switch
        if (self.member.type=='*' and self.member.group=='*'):
            lret=self._get_switch_name_list()

        if (self.member.type!='*'):
            ltype=self._get_switch_name_list_from_switch_type()
            lret=ltype


        if (self.member.group!='*'):
            lgroup=self._get_switch_name_list_from_switch_hwgroup()
            lret=lgroup

        
        # intersection from hw_group and ftype
        if (self.member.group!='*' and self.member.type!='*'):
            linter=misc.intersect_list(ltype,lgroup)
            lret=linter



        return lret



    def get_node_name_from_member(self):
        lftype=[]
        lgroup=[]
        linter=[]
        lret=[]

        # fetch all the node
        if (self.member.type=='*' and self.member.group=='*'):
            lret=self._get_node_name_list()

        # fetch all node with the defined ftype
        if (self.member.type!='*'):
            lftype=self._get_node_name_list_from_node_ftype()
            lret=lftype
            #print lret

        # fetch all node from the defined hw_group
        if (self.member.group!='*'):
#            print self.member.group
#            sys.exit(0)
            lgroup=self._get_node_name_list_from_node_hwgroup()
            lret=lgroup
            #print lret

        # intersection from hw_group and ftype
        if (self.member.group!='*' and self.member.type!='*'):
            linter=misc.intersect_list(lftype,lgroup)
            lret=linter
            
        return lret


    def _get_hwmanager_name_list(self):
        cdb=self.cdb
        list=[]
        for hwmanager in cdb.session.query(Hwmanager):
            list.append(hwmanager.name)
        return list


    def _get_disk_array_name_list(self):
        cdb=self.cdb
        list=[]
        for disk_array in cdb.session.query(Disk_array):
            list.append(disk_array.name)
        return list



    def _get_hwmanager_name_list_from_hwmanager_type(self):
        cdb=self.cdb
        list=[]
        for hwmanager in cdb.session.query(Hwmanager).filter(Hwmanager.type==self.member.type):
            list.append(hwmanager.name)
        return list



    def _get_disk_array_name_list_from_disk_array_type(self):
        cdb=self.cdb
        list=[]
        for disk_array,disk_array_port in cdb.session.query(Disk_array ,Disk_array_port).filter(Disk_array.id==Disk_array_port.disk_array_id).filter(Disk_array_port.type==self.member.type):
            if (not (disk_array.name in list)):
                list.append(disk_array.name)
        return list




    def _get_switch_name_list(self):
        cdb=self.cdb
        list=[] 
        for switch in cdb.session.query(Switch.name):
            list.append(switch.name)
        return list

    def _get_switch_name_list_from_switch_type(self):
        cdb=self.cdb
        list=[]
        for switch in cdb.session.query(Switch).filter(Switch.type==self.member.type):
            list.append(switch.name)
        return list


    def _get_switch_name_list_from_switch_hwgroup(self):
        cdb=self.cdb
        list=[]
        for switch,hw_group,hw_group_member in cdb.session.query(Switch,Hw_group,Hw_group_member).filter(Hw_group.name==self.member.group).filter(Hw_group_member.target_table=='switch').filter(Hw_group.id==Hw_group_member.hw_group_id).filter(Hw_group_member.ref_id==Switch.id).order_by(Switch.id):
            list.append(switch.name)
        return list




    def _get_node_name_list(self):
        cdb=self.cdb
        list=[]
        #print str(cdb.session.query(Node.name))        
        for node in cdb.session.query(Node.name):
            list.append(node.name)
        return list


    def _get_hwmanager_name_list_from_hwmanager_hwgroup(self):
        cdb=self.cdb
        list=[]
        for hwmanager,hw_group,hw_group_member in cdb.session.query(Hwmanager,Hw_group,Hw_group_member).filter(Hw_group.name==self.member.group).filter(Hw_group_member.target_table=='hwmanager').filter(Hw_group.id==Hw_group_member.hw_group_id).filter(Hw_group_member.ref_id==Hwmanager.id).order_by(Hwmanager.id):
            list.append(hwmanager.name)
        return list


    def _get_disk_array_name_list_from_disk_array_hwgroup(self):
        cdb=self.cdb
        list=[]
        for disk_array,hw_group,hw_group_member in cdb.session.query(Disk_array,Hw_group,Hw_group_member).filter(Hw_group.name==self.member.group).filter(Hw_group_member.target_table=='disk_array').filter(Hw_group.id==Hw_group_member.hw_group_id).filter(Hw_group_member.ref_id==Disk_array.id).order_by(Disk_array.id):
            list.append(disk_array.name)
        return list



    def _get_node_name_list_from_node_hwgroup(self):
        cdb=self.cdb
        list=[]
        for node,hw_group,hw_group_member in cdb.session.query(Node,Hw_group,Hw_group_member).filter(Hw_group.name==self.member.group).filter(Hw_group_member.target_table=='node').filter(Hw_group.id==Hw_group_member.hw_group_id).filter(Hw_group_member.ref_id==Node.id).order_by(Node.id):
            list.append(node.name)
        return list

    def _get_node_name_list_from_node_ftype(self):
        cdb=self.cdb
        list=[]
        for node,node_ftype,ftype in cdb.session.query(Node,Node_ftype,Ftype).filter(Node.id==Node_ftype.node_id).filter(Node_ftype.ftype_id==Ftype.id).filter(Ftype.label==self.member.type).order_by(Node.id):
            list.append(node.name)
        return list

################################################################################

class hw_group_sql:

    def __init__(self,cdb):
        self.cdb=cdb
        self.lhw_group=[]


    def fetch_all_hw_group(self):
        cdb=self.cdb
        for hw_group in cdb.session.query(Hw_group):
            self.lhw_group.append(hw_group.name)
        return self.lhw_group

    def hw_group_exist(self,hw_group_name):
        cdb=self.cdb
        found=False
        for hw_group in cdb.session.query(Hw_group):
            if hw_group.name==hw_group_name:
                found=True
        return found



################################################################################


class node_sql:

    def __init__(self,cdb):
        self.cdb=cdb


    def fetch_from_name(self,node_name):
        """
        Public Method
        """
        cdb=self.cdb
        e=equipment()
        nps=node_profile_sql(cdb)
        #print str(cdb.session.query(Node.name,Node.id,Node.profile_id).filter(Node.name==node_name))
        #print ""
        #return True
        #for n in cdb.session.query(Node).filter(Node.name==node_name):
        for n in cdb.session.query(Node.name,Node.id,Node.profile_id).filter(Node.name==node_name):

            e.set_table_name("node")
            e.set_name(n.name)
            e.set_id(n.id)
            e.set_profile_id(n.profile_id)
            e.set_profile(nps.fetch_profile_from_id(n.profile_id))
        return e

    def exist_node_name(self,node_name):
        """
        Public Method
        """
        cdb=self.cdb
        ok=False
        e=equipment()
        for n in cdb.session.query(Node).filter(Node.name==node_name):
            ok=True
        return ok


    def get_hwmanager_from_node_name(self,node_name):
        """
        Public Method
        """
        cdb=self.cdb
        hwmanager_name=""
        for n,h in cdb.session.query(Node,Hwmanager).filter(Node.name==node_name).filter(Node.hwmanager_id==Hwmanager.id):
            hwmanager_name=h.name
        return hwmanager_name




################################################################################




class hwmanager_sql:

    def __init__(self,cdb):
        self.cdb=cdb
        self.lhwmanager_type=[]
        self.lhwmanager_name=[]


    def exist_hwmanager_name(self,hwmanager_name):
        """
        Public Method
        """
        cdb=self.cdb
        ok=False
        for h in cdb.session.query(Hwmanager).filter(Hwmanager.name==hwmanager_name):
            ok=True
        return ok



    def fetch_from_name(self,hwmanager_name):
        """
        Public Method
        """
        cdb=self.cdb
        e=equipment()
        for h in cdb.session.query(Hwmanager).filter(Hwmanager.name==hwmanager_name):
            e.set_table_name("hwmanager")
            e.set_name(h.name)
        return e


    def fetch_all_hwmanager_type(self):
        cdb=self.cdb
        for hwm in cdb.session.query(Hwmanager):
            if hwm.type not in self.lhwmanager_type:
                self.lhwmanager_type.append(hwm.type)
        return self.lhwmanager_type


    def fetch_all_hwmanager_name(self):
        cdb=self.cdb
        for hwm in cdb.session.query(Hwmanager):
            if hwm.name not in self.lhwmanager_name:
                self.lhwmanager_name.append(hwm.name)
        return self.lhwmanager_name


    def type_exist(self,type):
        ok=False
        if len(self.lhwmanager_type)==0:
            hwmt=self.fetch_all_hwmanager_type()
        else:
            hwmt=self.lhwmanager_type

        if type in hwmt:
            ok=True
        else:
            ok=False
        return ok


    def name_exist(self,name):
        ok=False
        if len(self.lhwmanager_name)==0:
            hwmn=self.fetch_all_hwmanager_name()
        else:
            hwmn=self.lhwmanager_name

        if name in hwmn:
            ok=True
        else:
            ok=False
        return ok



################################################################################

class switch_sql:

    def __init__(self,cdb):
        self.cdb=cdb
        self.lswitch_type=[]
        self.lswitch_name=[]


    def exist_switch_name(self,switch_name):
        """
        Public Method
        """
        cdb=self.cdb
        ok=False
        for h in cdb.session.query(Switch).filter(Switch.name==switch_name):
            ok=True
        return ok

    def fetch_from_name(self,switch_name):
        """
        Public Method
        """
        cdb=self.cdb
        e=equipment()
        for s in cdb.session.query(Switch).filter(Switch.name==switch_name):
            e.set_table_name("switch")
            e.set_name(s.name)
        return e




    def fetch_all_switch_type(self):
        cdb=self.cdb
        for sw in cdb.session.query(Switch):
            if sw.type not in self.lswitch_type:
                self.lswitch_type.append(sw.type)
        return self.lswitch_type


    def fetch_all_switch_name(self):
        cdb=self.cdb
        for sw in cdb.session.query(Switch):
            if sw.name not in self.sw_name:
                self.lswitch_name.append(sw.name)
        return self.lswitch_name


    def type_exist(self,type):
        ok=False
        if len(self.lswitch_type)==0:
            lswt=self.fetch_all_switch_type()
        else:
            lswt=self.lswitch_type

        if type in lswt:
            ok=True
        else:
            ok=False
        return ok


    def name_exist(self,name):
        ok=False
        if len(self.lswitch_name)==0:
            lswn=self.fetch_all_hwmanager_name()
        else:
            lswn=self.lswitch_name

        if name in lswn:
            ok=True
        else:
            ok=False
        return ok




################################################################################

class disk_array_sql:

    def __init__(self,cdb):
        self.cdb=cdb
        self.lda_type=[]
        self.lda_name=[]


    def exist_disk_array_name(self,da_name):
        """
        Public Method
        """
        cdb=self.cdb
        ok=False
        for h in cdb.session.query(Disk_array).filter(Disk_array.name==da_name):
            ok=True
        return ok


    def fetch_from_name(self,da_name):
        """
        Public Method
        """
        cdb=self.cdb
        e=equipment()
        for d in cdb.session.query(Disk_array).filter(Disk_array.name==da_name):
            e.set_table_name("disk_array")
            e.set_name(d.name)
        return e



    def fetch_all_disk_array_type(self):
        cdb=self.cdb
        for da in cdb.session.query(Disk_array):
            if da.type not in self.lda_type:
                self.lda_type.append(da.type)
        return self.lda_type


    def fetch_all_disk_array_name(self):
        cdb=self.cdb
        for da in cdb.session.query(Disk_array):
            if da.name not in self.lda_name:
                self.lda.append(da.name)
        return self.lda_name


    def type_exist(self,type):
        ok=False
        if len(self.lda_type)==0:
            lda_type=self.fetch_all_disk_array_type()
        else:
            lda_type=self.lda_type

#        print lda_type
        if type in lda_type:
            ok=True
        else:
            ok=False
        return ok


    def name_exist(self,name):
        ok=False
        if len(self.lda_name)==0:
            lda_name=self.fetch_all_disk_array_name()
        else:
            lda_name=self.lda_name

        if name in lda_name:
            ok=True
        else:
            ok=False
        return ok



################################################################################


class node_port_sql:

    def __init__(self,cdb):
        self.cdb=cdb



#>     def fetch_from_profile_hba_port(self,profile_name,hba_rank,port_rank):
#>         cdb=self.cdb
#>         iname=None
#>         i=0
#>         for node_profile,node_profile_device in cdb.session.query(Node_profile,Node_profile_device).filter(Node_profile.name==profile_name).filter(Node_profile_device.node_profile_id==Node_profile.id).filter(No
#"de_profile_device.hba_rank==hba_rank).filter(Node_profile_device.port_rank==port_rank):
#>             i=i+1
#>             #print node_profile_device.interface_name
#>             iname=node_profile_device.interface_name
#>         if i>1:
#>             print "fetch_from_profile_hba_port contain more than one hba_rank port_rank for profile : "+profile_name
#>             sys.exit(1)   
#>         return iname
#>

    def delete_logical_node_port(self):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for node_port in cdb.session.query(Node_port).filter(Node_port.type=='logical'):            
            cdb.session.delete(node_port)
            ok=True
        return ok


    def change_node_port_network_id(self,from_network_id,to_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for node_port in cdb.session.query(Node_port).filter(Node_port.network_id==from_network_id):            
            node_port.network_id=to_network_id
        return ok


    def clean_ipaddr(self,from_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for node_port in cdb.session.query(Node_port).filter(Node_port.network_id==from_network_id):            
            node_port.ipaddr=None
        return ok


    def exist_interface_from_node_profile(self,equipment,iname):
        """
        return True if the interface name exist with this equipment
        """
        cdb=self.cdb
        ok=False
        for node_profile,node_profile_device in cdb.session.query(Node_profile,Node_profile_device).filter(Node_profile.name==equipment.get_profile()).filter(Node_profile_device.node_profile_id==Node_profile.id).filter(Node_profile_device.interface_name==iname):
            ok=True
        return ok


    def exist_hba_port_from_node_profile(self,equipment,iname,hba_rank,port_rank):
        """
        return True if the interface name exist and match the hba_rank port_rank in node_port
        """
        cdb=self.cdb
        ok=False
        for node_profile,node_profile_device in cdb.session.query(Node_profile,Node_profile_device).filter(Node_profile.name==equipment.get_profile()).filter(Node_profile_device.node_profile_id==Node_profile.id).filter(Node_profile_device.interface_name==iname).filter(Node_profile_device.hba_rank==ba_rank).filter(Node_profile_device.port_rank==port_rank):
            ok=True
        return ok




    def get_iname_from_node_profile(self,equipment,hba_rank,port_rank,type):
        """
        return True if the interface name exist and match the hba_rank port_rank in node_port
        """
        cdb=self.cdb
        iname=None
#        print "get iname"
        for node_profile,node_profile_device in cdb.session.query(Node_profile,Node_profile_device).filter(Node_profile_device.type==type).filter(Node_profile.name==equipment.get_profile()).filter(Node_profile_device.node_profile_id==Node_profile.id).filter(Node_profile_device.hba_rank==hba_rank).filter(Node_profile_device.port_rank==port_rank):
            iname=node_profile_device.interface_name
        return iname



    


    def fetch_from_name_network_name_iname(self,equipment,network_name,iname):
        """
        Public Method
        """
        cdb=self.cdb

####        for node,node_port,node_profile_device,network in cdb.session.query(Node,Node_port,Node_profile_device,Network).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id).order_by(Node_port.hba_rank).order_by(Node_port.port_rank):
        #print str(cdb.session.query(Node,Node_port,Node_profile_device,Network).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id).order_by(Node_port.hba_rank).order_by(Node_port.port_rank))
        #return True
#        for node,node_port,node_profile_device,network in cdb.session.query(Node,Node_port,Node_profile_device,Network).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id).order_by(Node_port.hba_rank).order_by(Node_port.port_rank):
#        print str(cdb.session.query(Node.name,Node.id,Network.name,Network.id,Node_port,Node_profile_device,Network).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id))

#        print str(cdb.session.query(Node.name,                                                                            Node.id,
#                                                                            Node_profile_device.id,
#                                                                            Node_profile_device.interface_name,
#                                                                            Network.name,
#                                                                            Network.id,
#                                                                            Node_port).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id))
#        print ""
#

        for node,node_port,node_profile_device,network in cdb.session.query(Node,Node_port,Node_profile_device,Network).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name).filter(Node_port.node_profile_device_id==Node_profile_device.id).order_by(Node_port.hba_rank).order_by(Node_port.port_rank):

            ni=network_interface()
            
            ni.set_network_name(network.name)
            ni.set_network_id(node_port.network_id)
            
            ni.set_id(node_port.id)
            ni.set_ipaddr(node_port.ipaddr)
            ni.set_hwaddr(node_port.hwaddr)
            ni.set_hba_rank(node_port.hba_rank)
            ni.set_port_rank(node_port.port_rank)
            ni.set_switch_id(node_port.switch_id)
            ni.set_switch_slot(node_port.switch_slot)
            ni.set_switch_port(node_port.switch_port)
            ni.set_type(node_port.type)
            ni.set_function(node_port.function)
            ni.set_node_profile_device_id(node_port.node_profile_device_id)

            ni.set_interface_name(node_profile_device.interface_name)
            equipment.add_network_interface(ni)
            

            

    def store_network_interface(self,equipment):
        cdb=self.cdb
        for node,node_port in cdb.session.query(Node,Node_port).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id):
            for netint in equipment.get_all_network_interface():

                if (netint.id==node_port.id):
                    node_port.ipaddr=netint.ipaddr
                    node_port.network_id=netint.network_id

                    node_port.switch_id=netint.switch_id
                    node_port.switch_slot=netint.switch_slot
                    node_port.switch_port=netint.switch_port


        for netint in equipment.get_all_network_interface():
            if (netint.id==None     and 
                netint.forged==True):
                already_created=False
                for node,node_port in cdb.session.query(Node,Node_port).filter(Node_port.type=="logical").filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==netint.network_id):
                    already_created=True

                if already_created==False:
                    self.add_logical_network_interface(equipment,netint)

        cdb.session.commit()


    def add_logical_network_interface(self,equipment,netint):
        cdb=self.cdb
        N=Node_port()
        N.id=None
        N.node_id=equipment.get_id()
        N.type=netint.get_type()
#        N.hwaddr=netint.hwaddr
        N.hba_rank=netint.get_hba_rank()
        N.port_rank=netint.get_port_rank()
        N.switch_id=netint.get_switch_id()
        N.switch_slot=netint.get_switch_slot()
        N.switch_port=netint.get_switch_port()

        N.node_profile_device_id=netint.get_node_profile_device_id()
        N.function=netint.get_function()

        N.network_id=netint.get_network_id()
        cdb.session.add(N)


################################################################################


class hwmanager_port_sql:

    def __init__(self,cdb):
        self.cdb=cdb


    def change_hwmanager_port_network_id(self,from_network_id,to_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for hwmanager_port in cdb.session.query(Hwmanager_port).filter(Hwmanager_port.network_id==from_network_id):            
            hwmanager_port.network_id=to_network_id
        return ok


    def clean_ipaddr(self,from_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for hwmanager_port in cdb.session.query(Hwmanager_port).filter(Hwmanager_port.network_id==from_network_id):            
            hwmanager_port.ipaddr=None
        return ok



    def fetch_from_name_network_name(self,equipment,network_name):
        """
        Public Method
        """
        cdb=self.cdb
#        print hba_rank
#        print port_rank

#        for npd in cdb.session.query(Node.name,Node_port.id,Node_port.ipaddr,Node_port.hwaddr,Node_port.hba_rank,Node_port.port_rank,Network.subnet).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name):
        for hwmanager,hwmanager_port,network in cdb.session.query(Hwmanager,Hwmanager_port,Network).filter(Network.name==network_name).filter(Hwmanager.name==equipment.get_name()).filter(Hwmanager_port.hwmanager_id==Hwmanager.id).filter(Hwmanager_port.network_id==Network.id):
            ni=network_interface()

            ni.network_name=network.name
            ni.network_id=network.id

            ni.network_id=hwmanager_port.network_id
            ni.id=hwmanager_port.id
            ni.ipaddr=hwmanager_port.ipaddr
            ni.hwaddr=hwmanager_port.hwaddr

            ni.switch_id=hwmanager_port.switch_id
            ni.switch_slot=hwmanager_port.switch_slot
            ni.switch_port=hwmanager_port.switch_port


            equipment.add_network_interface(ni)


    def store_network_interface(self,equipment):
        cdb=self.cdb
        for hwmanager,hwmanager_port in cdb.session.query(Hwmanager,Hwmanager_port).filter(Hwmanager.name==equipment.get_name()).filter(Hwmanager_port.hwmanager_id==Hwmanager.id):
            for netint in equipment.get_all_network_interface():
                if (netint.id!=None and 
                    netint.forged==True):
                    print "TODO"

                if (netint.id==hwmanager_port.id):
                    hwmanager_port.ipaddr=netint.ipaddr
                    hwmanager_port.network_id=netint.network_id

                    hwmanager_port.switch_id=netint.switch_id
                    hwmanager_port.switch_slot=netint.switch_slot
                    hwmanager_port.switch_port=netint.switch_port


################################################################################


class switch_port_sql:

    def __init__(self,cdb):
        self.cdb=cdb

    def delete_logical_switch_port(self):
        cdb=self.cdb
        ok=False
        for switch_port in cdb.session.query(Switch_port).filter(Switch_port.type=='logical'):
            cdb.session.delete(switch_port)
        return ok

    def change_switch_port_network_id(self,from_network_id,to_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for switch_port in cdb.session.query(Switch_port).filter(Switch_port.network_id==from_network_id):            
            switch_port.network_id=to_network_id
        return ok


    def clean_ipaddr(self,from_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for switch_port in cdb.session.query(Switch_port).filter(Switch_port.network_id==from_network_id):
            switch_port.ipaddr=None
        return ok



    def fetch_from_name_network_name(self,equipment,network_name):
        """
        Public Method
        """
        cdb=self.cdb
#        print hba_rank
#        print port_rank

#        for npd in cdb.session.query(Node.name,Node_port.id,Node_port.ipaddr,Node_port.hwaddr,Node_port.hba_rank,Node_port.port_rank,Network.subnet).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name):
        for switch,switch_port,network in cdb.session.query(Switch,Switch_port,Network).filter(Network.name==network_name).filter(Switch.fusion_switch_id==None).filter(Switch.name==equipment.get_name()).filter(Switch_port.belong_switch_id==Switch.id).filter(Switch_port.network_id==Network.id):
            ni=network_interface()

            ni.network_name=network.name
            ni.network_id=network.id

            ni.id=switch_port.id
            ni.ipaddr=switch_port.ipaddr
            ni.hwaddr=switch_port.hwaddr

            ni.switch_id=switch_port.switch_id
            ni.switch_slot=switch_port.switch_slot
            ni.switch_port=switch_port.switch_port


            equipment.add_network_interface(ni)


    def store_network_interface(self,equipment):
        cdb=self.cdb
        for switch,switch_port in cdb.session.query(Switch,Switch_port).filter(Switch.name==equipment.get_name()).filter(Switch_port.belong_switch_id==Switch.id):
            for netint in equipment.get_all_network_interface():
                if (netint.id!=None and 
                    netint.forged==True):
                    print "TODO"

                if (netint.id==switch_port.id):
                    switch_port.ipaddr=netint.ipaddr
                    switch_port.network_id=netint.network_id

                    switch_port.switch_id=netint.switch_id
                    switch_port.switch_slot=netint.switch_slot
                    switch_port.switch_port=netint.switch_port









################################################################################


class disk_array_port_sql:

    def __init__(self,cdb):
        self.cdb=cdb


    def change_disk_array_port_network_id(self,from_network_id,to_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for disk_array_port in cdb.session.query(Disk_array_port).filter(Disk_array_port.network_id==from_network_id):            
            disk_array_port.network_id=to_network_id
        return ok


    def clean_ipaddr(self,from_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for disk_array_port in cdb.session.query(Disk_array_port).filter(Disk_array_port.network_id==from_network_id):
            disk_array_port.ipaddr=None
        return ok



    def fetch_from_name_network_name(self,equipment,network_name):
        """
        Public Method
        """
        cdb=self.cdb
#        print hba_rank
#        print port_rank

#        for npd in cdb.session.query(Node.name,Node_port.id,Node_port.ipaddr,Node_port.hwaddr,Node_port.hba_rank,Node_port.port_rank,Network.subnet).filter(Node.name==equipment.get_name()).filter(Node_port.node_id==Node.id).filter(Node_port.network_id==Network.id).filter(Network.name==network_name):
        for disk_array,disk_array_port,network in cdb.session.query(Disk_array,Disk_array_port,Network).filter(Network.name==network_name).filter(Disk_array.name==equipment.get_name()).filter(Disk_array_port.disk_array_id==Disk_array.id).filter(Disk_array_port.network_id==Network.id).order_by(Disk_array_port.ctrl_rank).order_by(Disk_array_port.port_rank):
            ni=network_interface()

            ni.network_name=network.name
            ni.network_id=network.id

            ni.id=disk_array_port.id
            ni.ipaddr=disk_array_port.ipaddr
            ni.hwaddr=disk_array_port.hwaddr

            ni.switch_id=disk_array_port.switch_id
            ni.switch_slot=disk_array_port.switch_slot
            ni.switch_port=disk_array_port.switch_port


            equipment.add_network_interface(ni)


    def store_network_interface(self,equipment):
        cdb=self.cdb
        for disk_array,disk_array_port in cdb.session.query(Disk_array,Disk_array_port).filter(Disk_array.name==equipment.get_name()).filter(Disk_array_port.disk_array_id==Disk_array.id):
            for netint in equipment.get_all_network_interface():
                if (netint.id!=None and 
                    netint.forged==True):
                    print "TODO"

                if (netint.id==disk_array_port.id):
                    disk_array_port.ipaddr=netint.ipaddr
                    disk_array_port.network_id=netint.network_id


                    disk_array_port.switch_id=netint.switch_id
                    disk_array_port.switch_slot=netint.switch_slot
                    disk_array_port.switch_port=netint.switch_port

        


################################################################################

class metaservice_port_sql:

    def __init__(self,cdb):
        self.cdb=cdb

    def change_metaservice_port_network_id(self,from_network_id,to_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for metaservice_port in cdb.session.query(Metaservice_port).filter(Metaservice_port.network_id==from_network_id):            
            metaservice_port.network_id=to_network_id
        return ok


    def clean_ipaddr(self,from_network_id):
        """
        change all network_id from network entry from the id "from_id" to the id "to_id"
        """
        cdb=self.cdb
        ok=False
        for metaservice_port in cdb.session.query(Metaservice_port).filter(Metaservice_port.network_id==from_network_id):            
            metaservice_port.ipaddr=None
        return ok



    def exist_metaservice_port_name(self,name):
        """
        Public Method
        """
        cdb=self.cdb
        ok=False
        for metaservice_port in cdb.session.query(Metaservice_port).filter(Metaservice_port.name==name):
            ok=True
        return ok


    def fetch_from_name(self,name):
        """
        Public Method
        """
        cdb=self.cdb
        e=equipment()
        for vp in cdb.session.query(Metaservice_port).filter(Metaservice_port.name==name):
            e.set_table_name("metaservice")
            e.set_name(vp.name)
            e.set_id(vp.id)
        return e



    def fetch_from_name_network_name(self,equipment,network_name):
        """
        Public Method
        """
        cdb=self.cdb
        
        for metaservice_port,network in cdb.session.query(Metaservice_port,Network).filter(Network.name==network_name).filter(Metaservice_port.name==equipment.get_name()).filter(Network.id==Metaservice_port.network_id):
            ni=network_interface()
        
            ni.network_name=network.name
            ni.network_id=network.id
            
            ni.network_id=metaservice_port.network_id
            ni.id=metaservice_port.id
            ni.ipaddr=metaservice_port.ipaddr
            ni.hwaddr=metaservice_port.hwaddr
            
            equipment.add_network_interface(ni)


    def store_network_interface(self,equipment):
        cdb=self.cdb
        for metaservice_port in cdb.session.query(Metaservice_port).filter(Metaservice_port.name==equipment.get_name()):
            for netint in equipment.get_all_network_interface():
                if (netint.id!=None and
                    netint.forged==True):
                    print "TODO"
                    
                if (netint.id==metaservice_port.id):
                    metaservice_port.ipaddr=netint.ipaddr
                    metaservice_port.network_id=netint.network_id


################################################################################


class load_ip_from_db:

    def __init__(self,cdb):
        self.cdb=cdb

        self.node_namelist=[]
        self.node_port_iplist=[]
        self.node_idlist=[]

        self.hwmanager_namelist=[]
        self.hwmanager_port_iplist=[]
        self.hwmanager_idlist=[]

        self.switch_namelist=[]
        self.switch_port_iplist=[]
        self.switch_idlist=[]

        self.disk_array_namelist=[]
        self.disk_array_port_iplist=[]
        self.disk_array_idlist=[]

	self.metaservice_port_namelist=[]
	self.metaservice_port_iplist=[]
	self.metaservice_port_idlist=[]

    def fetch_list_node(self):
        for node,node_port in self.cdb.session.query(Node,Node_port).filter(Node_port.ipaddr!=None).filter(Node.id==Node_port.node_id).order_by(Node_port.ipaddr):
            self.node_port_iplist.append(str(node_port.ipaddr))
            self.node_idlist.append(str(node_port.node_id))
            self.node_namelist.append(node.name)

    def fetch_list_hwmanager(self):
        for hwmanager,hwmanager_port in self.cdb.session.query(Hwmanager,Hwmanager_port).filter(Hwmanager_port.ipaddr!=None).filter(Hwmanager.id==Hwmanager_port.hwmanager_id).order_by(Hwmanager_port.ipaddr):
            self.hwmanager_port_iplist.append(str(hwmanager_port.ipaddr))
            self.hwmanager_idlist.append(str(hwmanager_port.hwmanager_id))
            self.hwmanager_namelist.append(hwmanager.name)

    def fetch_list_switch(self):
        for switch,switch_port in self.cdb.session.query(Switch,Switch_port).filter(Switch_port.ipaddr!=None).filter(Switch.id==Switch_port.belong_switch_id).order_by(Switch_port.ipaddr):
            self.switch_port_iplist.append(str(switch_port.ipaddr))
            self.switch_idlist.append(str(switch_port.belong_switch_id))
            self.switch_namelist.append(switch.name)

    def fetch_list_disk_array(self):
        for disk_array,disk_array_port in self.cdb.session.query(Disk_array,Disk_array_port).filter(Disk_array_port.ipaddr!=None).filter(Disk_array.id==Disk_array_port.disk_array_id).order_by(Disk_array_port.ipaddr):
            self.disk_array_port_iplist.append(str(disk_array_port.ipaddr))
            self.disk_array_idlist.append(str(disk_array_port.disk_array_id))
            self.disk_array_namelist.append(disk_array.name)

    def fetch_list_metaservice(self):
        for metaservice_port in self.cdb.session.query(Metaservice_port).filter(Metaservice_port.ipaddr!=None).order_by(Metaservice_port.ipaddr):
            self.metaservice_port_iplist.append(str(metaservice_port.ipaddr))
            self.metaservice_port_idlist.append(str(metaservice_port.id))
            self.metaservice_port_namelist.append(metaservice_port.name)

    def fetch(self):
        self.fetch_list_node()
        self.fetch_list_hwmanager()
        self.fetch_list_disk_array()
        self.fetch_list_switch()
	self.fetch_list_metaservice()


    def already_exist(self,ip_str):
        """
        Check if the ip_str : 10.0.0.24 is in the fetched ip from database
        Return True  if yes
        Return False if no
        """
        if ip_str in self.node_port_iplist:
            return True
        if ip_str in self.hwmanager_port_iplist:
            return True
        if ip_str in self.disk_array_port_iplist:
            return True
        if ip_str in self.switch_port_iplist:
            return True
        if ip_str in self.metaservice_port_iplist:
            return True
        return False


    def print_string(self):
        print ""

        print "NODE            :",
        print self.node_port_iplist
        print ""
        print "NODE ID         :",
        print self.node_idlist
        print ""
        print "NODE NAME       :",
        print self.node_namelist

        print ""
        print "HWMANGER        :",
        print self.hwmanager_port_iplist
        print ""
        print "HWMANGER ID     :",
        print self.hwmanager_idlist
        print ""
        print "HWMANGER NAME   :",
        print self.hwmanager_namelist

        print ""
        print "DISK_ARRAY      :",
        print self.disk_array_port_iplist
        print ""
        print "DISK_ARRAY ID   :",
        print self.disk_array_idlist
        print ""
        print "DISK_ARRAY NAME :",
        print self.disk_array_idlist

        print ""
        print "SWITCH        :",
        print self.switch_port_iplist
        print ""
        print "SWITCH ID     :",
        print self.switch_idlist
        print ""
        print "SWITCH NAME   :",
        print self.switch_namelist

	print "TODO => metaservice listing"

        print ""

    def ret_list(self):
        pass
################################################################################


class load_equipment:

    def __init__(self,cdb):
        self.member=None
        self.networkName=None
        self.interfaceName=None
        
    def set_member(self,member):
        self.member=member

    def set_network_name(self,networkName):
        self.networkName=networkName

    def set_interface_name(self,interfaceName):
        self.interfaceName=interfaceName


    def load(self):
        print ""
