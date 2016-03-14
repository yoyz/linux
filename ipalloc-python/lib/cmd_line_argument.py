import sys,getopt,re,os


class engine_argument:

    def __init__(self):
                                     #
                                     # Argument to be executed
                                     #
#############################################################
        self.v=False                 # verbose      not mandatory
        self.help=False              # help         not mandatory
        self.full_help=False         # full help    not mandatory
        self.d=False                 # dryrun       not mandatory
        self.u=False                 # update or update-ip
        self.cn=False                # change-network 
        self.clp=False               # create-logical-port
        self.cln=False               # create-logical-network
        self.rln=False               # remove-logical-network
        self.le=False                # list-equipment
        self.ln=False                # list-network
        self.clean_ip=False          # clean-ip
        self.extend_ip=False         # extend-ip
        self.create_range=False      # create-range
        self.remove_range=False      # remove-range
        self.create_member=False     # create-member
        self.remove_member=False     # remove-member
        self.apply_xml=False         # apply an xml file to the database
                                     #
##############################################################
                                     #
                                     # Parameter for argument
                                     # 
        self.nw_cidr=None            # subnet cidr ex : 10.16.0.0/16
        self.net_type=None           # network type   : primary or secondary
        self.net_function=None       # network function


        self.net_src_name=None       # network name src
        self.net_dst_name=None       # network name dst
        self.net_parent_name=None    # network name parent
        self.net_name=None           # network name 
        self.net_suffix=None         # network suffix
        self.vpartition=None         # vpartition : pkey vlan
        self.fi=None                 # firstip     mandatory !!!
        self.li=None                 # lastip   not mandatory
        self.gateway=None            # gateway  not mandatory
                                     #
#        self.rg=None                 # regex for the equipment name   not mandatory
                                     #
        self.saferange=False         # Saferange for ip like A.B.C.D with D!=[0,254,255]
                                     #
        self.object_type=None        # node      hwmamanger
        self.object_subtype=None     # compute   cmc,bmc
        self.object_hwgroup=None     # islet,COMPUTE_1,SERVICE_M
        self.object_iname=None       # interface name
        self.object_regex=None         # if (None): => don't use regex for equipment name matching

        self.subnet=None             # network cidr mandatory or


        self.hba_rank=None           # hba_rank  should not be used anymore
        self.port_rank=None          # port_rank should not be used anymore

##############################################################
        
        self.dbu=None
        self.dbp=None
        self.dbn=None
        self.dbh=None

##############################################################
                                     #
                                     # ObjectSQL 
                                     #
        self.hgs=None                #
        self.ms=None                 # member_sql
        self.ns=None                 # node_sql
        self.hs=None                 # hwmanager_sql
        self.ds=None                 # disk_array_sql
        self.dps=None                # disk_array_port_sql
        self.hps=None                # hwmanager_port_sql
        self.nps=None                # node_port_sql
        self.nws=None                # network_sql
        self.vps=None                # metaservice_port_sql

        self.regex_cidr=re.compile("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/\d{1,2}")
        self.regex_ipv4=re.compile("\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}")

