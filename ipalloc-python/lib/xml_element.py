from error_code import error_code



class networkXML:
    """
    A networkXML class is used to store a network table from the xml document
    """

    def __init__(self):
        self.XMLname=None
        self.XMLparent_name=None
        self.XMLtype=None
        self.XMLsubnet=None
        self.XMLgateway=None
        self.XMLvlan_id=None
        self.XMLfunction=None
        self.XMLsuffix=None

        self.is_a_network=False
        self.is_a_logical_network=False


    def set_name(self,name):
        self.XMLname=name

    def set_parent_name(self,parent_name):
        self.XMLparent_name=parent_name

    def set_type(self,type):
        self.XMLtype=type

    def set_function(self,function):
        self.XMLfunction=function


    def set_subnet(self,subnet):
        self.XMLsubnet=subnet

    def set_gateway(self,gateway):
        self.XMLgateway=gateway

    def set_vlan_id(self,vlan_id):
        self.XMLvlan_id=vlan_id

    def set_pkey(self,pkey):
        self.XMLvlan_id=pkey

    def set_vpartition(self,vpartition):
        self.XMLvlan_id=vpartition

    def set_suffix(self,suffix):
        self.XMLsuffix=suffix

    


    def get_name(self):
        return self.XMLname

    def get_parent_name(self):
        return self.XMLparent_name

    def get_type(self):
        return self.XMLtype

    def get_subnet(self):
        return self.XMLsubnet

    def get_gateway(self):
        return self.XMLgateway

    def get_vlanid(self):
        return self.XMLvlan_id

    def get_pkey(self):
        return self.XMLvlan_id

    def get_vpartition(self):
        return self.XMLvlan_id

    def get_function(self):
        return self.XMLfunction

    def get_suffix(self):
        return self.XMLsuffix



    def clone_nw(self):
        nw_ret=networkXML()
        
        nw_ret.XMLname          = self.XMLname
        nw_ret.XMLfunction      = self.XMLfunction
        nw_ret.XMLparent_name   = self.XMLparent_name
        nw_ret.XMLtype          = self.XMLtype
        nw_ret.XMLsubnet        = self.XMLsubnet
        nw_ret.XMLgateway       = self.XMLgateway
        nw_ret.XMLvlan_id       = self.XMLvlan_id
        nw_ret.XMLsuffix        = self.XMLsuffix

        nw_ret.is_a_network=self.is_a_network
        nw_ret.is_a_logical_network=self.is_a_logical_network

        return nw_ret


    def set_is_a_network(self):
        self.is_a_network=True
        self.is_a_logical_network=False

    def set_is_a_logical_network(self):
        self.is_a_network=False
        self.is_a_logical_network=True


    def get_is_a_network(self):
        if (self.is_a_network==True and
            self.is_a_logical_network==False):
            return True
        else:
            return False

    def get_is_a_logical_network(self):
        if (self.is_a_network==False and
            self.is_a_logical_network==True):
            return True
        else:
            return False


    def is_a_son(self):
        ret=False
        if (self.XMLparent_name!=None):
            ret=True
        return ret


    def is_a_parent(self):
        ret=False
        if (self.XMLparent_name==None):
            ret=True
        return ret

    def __repr__(self):
        str="\t<undefined_networkXML/>"
        if self.is_a_network:
            str="\t<networkXML name='%s' function='%s'/>" % (self.XMLname,self.XMLfunction)
        if self.is_a_logical_network:
            str="\t<logical_networkXML parent_name='%s' name='%s' type='%s' subnet='%s' gateway='%s' vlan_id='%s' suffix='%s'/>" % (self.XMLparent_name,self.XMLname,self.XMLtype,self.XMLsubnet,self.XMLgateway,self.XMLvlan_id,self.XMLsuffix)
        return str

    
class rangeXML:
    """
    A rangeXML is used to store a 'first_ip' attribute
        
    """

    def __init__(self):
        self.first_ip=None


    def set_first_ip(self,first_ip):
        self.first_ip=first_ip

    def get_first_ip(self):
        return self.first_ip



    def clone_range(self):
        r=rangeXML()
        r.first_ip=self.first_ip
        return r

    def __repr__(self):
        return "\t<rangeXML first_ip='%s' />" % (self.first_ip)
        


class memberXML:
    """
    A memberXML is composed of three different thing : 
    - a type    : called 'object'   node,hwmanager, switch
    - a subtype : called 'type'     compute, mngt,ws, bmc, cmc  
    - a group   : called 'group'    islet1, admin, colddoor
    """

    def __init__(self):
        self.obj="*"     # node, hwmanager
        self.type="*"    # type of ftype
        self.group="*"   # hwgroup name
        self.iname="*"   # interface name
        self.regex="*"

    def get_object(self):
        return self.obj

    def get_type(self):
        return self.type

    def get_group(self):
        return self.group

    def get_interface_name(self):
        return self.iname

    def get_iname(self):
        return self.iname

    def set_iname(self,iname):
        self.set_interface_name(iname)

    def set_regex(self,regex):
        self.regex=regex

    def get_regex(self):
        return self.regex


    def set_interface_name(self,iname):
        permited_object=["*",
                         "eth0","eth1","eth2","eth3","eth4","eth5","eth6","eth7","eth8",
                         "ib0","ib1","ib2","ib3","ib4","ib5","ib6","ib7","ib8"]
        self.iname=iname


        
    def set_object(self,obj):
        permited_object=["*","node","hwmanager","switch","disk_array","metaservice"]
        if (obj in permited_object):
            self.obj=obj
        else:
            self.obj=obj
            err=error_code()
            err.xml_member_object_not_implemented(obj)
#            err.sysexit()

    def set_type(self,type):
        permited_type=["*",
                       "ADMIN","STATION","IO","MDS","OSS","NFS","MWS","MWS",
                       "admin","station","io","mds","oss","nfs","mws","mngt",
                       "LOGIN","STATION", "COMPUTE","MNGT","WS",               # ftype.label
                       "login","station", "compute","mngt","ws",               # ftype.label
                       "BMC","CMC","COLDOOR",                                  # hwmanager.type
                       "bmc","cmc","coldoor",                                  # hwmanager.type                       
                       "eth","ib","talim"]                                     # switch.type
                       

        if (type in permited_type):
            self.type=type
        else:
            self.type=type
            #err=error_code()
            #err.xml_member_type_not_implemented(type)


    def set_group(self,group):
        permited_group=["*"]
        self.group=group


    def get_subset(self):
        """
        return a string to match to network.subset
        the string is empty if the member match all
        """
        str=""
        if self.get_object()!="*":
            if len(str)==0:
                str=str+"object="+self.get_object()

        if self.get_type()!="*":
            if len(str)==0:
                str=str+"type="+self.get_type()
            else:
                str=str+","+"type="+self.get_type()

        if self.get_group()!="*":
            if len(str)==0:
                str=str+"group="+self.get_group()
            else:
                str=str+","+"group="+self.get_group()

        if self.get_iname()!="*":
            if len(str)==0:
                str=str+"iname="+self.get_iname()
            else:
                str=str+","+"iname="+self.get_iname()

        if self.regex!="*":
            if len(str)==0:
                str=str+"regex="+self.get_regex()
            else:
                str=str+","+"regex="+self.get_regex()

        return str
        

    def __repr__(self):
        return "\t<memberXML object='%s' type='%s' group='%s' regex=%s' />" % (
            self.get_object(),
            self.get_type(),
            self.get_group(),
            self.get_regex())
    
