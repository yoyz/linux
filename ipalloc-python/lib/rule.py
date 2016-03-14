from   CLM.DB.ipallocation.equipment                    import equipment_list,network_list,network,equipment,network_interface
from   CLM.DB.ipallocation.ip_range                     import ip_range
from   CLM.DB.ipallocation.ipaddr                       import ipaddr,ip_set,network
import misc

class rule_list:

    def __init__(self):
        self.rule_member_list=[]

    def add_rule(self,rule_member):
        rule_member.set_rule_number(len(self.rule_member_list))
        self.rule_member_list.append(rule_member)

    def get_rules(self):
        return self.rule_member_list

    def number_of_rule(self):
        return len(self.rule_member_list)

    def check_rules(self):
        """
        Public method

        Do a basic check of all rules defined in the list
        
        """
        ok=True
        for r in self.rule_member_list:
            if not r.check_attribute_loaded():
                ok=False
        return ok



    def sort_logical_port(self):
        rule_member_list=self.rule_member_list
        while True:
            for j in range(len(rule_member_list)-1):
                for i in range(len(rule_member_list)-1):                
                    if (rule_member_list[i+1].logical_network.get_type()=="secondary"):
                        rule_member_list[i+1], rule_member_list[i] = rule_member_list[i], rule_member_list[i+1]
            else: 
                break
        self.rule_member_list=rule_member_list


    def sort_logical_port_name(self):
        rule_member_list=self.rule_member_list
        while True:
            for j in range(len(rule_member_list)-1):
                for i in range(len(rule_member_list)-1):
                    #print misc.natcmp(rule_member_list[i].logical_network.get_name(),rule_member_list[i+1].logical_network.get_name())
                    if ((rule_member_list[i].logical_network.get_type()=="secondary" and
                        rule_member_list[i+1].logical_network.get_type()=="secondary" ) and
                        (misc.natcmp(rule_member_list[i].logical_network.get_name(),rule_member_list[i+1].logical_network.get_name())>0)):
                        rule_member_list[i+1], rule_member_list[i] = rule_member_list[i], rule_member_list[i+1]
            else: 
                break
        self.rule_member_list=rule_member_list




    def sort_first_ip(self):
        rule_member_list=self.rule_member_list
        while True:
            for i in range(len(rule_member_list)-1):
                if rule_member_list[i+1].logical_network.get_name()==rule_member_list[i].logical_network.get_name():
                    ip1=ipaddr(rule_member_list[i+1].range.get_first_ip())
                    ip2=ipaddr(rule_member_list[i].range.get_first_ip())
                    if ip1.intrep < ip2.intrep:
                        rule_member_list[i], rule_member_list[i+1] = rule_member_list[i+1], rule_member_list[i]
                        break
            else: 
                break
        self.rule_member_list=rule_member_list
    



    def __repr__(self):
        str=""
        for r in self.rule_member_list:
            str=str+r.__repr__()
        return str

class rule:
    """
    A rule is  composed of a set of different object :
    - a network object
    - a logical network object
    - a range object, which must be in the network subnet
    - a member object

    It also contain : 

    range, member and logical_network object are mandatory

    """
    def __init__(self):
        self.network=None          # the 'networkXML' tagged physical object
        self.logical_network=None  # the 'networkXML' tagged logical  object
        self.range=None            # the 'rangeXML'   object which contain the first_ip
        self.member=None           # the 'memberXML'  object

        self.fromDB=False          # loaded from DB 
        self.fromXML=False         # loaded from XML
        self.rule_number=0


    def get_rule_number(self):
        return self.rule_number


    def set_fromDB(self):
        self.fromDB=True
        self.fromXML=False


    def set_fromXML(self):
        self.fromDB=False
        self.fromXML=True


    def set_network(self,network):
        self.network=network

    def set_logical_network(self,logical_network):
        self.logical_network=logical_network


    def set_range(self,range):
        self.range=range

    def set_member(self,member):
        self.member=member

    def set_rule_number(self,value):
	self.rule_number = value


    def get_member(self):
        return self.member


    def get_range(self):
        return self.range

    def get_logical_network(self):
        return self.logical_network

    def get_network(self):
        return self.network


    def check_attribute_loaded(self):
        """
        Check the structure of a rule, 
        """
        ok=True
        str=""
        if self.fromDB==False and self.fromXML==False:
            ok=False
            str=str+"fromDB and fromXML not set. "

        if self.range==None:
            ok=False
            str=str+"range not set. "

        if self.member==None:
            ok=False
            str=str+"member not set. "
            
        if self.network==None and self.logical_network==None:
            ok=False
            str=str+"network or logical_network not set. "

        if str!="":
            print self.__repr__()
            print "Error : "
            print str

        return ok


#    def prepare_for_allocation(self):
#        """
#        Do the necessary stuff to initialize a 'IPRange' object
#        Has to be done after a 'check_attribute_loaded'
#        """
#
#        iprange=IPRange()
#        iprange=IPRange()
#        iprange.setNetworkIPAddress(self.logical_network.subnet+"/"+self.logical_network.netmask)
#        iprange.setFirstAddress(self.range.first_ip)
#        iprange.(self.logical_network.subnet+"/"+self.logical_network.netmask)
#        iprange.setFirstAddress(self.range.first_ip)
#        
#        if (iprange.check()):
#            self.iprange=iprange        
#            return True
#        else:
#            return False
#        
        


    def __repr__(self):
        str="<rule "
        str_end="</rule>"
        space="\t"
        line_feed="\n"
        endxml=">"

        if self.fromDB:
            str=str+" loaded_from_DB=yes  loaded_from_DB=yes"
        elif self.fromXML:
            str=str+" loaded_from_XML=yes loaded_from_DB=no "


        str=str+endxml+line_feed

        if (self.network):
            str=str+self.network.__repr__()
            str=str+space+line_feed

        if (self.logical_network):
            str=str+self.logical_network.__repr__()
            str=str+space+line_feed

        if (self.range):
            str=str+self.range.__repr__()
            str=str+space+line_feed

#        if (self.iprange):
#            str=str+self.iprange.__repr__()
#            str=str+space+line_feed

        if (self.member):
            str=str+self.member.__repr__()
            str=str+space+line_feed
        str=str+str_end+space+line_feed+line_feed
        return str
