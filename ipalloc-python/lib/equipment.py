class dbelement:
    def __init__(self):
        self.__table_name__="AbstractDBElement"
        self.forged=False


    def set_table_name(self,table):
        self.__table_name__=table
    

    def __repr__(self):
        print "<dbelement>"

class equipment_list:

    def __init__(self):
        self.equipment_list=[]

    def add_equipment(self,equipment):
        self.equipment_list.append(equipment)

    def number_of_elements(self):
        return len(self.equipment_list)

    def get_all(self):
        return self.equipment_list

    def get_all_equipment(self):
        return self.get_all()


    def __repr__(self):
        space="\n"
        str="<equipment_list>\n"
        str_end="</equipment_list>\n"
        for eq in self.equipment_list:
            if eq.get_number_of_network_interface()>0:
                str=str+eq.__repr__()+space
        str=str+"\n"+str_end
        return str


class equipment(dbelement):

    def __init__(self):
        self.__table_name__=None
        self.equipment_name=None
        self.id=None
        self.profile=None
        self.profile_id=None
        self.network_interface=[]


    def get_profile(self):
        return self.profile
 
    def set_profile(self,profile):
        self.profile=profile

    def set_profile_id(self,profile_id):
        self.profile_id=profile_id


    def get_table_name(self):
        return self.__table_name__

    def get_all_network_interface(self):
        return self.network_interface

    def get_number_of_network_interface(self):
        i=0
        for ni in self.network_interface:
            i=i+1
        return i

    def set_name(self,name):
        self.equipment_name=name

    def set_id(self,id):
        self.id=id

    def get_name(self):
        return self.equipment_name


    def get_id(self):
        return self.id


    def add_network_interface(self,net_int):
        #print len(self.network_interface)
        self.network_interface.append(net_int)
        #print len(self.network_interface)

    def __repr__(self):
        space="\n\t"
        
        str="\t<equipment"
        if self.__table_name__:
            str=str+" table_name='%s'" % self.__table_name__

        if self.id:
            str=str+" id='%s'" % (self.id)

        if self.equipment_name:
            str=str+" name='%s'" % (self.equipment_name)

        if self.profile:
            str=str+" profile='%s'" % (self.profile)

        if self.profile_id:
            str=str+" profile_id='%s'" % (self.profile_id)


        str=str+">"
        str_end="</equipment>\n"
        str=str+space
        for ni in self.network_interface:
            str=str+ni.__repr__()+space
        str=str+str_end
        return str