##############################################################


    def set_verbose(self,value):
        self.v=value

    def set_help(self,value):
        self.help=value

    def set_full_help(self,value):
        self.full_help=value

    def set_dryrun(self,value):
        self.d=value


    def set_apply_xml(self,value):
        self.apply_xml=value


    def set_update(self,value):
        self.u=value

    def set_update_ip(self,value):
        self.u=value

    def set_extend_ip(self,value):
        self.extend_ip=value

    def set_clean_ip(self,value):
        self.clean_ip=value

    def set_change_network(self,value):
        self.cn=value

    def set_create_logical_port(self,value):
        self.clp=value

    def set_create_logical_network(self,value):
        self.cln=value

    def set_remove_logical_network(self,value):
        self.rln=value

    def set_create_range(self,value):
        self.create_range=value

    def set_remove_range(self,value):
        self.remove_range=value

    def set_create_member(self,value):
        self.create_member=value

    def set_remove_member(self,value):
        self.remove_member=value

    def set_list_equipment(self,value):
        self.le=value

    def set_list_network(self,value):
        self.ln=value

    def set_hba_rank(self,value):
        self.hba_rank=value

    def set_port_rank(self,value):
        self.port_rank=value

    def set_subnet(self,value):
        if self.regex_cidr.match(value):
            self.nw_cidr=value
	    print value
        else:
            print "Error setting subnet\nset_subnet("+str(value)+")"+" CIDR example : 10.5.0.0/16"
            sys.exit(2)

    def set_network_type(self,value):
        self.net_type=value

    def set_network_function(self,value):
        self.net_function=value

    def set_network_source_name(self,value):
        self.net_src_name=value

    def set_network_dest_name(self,value):
        self.net_dst_name=value

    def set_network_parent_name(self,value):
        self.net_parent_name=value

    def set_network_name(self,value):
        self.net_name=value

    def set_first_ip(self,value):
        if self.regex_ipv4.match(value):
            self.fi=value
        else:
            print "Error setting first_ip \nset_first_ip("+str(value)+")"
            sys.exit(2)
            

    def set_last_ip(self,value):
        if self.regex_ipv4.match(value):
            self.li=value
        else:
            print "Error setting last_ip \nset_last_ip("+str(value)+")"
            sys.exit(2)

    def set_gateway(self,value):
        if self.regex_ipv4.match(value):
            self.gateway=value
        else:
            print "Error setting gateway \nset_gateway("+str(value)+")"
            sys.exit(2)

    def set_saferange(self,value):
        self.saferange=value

    def set_object_type(self,value):
        self.object_type=value

    def set_object_subtype(self,value):
        self.object_subtype=value

    def set_object_hwgroup(self,value):
        self.object_hwgroup=value

    def set_object_iname(self,value):
        self.object_iname=value

    def set_object_regex(self,value):
        self.object_regex=value

    def set_suffix(self,value):
        self.net_suffix=value

    def set_vpartition(self,value):
        self.vpartition=value

    def set_database_username(self,value):
        self.dbu=value

    def set_database_hostname(self,value):
        self.dbh=value

    def set_database_name(self,value):
        self.dbn=value

    def set_database_password(self,value):
        self.dbp=value
    

