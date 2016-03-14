from   CLM.DB.ipallocation.error_code                   import error_code
from   CLM.DB.ipallocation.clusterdb_sqlalchemy         import clusterdb_alchemy,Cluster,Network,Node,Node_port,Hwmanager,Hwmanager_port,Node_ftype,Ftype,Hw_group,Hw_group_member,Switch,Switch_port,Node_profile,Node_profile_device,Disk_array,Metaservice_port
from   CLM.DB.ipallocation.clusterdb_sql_request        import member_sql,interface_member_sql,network_sql,node_profile_device_sql,node_sql,node_port_sql,hwmanager_port_sql,hwmanager_sql,switch_sql,switch_port_sql,disk_array_sql,disk_array_port_sql,hw_group_sql,metaservice_port_sql
from   CLM.DB.ipallocation.equipment                    import equipment_list,network_list,network,equipment,network_interface
from   CLM.DB.ipallocation.xml_element                  import networkXML,memberXML,rangeXML
from   CLM.DB.ipallocation.ip_range                     import ip_range
from   CLM.DB.ipallocation.ipaddr                       import ipaddr,ip_set,network
from   CLM.DB.ipallocation.network_range                import network_range_before_2140
from   CLM.DB.ipallocation.rule                         import rule,rule_list
from   CLM.DB.ipallocation.allocation_rule              import network_allocation_rule
import misc
import sys,getopt,re


class fetch_store_equipment:
    """
    The class in charge of fetching and commiting object from the database
    It is used with an allocation rule
    """
    def __init__(self):
        self.ea=None                  # engine argument
        self.object_type=None         # node      hwmamanger
        self.object_subtype=None      # compute   cmc,bmc
        self.object_hwgroup=None         # islet
        self.iname=None

        self.subnet=None             # network cidr mandatory or
        self.net_name=None           # network name mandatory

        self.regex_name=None         # if (None): => don't use regex for equipment name matching

        self.equipment_list=None

        self.verbose=None

        self.hba_rank=None
        self.port_rank=None

        self.m=None                  # memberXML

                                     # SQL request
        self.hgs=None

        self.ms=None                 # member_sql

        self.ns=None                 # node_sql
        self.hs=None                 # hwmanager_sql
        self.ds=None                 # disk_array_sql

        self.dps=None                # disk_array_port_sql
        self.hps=None                # hwmanager_port_sql
        self.nps=None                # node_port_sql
        self.sps=None                # switch_port_sql
        self.mps=None                # metaservice_port_sql

        self.nws=None                # network_sql