class network_interface(dbelement):
    """
    Represent a network device of an equipment
    This class is used to store : 'node_port', 'da_port','hwmanager_port','switch_port'
    """

    def __init__(self):
        self.forged=False

        self.interface_name=None
        self.network_name=None

        self.id=None
        self.network_id=None

        self.type=None

        self.fusion_port_id=None

        # For Ethernet and infiniband device
        self.ipaddr=None
        self.hwaddr=None
        
        # For Node_port only
        self.function=None
        self.fusion_port_id=None
        self.hba_rank=None
        self.port_rank=None

        self.switch_id=None
        self.switch_slot=None
        self.switch_port=None
        self.node_profile_device_id=None

    

    def set_forged(self):
        self.forged=True

    def set_function(self,function):
        self.function=function

    def set_interface_name(self,interface_name):
        self.interface_name=interface_name

    def set_iname(self,interface_name):
        self.interface_name=interface_name

    def set_network_id(self,network_id):
        self.network_id=network_id

    def set_network_name(self,network_name):
        self.network_name=network_name
    
    def set_ctrl_rank(self,hba_rank):
        self.hba_rank=hba_rank

    def set_hba_rank(self,hba_rank):
        self.hba_rank=hba_rank

    def set_port_rank(self,port_rank):
        self.port_rank=port_rank

    def set_ipaddr(self,ipaddr):
        self.ipaddr=ipaddr

    def set_hwaddr(self,hwaddr):
        self.hwaddr=hwaddr

    def set_id(self,id):
        self.id=id

    def set_type(self,type):
        self.type=type

    def set_node_profile_device_id(self,node_profile_device_id):
        self.node_profile_device_id=node_profile_device_id
    
    def set_switch_id(self,switch_id):
        self.switch_id=switch_id

    def set_switch_slot(self,switch_slot):
        self.switch_slot=switch_slot

    def set_switch_port(self,switch_port):
        self.switch_port=switch_port

    def set_fusion_port_id(self,fusion_port_id):
        self.fusion_port_id=fusion_port_id



    def get_function(self):
        return self.function

    def get_id(self):
        return self.id

    def get_hwaddr(self):
        return self.hwaddr

    def get_ipaddr(self):
        return self.ipaddr

    def get_node_profile_device_id(self):
        return self.node_profile_device_id

    def get_switch_id(self):
        return self.switch_id

    def get_switch_slot(self):
        return self.switch_slot

    def get_switch_port(self):
        return self.switch_port

    def get_forged(self):
        return self.forged

    def get_type(self):
        return self.type


    def get_interface_name(self):
        return self.interface_name

    def get_iname(self):
        return self.interface_name

    def get_network_id(self):
        return self.network_id

    def get_network_name(self):
        return self.network_name

    def get_hba_rank(self):
        return self.hba_rank

    def get_ctrl_rank(self):
        return self.hba_rank

    def get_port_rank(self):
        return self.port_rank

    def get_ipaddr(self):
        return self.ipaddr

    def get_network_id(self):
        return self.network_id

    def get_fusion_port_id(self):
        return self.fusion_port_id

    def get_description(self):
        return self.fusion_port_id



    def clone(self):
        #print "CLONE "+str(self.get_id())
        ni=network_interface()
        ni.set_forged()
        #ni.set_type("logical")
        ni.set_interface_name(self.get_interface_name())
        ni.set_id(self.get_id())
        ni.set_ipaddr(self.get_ipaddr())
        ni.set_hwaddr(self.get_hwaddr())
        ni.set_fusion_port_id(self.get_fusion_port_id())
        ni.set_function(self.get_function())
        ni.set_hba_rank(self.get_hba_rank())
        ni.set_port_rank(self.get_port_rank())
        ni.set_node_profile_device_id(self.get_node_profile_device_id())
        ni.set_switch_id(self.get_switch_id())
        ni.set_switch_slot(self.get_switch_slot())
        ni.set_switch_port(self.get_switch_port())
        return ni

        
    def __repr__(self):
        str="\t\t<network_interface"
        str_end=' />'

        if self.id:
            str=str+" id='%s'" % (self.id)

        if self.interface_name:
            str=str+" iname='%s'" % (self.interface_name)

        if self.type:
            str=str+" type='%s'" % (self.type)

        if self.ipaddr:
            str=str+" ipaddr='%s'" % (self.ipaddr)

        if self.hwaddr:
            str=str+" hwaddr='%s'" % (self.hwaddr)

        if self.network_name:
            str=str+" net_name='%s'" % (self.network_name)

        if self.network_id:
            str=str+" net_id='%s'" % (self.network_id)

        if self.hba_rank!=None:
            str=str+" hba='%s'" % (self.hba_rank)
        if self.port_rank!=None:

            str=str+" port='%s'" % (self.port_rank)
        str=str+str_end
        return str




class network_list:
    """
    Represent a list of 'nw' object
    """
    def __init__(self):
        self.nwl=[]

    def get_all_network_and_range(self):
        return self.nwl

    def get_all_network(self):
        nwl=[]
        for nw in self.nwl:
            if nw.type!='range':
                nwl.append(nw)
        return nwl


    def add_network(self,nw):
        self.nwl.append(nw)

    def get_network_by_name(self,name):
        ret_nw=None
        for n in self.nwl:
            if (n.name==name):
                ret_nw=n
        return ret_nw

    def get_network_by_id(self,id):
        """
        Return a 'nw' object from the network.id in the db

        Note : the id parameter can be a int or a string for ease of use 
        """
        ret_nw=None
        for n in self.nwl:
            if (n.id==int(id)):
                ret_nw=n
        return ret_nw


    def get_network_by_subnet(self,subnet):
        """
        return a 'nw' object from the network.subnet in the db

        Note: The subnet parameter is a string
        """
        ret_nw=None
        for n in self.nwl:
            if (n.subnet==subnet):
                ret_nw=n
        return ret_nw
    

    def __repr__(self):
        str="<network_list>\n"
        str_end="</network_list>\n"
        for n in self.nwl:
            str=str+"\t"+n.__repr__()+"\n"
        str=str+str_end
        return str
            


class network(dbelement):
    """
    A network class used to store a network table entry of the database
    """
    def __init__(self):
        self.id=None
        self.name=None
        self.type=None
        self.function=None
        self.subset=None
        self.techno=None
        self.subnet=None
        self.gateway=None
        self.network_id=None
        self.next_ip=None
        self.vpartition=None
        self.comment=None

        self.next_ip=None

        self.__parent_name=None
