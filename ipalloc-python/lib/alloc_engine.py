from   CLM.DB.ipallocation.clusterdb_glue                 import clusterdb_glue
#from   CLM.DB.ipallocation.alloc_ip			import fetch_store_equipment,alloc_ip,move_network_interface,clone_primary_network_interface
from   CLM.DB.ipallocation.alloc_fetch_update             import fetch_store_equipment
from   CLM.DB.ipallocation.operation_on_fetched_equipment import extend_ip,clean_ip,move_network_interface,create_logical_network_interface,create_logical_network,remove_logical_network,create_range,remove_range,create_member,remove_member
from   CLM.DB.ipallocation.apply_xml			import alloc_xml
import sys

class alloc_engine:

#    def connect_db(self):
        

    def __init__(self):
##############################################################
        self.fse=None                # fetch_store_equipment object
        self.cdb_glue=None           # clusterdb glue
        self.operation_object=None   # operation on fetched equipment
        self.ea=None                 # engine argument
##############################################################
#                                     #
#                                     # ObjectSQL 
#                                     #
#        self.hgs=None                #
#        self.ms=None                 # member_sql
#        self.ns=None                 # node_sql
#        self.hs=None                 # hwmanager_sql
#        self.ds=None                 # disk_array_sql
#        self.dps=None                # disk_array_port_sql
#        self.hps=None                # hwmanager_port_sql
#        self.nps=None                # node_port_sql
#        self.nws=None                # network_sql
#        self.vps=None                # metaservice_port_sql
##############################################################


    def set_engine_argument(self,ea):
        self.ea=ea

    def connect_db(self):
        self.cdb_glue=clusterdb_glue()
        if self.ea.get_database_username()!=None:
            self.cdb_glue.set_user(self.ea.get_database_username())
        if self.ea.get_database_password()!=None:
            self.cdb_glue.set_passwd(self.ea.get_database_password())
        if self.ea.get_database_name()!=None:
            self.cdb_glue.set_name(self.ea.get_database_name())
        if self.ea.get_database_hostname()!=None:
            self.cdb_glue.set_host(self.ea.get_database_hostname())

        if self.cdb_glue.fetch_and_check()==True:
            return True
        else:
            return False


    def prepare_engine_and_fetch_store(self):
        if self.operation_object:
            self.operation_object.set_engine_argument(self.ea)
            self.operation_object.set_fetch_store_equipment(self.fse)


    def prepare_extend_ip(self):
        if (self.ea.get_extend_ip()):
            self.operation_object=extend_ip()
            self.operation_object.set_engine_argument(self.ea)

            if (self.ea.get_first_ip()):
                self.operation_object.set_firstip(self.ea.get_first_ip())

            if (self.ea.get_saferange()):
                self.operation_object.set_saferange()

            if (self.ea.get_last_ip()):
                self.operation_object.set_lastip(self.ea.get_last_ip())


    def prepare_clean_ip(self):
        if (self.ea.get_clean_ip()):
            self.operation_object=clean_ip()
            self.operation_object.set_engine_argument(self.ea)

            if (self.ea.get_first_ip()):
                self.operation_object.set_firstip(self.ea.get_first_ip())

            if (self.ea.get_saferange()):
                self.operation_object.set_saferange()

            if (self.ea.get_last_ip()):
                self.operation_object.set_lastip(self.ea.get_last_ip())


    def prepare_change_network(self):
        if (self.ea.get_change_network()):  # Change node port to a different network
            self.operation_object=move_network_interface()


    def prepare_create_logical_port(self):
        if (self.ea.get_create_logical_port()): # create 'logical' node port in a different network
            self.operation_object=create_logical_network_interface()


    def prepare_create_logical_network(self):
        if (self.ea.get_create_logical_network()):
            self.operation_object=create_logical_network()


    def prepare_remove_logical_network(self):
        if (self.ea.get_remove_logical_network()):
            self.operation_object=remove_logical_network()


    def prepare_create_range(self):
        if (self.ea.get_create_range()):
            self.operation_object=create_range()


    def prepare_remove_range(self):
        if (self.ea.get_remove_range()):
            self.operation_object=remove_range()


    def prepare_create_member(self):
        if (self.ea.get_create_member()):
            self.operation_object=create_member()


    def prepare_remove_member(self):
        if (self.ea.get_remove_member()):
            self.operation_object=remove_member()


    def prepare_alloc_object(self):
        self.fse=fetch_store_equipment()
        self.fse.set_engine_argument(self.ea)
        self.prepare_extend_ip()
        self.prepare_clean_ip()
        self.prepare_change_network()
        self.prepare_create_logical_port()
        self.prepare_create_logical_network()
        self.prepare_remove_logical_network()
        self.prepare_create_range()
        self.prepare_remove_range()
        self.prepare_create_member()
        self.prepare_remove_member()
        self.prepare_engine_and_fetch_store()
        

    def list_equipment(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()
        self.fse.check_input_validity()
        self.fse.check_input_network()
        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()
        self.fse.print_interface_collected()
        self.fse.list_equipment()


    def list_network(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()
        self.fse.list_network()


    def create_logical_network(self):
        self.fse.fetch_init(self.cdb_glue)        
        self.fse.fetch_network_list()
        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():
            self.fse.commit()
        else:
            return False


    def remove_logical_network(self):
        self.fse.fetch_init(self.cdb_glue)        
        self.fse.fetch_network_list()
        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():            
            self.fse.commit()            
        else:
            return False


    def create_range(self):
        self.fse.fetch_init(self.cdb_glue)        
        self.fse.fetch_network_list()
        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():
            self.fse.commit()
        else:
            return False


    def remove_range(self):
        self.fse.fetch_init(self.cdb_glue)        
        self.fse.fetch_network_list()
        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():            
            self.fse.commit()
        else:
            return False


    def create_member(self):
        self.fse.fetch_init(self.cdb_glue)        
        self.fse.fetch_network_list()
        self.fse.check_input_validity()
        self.fse.check_input_network()
        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()

        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():
            self.fse.commit()
        else:
            return False


    def remove_member(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()
        self.fse.check_input_validity()
        self.fse.check_input_network()
        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()
        self.fse.print_interface_collected()
        #self.fse.list_equipment()

        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        if self.operation_object.do_it():
            self.fse.commit()
        else:
            return False

    def move_network_interface(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.net_name=self.ea.get_network_source_name()
        self.fse.fetch_network_list()
        self.fse.check_input_validity()
        self.fse.check_input_network()
        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()
        self.fse.print_interface_collected()        

        self.operation_object.set_fetch_store_equipment(self.fse)
        self.operation_object.set_engine_argument(self.ea)
        self.operation_object.set_name_of_the_network_source(self.ea.get_network_source_name())
        self.operation_object.set_name_of_the_network_destination(self.ea.get_network_dest_name())
        if self.operation_object.do_it():
            self.fse.commit()
        else:
            return False


    def extend_ip(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()

        if (self.fse.check_input_validity()==False or
            self.fse.check_input_network()==False):
	    print("==> input or network invalid")
            sys.exit(1)
            
        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()

        if (self.ea.get_verbose()):
            print("**Equipment fetched from the DB**")
            self.fse.list_equipment()

        ok=self.operation_object.do_it()

        self.fse.print_interface_collected()
        if (self.ea.get_dryrun()!=True and ok):
            self.fse.commit()

        if (self.ea.get_verbose()):
            print("**Equipment commited to the DB**")
            self.fse.list_equipment()


    def update_network(self):

        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()

        if (self.fse.check_input_validity()==False or
            self.fse.check_input_network()==False):
	    print("==> input or network invalid")
            sys.exit(1)

        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()

        if (self.v):
            print("**Equipment fetched from the DB**")
            self.fse.list()

        if (self.alloc.update()):
            if (self.v):
                print("**Equipment updated in memory**")
                self.fse.list()
        else:
            print("**Error in the updating procedure**")
            sys.exit(1)
        self.fse.print_interface_collected()
        self.fse.commit(self.cdb_glue)

#        if (self.v):
#            print("**Equipment commited to the DB**")
#            self.fse.list()


    def create_logical_network_interface(self):

        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()
        self.fse.net_name=self.ea.get_network_source_name()
        self.operation_object.set_name_of_the_network_source(self.ea.get_network_source_name())
        self.operation_object.set_name_of_the_network_destination(self.ea.get_network_dest_name())

        if (self.fse.check_input_validity()==False or
            self.fse.check_input_network()==False):
            print("==> input or network invalid")
            sys.exit(1)

        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()

        if (self.ea.get_verbose()):
            print("**Equipment fetched from the DB**")
            self.fse.list_equipment()

        if (self.operation_object.do_it()):
            if (self.ea.get_verbose()):
                print("**Equipment updated in memory**")
                self.fse.list_equipment()
        else:
            print("ERROR: can't update the database")
            sys.exit(1)

        self.fse.print_interface_collected()
        self.fse.commit()

#        if (self.v):
#            print "**Equipment commited to the DB**"
#            self.fse.list()


    def clean_ip(self):
        self.fse.fetch_init(self.cdb_glue)
        self.fse.fetch_network_list()

        if (self.fse.check_input_validity()==False or
            self.fse.check_input_network()==False):
            print("==> input or network invalid")
            sys.exit(1)

        self.fse.fetch_equipment_name()
        self.fse.fetch_equipment()
        self.fse.fetch_equipment_port()
        self.fse.print_interface_collected()

        if (self.operation_object.do_it()):
            if (self.ea.get_verbose()):
                self.fse.list_equipment()
        else:
            print("Error in the cleaning procedure")
            return False

        self.fse.print_interface_collected()
        if (self.ea.get_dryrun()!=None):
            self.fse.commit()

        if (self.ea.get_verbose()):
            self.fse.list_equipment()

    def full_clean(self):
	allocxml = alloc_xml()
	allocxml.cdb = self.cdb_glue
	allocxml.cdb.fetch_dbversion()
	allocxml.clean_network_map()

    def apply_xml(self):
	allocxml = alloc_xml()
	allocxml.xml_file = self.ea.get_apply_xml()
	allocxml.saferange = self.ea.get_saferange()
	allocxml.cdb = self.cdb_glue
	allocxml.cdb = self.cdb_glue
	allocxml.apply_network_map()