###########################################################################################################

    
    def get_verbose(self):
        return self.v

    def get_help(self):
        return self.help

    def get_full_help(self):
        return self.full_help

    def get_dryrun(self):
        return self.d

    def get_apply_xml(self):
        return self.apply_xml


    def get_update_ip(self):
        return self.u

    def get_clean_ip(self):
        return self.clean_ip

    def get_extend_ip(self):
        return self.extend_ip

    def get_change_network(self):
        return self.cn

    def get_create_logical_network(self):
        return self.cln

    def get_remove_logical_network(self):
        return self.rln

    def get_create_logical_port(self):
        return self.clp

    def get_create_range(self):
        return self.create_range

    def get_remove_range(self):
        return self.remove_range

    def get_create_member(self):
        return self.create_member

    def get_remove_member(self):
        return self.remove_member

    def get_list_equipment(self):
        return self.le

    def get_list_network(self):
        return self.ln

    def get_hba_rank(self):
        return self.hba_rank

    def get_port_rank(self):
        return self.port_rank

    def get_subnet(self):
        return self.nw_cidr

    def get_suffix(self):
        return self.net_suffix

    def get_vpartition(self):
        return self.vpartition

    def get_network_type(self):
        return self.net_type

    def get_network_function(self):
        return self.net_function

    def get_network_source_name(self):
        return self.net_src_name

    def get_network_dest_name(self):
        return self.net_dst_name

    def get_network_parent_name(self):
        return self.net_parent_name

    def get_network_name(self):
        return self.net_name

    def get_first_ip(self):
        return self.fi

    def get_last_ip(self):
        return self.li

    def get_gateway(self):
        return self.gateway

    def get_saferange(self):
        return self.saferange

    def get_object_type(self):
        return self.object_type

    def get_object_subtype(self):
        return self.object_subtype

    def get_object_hwgroup(self):
        return self.object_hwgroup

    def get_object_regex(self):
        return self.object_regex

    def get_object_iname(self):
        return self.object_iname

    def get_database_username(self):
        return self.dbu

    def get_database_hostname(self):
        return self.dbh

    def get_database_name(self):
        return self.dbn

    def get_database_password(self):
        return self.dbp


    def print_string(self):
        print "---[engine_agument]---"
        print "get_verbose()                   : "+str(self.get_verbose())
        print "get_help()                      : "+str(self.get_help())
        print "get_full_help()                 : "+str(self.get_full_help())
        print "get_dryrun()                    : "+str(self.get_dryrun())
        print "get_update_ip()                 : "+str(self.get_update_ip())
        print "get_clean_ip()                  : "+str(self.get_clean_ip())
        print "get_extend_ip()                 : "+str(self.get_extend_ip())
        print "get_change_network()            : "+str(self.get_change_network())
        print "get_create_logical_network()    : "+str(self.get_create_logical_network())
        print "get_remove_logical_network()    : "+str(self.get_remove_logical_network())
        print "get_create_range()              : "+str(self.get_create_range())
        print "get_remove_range()              : "+str(self.get_remove_range())
        print "get_create_logical_port()       : "+str(self.get_create_logical_port())
        print "get_create_range()              : "+str(self.get_create_range())
        print "get_remove_range()              : "+str(self.get_remove_range())
        print "get_list_equipment()            : "+str(self.get_list_equipment())
        print "get_list_network()              : "+str(self.get_list_network())
        print "get_hba_rank()                  : "+str(self.get_hba_rank())
        print "get_port_rank()                 : "+str(self.get_port_rank())
        print "get_subnet()                    : "+str(self.get_subnet())
        print "get_suffix()                    : "+str(self.get_suffix())
        print "get_vpartition()                : "+str(self.get_vpartition())
        print "get_network_type()              : "+str(self.get_network_type())
        print "get_network_function()          : "+str(self.get_network_function())
        print "get_network_source_name()       : "+str(self.get_network_source_name())
        print "get_network_dest_name()         : "+str(self.get_network_dest_name())
        print "get_network_parent_name()       : "+str(self.get_network_parent_name())
        print "get_network_name()              : "+str(self.get_network_name())
        print "get_first_ip()                  : "+str(self.get_first_ip())
        print "get_last_ip()                   : "+str(self.get_last_ip())
        print "get_gateway()                   : "+str(self.get_gateway())
        print "get_saferange()                 : "+str(self.get_saferange())
        print "get_object_type()               : "+str(self.get_object_type())
        print "get_object_subtype()            : "+str(self.get_object_subtype())
        print "get_object_hwgroup()            : "+str(self.get_object_hwgroup())
        print "get_object_regex()              : "+str(self.get_object_regex())
        print "get_object_iname()              : "+str(self.get_object_iname())
        print "get_database_username()         : "+str(self.get_database_username())
        print "get_database_hostname()         : "+str(self.get_database_hostname())
        print "get_database_name()             : "+str(self.get_database_name())
        print "get_database_password()         : "+str(self.get_database_password())

        
        


##################################################################################################    