#        self.eq=None                # equipment
        self.eql=None                # equipmentList
        self.nwl=None                # networkList   
        self.nw=None                 # Current network

        self.cdb=None

        self.nameList=[]

        self.pb_counter=100
        self.pb_counter_start='['
        self.pb_counter_end=']'
        self.pb_counter_symbol='-'


    def set_engine_argument(self,ea):
        self.ea=ea
    

    def fetch_init(self,cdb):
        self.cdb=cdb
        self.m=memberXML()

        self.hgs=hw_group_sql(cdb)

        self.ms=member_sql(cdb)

        self.ns=node_sql(cdb)
        self.hs=hwmanager_sql(cdb)
        self.ss=switch_sql(cdb)
        self.ds=disk_array_sql(cdb)

        self.nps=node_port_sql(cdb)
        self.hps=hwmanager_port_sql(cdb)
        self.sps=switch_port_sql(cdb)
        self.dps=disk_array_port_sql(cdb)

        self.mps=metaservice_port_sql(cdb)

        self.nws=network_sql(cdb)

        self.eql=equipment_list()        
        self.nwl=network_list()

        self.object_type=self.ea.get_object_type()
        self.object_subtype=self.ea.get_object_subtype()
        self.object_hwgroup=self.ea.get_object_hwgroup()
        self.iname=self.ea.get_object_iname()
        self.regex_name=self.ea.get_object_regex()
        self.subnet=self.ea.get_subnet()
        self.net_name=self.ea.get_network_name()

        
        if self.object_type==None:
            self.m.set_object("*")
        else:
            self.m.set_object(self.object_type)
        if self.object_subtype==None:
            self.m.set_type("*")
        else:
            self.m.set_type(self.object_subtype)


        if self.object_hwgroup==None:
            self.m.set_group("*")
        else:
            self.m.set_group(self.object_hwgroup)
        
        if self.iname==None:
            self.m.set_iname("*")
        else:
            self.m.set_iname(self.iname)

        if self.regex_name==None:
            self.m.set_regex("*")
        else:
            self.m.set_regex(self.regex_name)
        

        self.ms.set_member(self.m)


    def get_member(self):
        return self.m

    def get_logical_network_name(self):
        return self.net_name

    def get_subnet(self):
        return self.subnet


    def fetch_network_list(self):
        self.nwl=self.nws.fetch_all_network()
        
    def get_equipment_list(self):
        return self.eql


    def check_input_validity(self):
        ok=True
        print "-> check_input_validity"
        #if self.check_input_network()==False:
        #    ok=False
        if self.check_input_hw_group()==False:
            ok=False
        return ok


    def check_input_hw_group(self):
        ok=False
        if self.m.get_group()=="*":
            ok=True
        else:
            hwgl=self.hgs.fetch_all_hw_group()
            for hwg in hwgl:
                if hwg==self.object_hwgroup:
                    ok=True
        if ok==False:
            print("==> hw_group not found in the database")
        return ok


    def check_input_network(self):
        ok=True        

        if (self.subnet!=None and self.net_name!=None):
            print("==> subnet or network name has to be given not both")
            return False

        if (self.subnet==None and self.net_name==None):
            return False
        ok_net=False
        for nw in self.nwl.get_all_network():
            if nw.get_name()==self.net_name:
                self.subnet=nw.get_subnet()
                ok_net=True
                
            if (nw.get_subnet()!=None and
                nw.get_subnet()==self.subnet):
                self.net_name=nw.get_name()
                ok_net=True

        if ok_net==False:
            print("==>subnet or network name not found in database")
            sys.exit(1)
        return ok


    def filter_by_regex(self,list_of_name):
        if self.regex_name==None:
            return list_of_name
        if len(list_of_name)>0:
                temp_list_start=list_of_name
                temp_list_dest=[]
                for name in temp_list_start:
                    if re.match(self.regex_name,name):
                        temp_list_dest.append(name)
        return temp_list_dest


    def fetch_equipment_name(self):
        """
        fetch equipment name
        filter by regex if there is one
        """
        if self.ms.get_equipment_name_list()==0:
            print("No element fetched : fetch_equipment_name()==0")
            return False

        #nameListToFilter=misc.natsorted(nameListToSort)
        self.ms.set_list_node(misc.natsorted(self.ms.get_list_node()))
        self.ms.set_list_hwmanager(misc.natsorted(self.ms.get_list_hwmanager()))
        self.ms.set_list_switch(misc.natsorted(self.ms.get_list_switch()))
        self.ms.set_list_diskarray(misc.natsorted(self.ms.get_list_diskarray()))
        self.ms.set_list_metaservice(misc.natsorted(self.ms.get_list_metaservice()))
        
        if self.regex_name==None:
            return True
        else:
            if len(self.ms.get_list_node())>0:
                self.ms.set_list_node(         self.filter_by_regex(self.ms.get_list_node()))
            if len(self.ms.get_list_hwmanager())>0:
                self.ms.set_list_hwmanager(    self.filter_by_regex(self.ms.get_list_hwmanager()))
            if len(self.ms.get_list_switch())>0:
                self.ms.set_list_switch(       self.filter_by_regex(self.ms.get_list_switch()))
            if len(self.ms.get_list_diskarray())>0:
                self.ms.set_list_diskarray(    self.filter_by_regex(self.ms.get_list_diskarray()))
            if len(self.ms.get_list_metaservice())>0:
                self.ms.set_list_metaservice(  self.filter_by_regex(self.ms.get_list_metaservice()))


    def fetch_equipment(self):
        """
        fetch from the db the equipment associated with the rule
        """
        if (self.verbose):
            print("**fetch**")

        print("-> fetch_equipment")
        print("--> subset         : "+self.m.get_subset())
        print("--> network name   : "+str(self.net_name))


        if (self.m.get_object()=="*" or
            self.m.get_object()=="node"):
            for name in self.ms.get_list_node():
                eq=self.ns.fetch_from_name(name)
                self.eql.add_equipment(eq)

        if (self.m.get_object()=="*" or
            self.m.get_object()=="hwmanager"):
            for name in self.ms.get_list_hwmanager():
                eq=self.hs.fetch_from_name(name)
                self.eql.add_equipment(eq)

        if (self.m.get_object()=="*" or
            self.object_type=="switch"):
            for name in self.ms.get_list_switch():
                eq=self.ss.fetch_from_name(name)
                self.eql.add_equipment(eq)

        if (self.m.get_object()=="*" or
            self.m.get_object()=="disk_array"):
            for name in self.ms.get_list_diskarray():
                eq=self.ds.fetch_from_name(name)
                self.eql.add_equipment(eq)

        if (self.m.get_object()=="*" or
            self.m.get_object()=="metaservice"):
            for name in self.ms.get_list_metaservice():
                eq=self.mps.fetch_from_name(name)
                self.eql.add_equipment(eq)


    def fetch_equipment_port(self):
        print("-> fetch_equipment_port")

        nw=None
        if (self.net_name!=None):
            nw=self.nwl.get_network_by_name(self.net_name)

        permited_object=["node","hwmanager","switch","disk_array","metaservice"]

        if (self.m.get_object() in permited_object or
            self.m.get_object() == "*"):
            if (self.net_name==None):
                for nw in self.nwl.get_all_network():
                    if self.m.get_object() == "*":
                        for obj in permited_object:
                            self.fetch_equipment_port_next(obj,nw)
                    else:
                        self.fetch_equipment_port_next(self.m.get_object(),nw)
            else:
                if self.m.get_object() == "*":
                    for obj in permited_object:
                        self.fetch_equipment_port_next(obj,nw)
                else:
                    self.fetch_equipment_port_next(self.m.get_object(),nw)


    def fetch_equipment_port_next(self,table_name,nw):
        """
        fetch from the db the port associated with the equipment
        """
        i=0
        eq=equipment()
        iname=None
        if self.m.get_iname()==None:
            iname='*'
        else:
            iname=self.m.get_iname()

        for eq in self.eql.equipment_list:
            if eq.get_table_name()!=table_name:
                continue

            if i==0:
                sys.stdout.write("-> "+table_name+"_port "+nw.name+"\t\n-->"+self.pb_counter_start)

            if eq.get_table_name()=="node":
                self.nps.fetch_from_name_network_name_iname(eq,nw.get_name(),iname)
            if eq.get_table_name()=="hwmanager":
                self.hps.fetch_from_name_network_name(eq,nw.get_name())
            if eq.get_table_name()=="disk_array":
                self.dps.fetch_from_name_network_name(eq,nw.get_name())
            if eq.get_table_name()=="switch":
                self.sps.fetch_from_name_network_name(eq,nw.get_name())
            if eq.get_table_name()=="metaservice":
                self.mps.fetch_from_name_network_name(eq,nw.get_name())

            if i==1:
                sys.stdout.write(self.pb_counter_symbol)
            if i==self.pb_counter:
                i=0
                sys.stdout.write(self.pb_counter_symbol)
                sys.stdout.flush()
            i=i+1
        if i>0:
            print(self.pb_counter_end)


    def print_interface_collected(self):
        numberOfNetworkInterfaceToAlloc=0
        for eq in self.eql.get_all():
            for net in eq.get_all_network_interface():
                numberOfNetworkInterfaceToAlloc=numberOfNetworkInterfaceToAlloc+1
        print("-> interfaces matching the rule : "+str(numberOfNetworkInterfaceToAlloc))


    def commit(self):
        if (self.verbose):
            print("**Commit**")
        for eq in self.eql.get_all():
            if eq.get_table_name() == "node":
                self.nps.store_network_interface(eq)
            elif eq.get_table_name() == "hwmanager":
                self.hps.store_network_interface(eq)
            elif eq.get_table_name() == "switch":
                self.sps.store_network_interface(eq)
            elif eq.get_table_name() == "disk_array":
                self.dps.store_network_interface(eq)
            elif eq.get_table_name() == "metaservice":
                self.mps.store_network_interface(eq)

        self.cdb.session.commit()


    def list_equipment(self):
        if (self.verbose):
            print("**equipment list**")
        print(self.eql)


    def list_network(self):
        if (self.verbose):
            print "**network list**"
        print self.nwl