#        self.__type=None
#        self.__netmask=None
#        self.__vlan_id=None


    def set_id(self,myid):
        self.id=myid

    def set_name(self,myname):
        self.name=myname
    
    def set_type(self,mytype):
        self.type=mytype

    def set_function(self,myfunction):
        self.function=myfunction


    def set_subset(self,mysubset):
        self.subset=mysubset

    def set_techno(self,mytechno):
        self.techno=mytechno

    def set_subnet(self,mysubnet):
        self.subnet=mysubnet


    def set_gateway(self,mygateway):
        self.gateway=mygateway

    def set_network_id(self,mynetwork_id):
        self.network_id=mynetwork_id

    def set_next_ip(self,mynextip):
        self.next_ip=mynextip

    def set_first_ip(self,myfirstip):
        self.next_ip=myfirstip

    def set_vpartition(self,vpartition):
        self.vpartition=vpartition


    def set_comment(self,mycomment):
        self.comment=mycomment

    def set_parent_name(self,parent_name):
        self.__parent_name=parent_name


    def get_id(self):
        return self.id

    def get_name(self):
        return self.name

    def get_type(self):
        return self.type
    
    def get_function(self):
        return self.function

    def get_subset(self):
        return self.subset

    def get_techno(self):
        return self.techno

    def get_subnet(self):
        return self.subnet

    def get_gateway(self):
        return self.gateway

    def get_network_id(self):
        return self.network_id


    def get_next_ip(self):
        return self.next_ip

    def get_first_ip(self):
        return self.next_ip

    def get_vpartition(self):
        return self.vpartition


    def get_comment(self):
        return self.comment

    def get_parent_name(self):
        return self.__parent_name



    def clone_network(self):
        nw_ret=network()
        nw_ret.id=self.id
        nw_ret.name=self.name
        nw_ret.type=self.type
        nw_ret.function=self.function
        nw_ret.subset=self.subset
        nw_ret.techno=self.techno
        nw_ret.subnet=self.subnet
        nw_ret.gateway=self.gateway
        nw_ret.network_id=self.network_id
        nw_ret.next_ip=self.next_ip
        nw_ret.comment=self.comment
        nw_ret.vpartition=self.vpartition
        
        nw_ret.__parent_name=self.__parent_name
        
        return nw_ret


    def is_a_son(self):
        son=False
        if (self.network_id==""
            or
            self.__parent_name!=""):
            son=True
        return son


    def is_a_parent(self):
        ret=False
        son=self.is_a_son()
        if (son): 
            ret=False
        else:
            ret=True
        return ret


    def __repr__(self):
        i=0
        str=""
        str=str+"\t<network"
        if self.name:
            str=str+" name='%s'" % (self.name)
            i=i+1
        if self.id:
            str=str+" id='%s'" % (self.id)
            i=i+1
        if self.network_id:
            str=str+" network_id='%s'" % (self.network_id)
            i=i+1
        if self.__parent_name:
            str=str+" parent_name='%s'" % (self.__parent_name)
            i=i+1
        if self.type:
            str=str+" type='%s'" % (self.type)
            i=i+1
        if self.function:
            str=str+" function='%s'" % (self.function)
            i=i+1
        if self.techno:
            str=str+" techno='%s'" % (self.techno)
            i=i+1
        if i>5:
            str=str+"\n\t\t"

        if self.subset:
            str=str+" subset='%s'" % (self.subset)

        if self.subnet:
            str=str+" subnet='%s'" % (self.subnet)
        if self.gateway:
            str=str+" gateway='%s'" % (self.gateway)
        if self.next_ip:
            str=str+" next_ip='%s'" % (self.next_ip)
        if self.vpartition:
            str=str+" vpartition='%s'" % (self.vpartition)


        str=str+"/>\n"

        return str

#        return "<network parent_name='%s',type='%s',netmask='%s',vlan_id='%s' id='%s' name='%s' type='%s' function='%s' subset='%s' techno='%s' subnet='%s' gateway='%s' cluster_id='%s' network_id='%s' comment='%s' />" % (parent,type,netmask,vlan_id,self.id,self.name,self.type,self.function,self.subset,self.techno,self.subnet,self.gateway,self.cluster_id,self.network_id,self.comment)
#        return "<network parent_name='%s',type='%s',netmask='%s',vlan_id='%s' id='%s' name='%s' type='%s' function='%s' subset='%s' techno='%s' subnet='%s' gateway='%s' cluster_id='%s' network_id='%s' comment='%s' />" % (parent,type,netmask,vlan_id,self.id,self.name,self.type,self.function,self.subset,self.techno,self.subnet,self.gateway,self.cluster_id,self.network_id,self.comment)




            