class command_line_argument:

    def __init__(self):
        self.eg=None

    def get_engine_argument(self):
        return self.eg

    def set_engine_argument(self,value):
        self.eg=value

    def parse_command_line(self):
        self.eg=engine_argument()
        try:
            options, remainder = getopt.getopt(sys.argv[1:], '',
                                               ['dbname=',
                                                'dbuser=',
                                                'dbpasswd=',
                                                'dbhost=',
                                                'hwgroup=',
                                                'obj=',
                                                'object=',
                                                'type=',
                                                'iname=',
                                                'interface-name=',
                                                'subnet=',
                                                'function=',
                                                'net-name=',
                                                'net-type=',
                                                'suffix=',
                                                'src-net-name=',
                                                'dst-net-name=',
                                                'parent-net-name=',
                                                'first-ip=',
                                                'last-ip=',
                                                'gateway=',
                                                'vpartition=',
                                                'hba-rank=',
                                                'port-rank=',
                                                'regex=',
                                                'verbose',
                                                'dryrun',
                                                'saferange',
                                                'list-network',
                                                'list-equipment',
                                                'update',
                                                'update-ip',
                                                'clean',
                                                'clean-ip',
                                                'extend',
                                                'extend-ip',
                                                'change-network',
                                                'create-logical-port',
                                                'remove-logical-network',
                                                'create-logical-network',
                                                'remove-logical-network',
                                                'create-range',
                                                'remove-range',
                                                'create-member',
                                                'remove-member',                                                
						'apply=', 
                                                'help',                          
                                                'full-help'                          
                                                ])
        except getopt.GetoptError,e:
            print ""
            print "**Wrong options in command line**"
            print "**"+str(e)+"**"
            print ""
            sys.exit(1)
            
            
        for opt, arg in options:
            # Parse Flags

            if opt in ('h','--help'):
                self.eg.set_help(True)

            if opt in ('--full-help'):
                self.eg.set_full_help(True)

            if opt in ('v','--verbose'):
                self.eg.set_verbose(True)

            if opt in ('d','--dryrun'):
                self.eg.set_dryrun(True)

            if opt in ('u','--update'):
                self.eg.set_extend_ip(True)

            if opt in ('u','--update-ip'):
                self.eg.set_extend_ip(True)

            if opt in ('e','--extend'):
                self.eg.set_extend_ip(True)

            if opt in ('e','--extend-ip'):
                self.eg.set_extend_ip(True)

            if opt in ('--change-network'):
                self.eg.set_change_network(True)

            if opt in ('--create-logical-network'):
                self.eg.set_create_logical_network(True)

            if opt in ('--remove-logical-network'):
                self.eg.set_remove_logical_network(True)

            if opt in ('--create-logical-port'):
                self.eg.set_create_logical_port(True)

            if opt in ('--create-range'):
                self.eg.set_create_range(True)

            if opt in ('--remove-range'):
                self.eg.set_remove_range(True)

            if opt in ('--create-member'):
                self.eg.set_create_member(True)

            if opt in ('--remove-member'):
                self.eg.set_remove_member(True)

            if opt in ('c','--clean-ip'):
                self.eg.set_clean_ip(True)

            if opt in ('c','--clean'):
                self.eg.set_clean_ip(True)

            if opt in ('--list-network'):
                self.eg.set_list_network(True)

            if opt in ('--list-equipment'):
                self.eg.set_list_equipment(True)

            if opt in ('--saferange'):
                self.eg.set_saferange(True)

        # Parse Options
            if opt == '--obj':
                self.eg.set_object_type(arg)

            if opt == '--object':
                self.eg.set_object_type(arg)

            if opt == '--hwgroup':

                self.eg.set_object_hwgroup(arg)
            if opt == '--type':
                self.eg.set_object_subtype(arg)

            if opt == '--interface-name':
                self.eg.set_object_iname(arg)

            if opt == '--iname':
                self.eg.set_object_iname(arg)

            if opt == '--hba-rank':
                self.eg.set_hba_rank(arg)

            if opt == '--port-rank':
                self.eg.set_port_rank(arg)

            if opt == '--regex':
                self.eg.set_object_regex(arg)

            if opt == '--subnet':
                self.eg.set_subnet(arg)

            if opt == '--net-name':
                self.eg.set_network_name(arg)

            if opt == '--net-type':
                self.eg.set_network_type(arg)

            if opt == '--suffix':
                self.eg.set_suffix(arg)

            if opt == '--src-net-name':
                self.eg.set_network_source_name(arg)

            if opt == '--dst-net-name':
                self.eg.set_network_dest_name(arg)

            if opt == '--parent-net-name':
                self.eg.set_network_parent_name(arg)

            if opt == '--function':
                self.eg.set_network_function(arg)

            if opt == '--first-ip':
                self.eg.set_first_ip(arg)

            if opt == '--last-ip':
                self.eg.set_last_ip(arg)

            if opt == '--gateway':
                self.eg.set_gateway(arg)

            if opt == '--vpartition':
                self.eg.set_vpartition(arg)

            if opt == '--dbuser':
                self.eg.set_database_username(arg)

            if opt == '--dbpasswd':
                self.eg.set_database_password(arg)

            if opt == '--dbname':
                self.eg.set_database_name(arg)

            if opt == '--dbhost':
                self.eg.set_database_hostname(arg)

	    if opt == '--apply':
		self.eg.set_apply_xml(arg)

        if (self.eg.get_verbose()):
            print "Opt "+str(options)
            print "Rem "+str(remainder)
    
