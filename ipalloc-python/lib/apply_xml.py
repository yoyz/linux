from   CLM.DB.ipallocation.clusterdb_glue			import clusterdb_glue
from CLM.DB.ipallocation.clusterdb_sql_request		import network_sql,interface_member_sql,ftype_sql,node_profile_device_sql,node_profile_sql,member_sql,hw_group_sql,node_sql,hwmanager_sql,switch_sql,disk_array_sql,node_port_sql,hwmanager_port_sql,switch_port_sql,disk_array_port_sql,metaservice_port_sql
from CLM.DB.ipallocation.handle_xml_ipallocation		import handle_xml_ipallocation
from CLM.DB.ipallocation.allocation_rule			import network_allocation_rule
import misc

import sys,re,os
from subprocess import *


class alloc_xml:

    def __init__(self):
        self.cdb=None # clusterdb_sqlalchemy handler
        #             # Flags
        self.h=False  # Help
        self.v=False  # verbose not mandatory
        self.d=False  # dryrun  not mandatory
        self.xml_file=None
        self.apply=False   # apply  network map
        self.clean=False   # clean network map
        self.copydb=False
        self.saferange=False
        self.spath="/tmp/ipallocation.sh"
	self.log = []

        self.dbn_src="clusterdb"
        self.dbu="clusterdb"
        self.dbp="clusterdb"
        self.dbh="127.0.0.1"

        self.dbn_dst=""


    def connect_db(self):
        cdb=clusterdb_alchemy()
        cdb.set_user(self.dbu)
        cdb.set_passwd(self.dbp)
        cdb.set_host(self.dbh)
        cdb.set_name(self.dbn_src)
        cdb.connect()
        self.cdb=cdb


    def copy_database(self):
        md=manage_db_instance()
        md.set_dbnamesrc(self.dbn_src)
        md.set_dbnamedst(self.dbn_dst)
        md.set_dbuser(self.dbu)
        md.set_dbpasswd(self.dbp)
        md.set_dbhost(self.dbh)


        if md.copy()==True:

            print("copydb OK")
        else:
            print("copydb Failed")
        sys.exit(0)

    

    def check_command_line(self):
        ok=True
        if (self.apply==False and
            self.clean==False and
	    self.copydb==False):
            self.usage()
            sys.exit(0)

        if (self.apply==True and
            len(self.xml_file)>0):
            if (os.path.exists(self.xml_file)==False):
                print("File not found: %s" % (self.xml_file))
                self.usage()
                sys.exit(1)

        if (self.copydb==True and
            len(self.dbn_dst)==0):
            self.usage()
            print("--dbsrc <dbnamesrc> --dbdst <dbnamedst>")
            print("You must provide this two parameters")
            sys.exit(1)

        return ok


    def clean_network_map(self):
        ns=network_sql(self.cdb)
        nps=node_port_sql(self.cdb)
        hps=hwmanager_port_sql(self.cdb)
        das=disk_array_port_sql(self.cdb)
        sms=switch_port_sql(self.cdb)
        msp=metaservice_port_sql(self.cdb)

        nps.delete_logical_node_port()
	sms.delete_logical_switch_port()
        
        nwl=ns.fetch_all_network();
        for nw in nwl.get_all_network():
            if nw.network_id!=None:
                print nw
                nps.change_node_port_network_id(nw.id,nw.network_id)
                hps.change_hwmanager_port_network_id(nw.id,nw.network_id)
                das.change_disk_array_port_network_id(nw.id,nw.network_id)
                sms.change_switch_port_network_id(nw.id,nw.network_id)
                msp.change_metaservice_port_network_id(nw.id,nw.network_id)

        ns.delete_range_network()
        ns.delete_son_network()
        ns.clean_network_table()

        nwl=ns.fetch_all_network();
        for nw in nwl.get_all_network():
            nps.clean_ipaddr(nw.id)
            hps.clean_ipaddr(nw.id)
            das.clean_ipaddr(nw.id)
            sms.clean_ipaddr(nw.id)
            msp.clean_ipaddr(nw.id)

        try:
            self.cdb.session.commit()
	    print("Done")
        except:
            print("I am very sorry, unexpected error commiting transaction")
            sys.exit(1)
	misc.delete_logfile()


    def apply_network_map(self):
	#print "DB: name=%s,user=%s,password=%s" % (self.cdb.get_name(), self.cdb.get_user(), self.cdb.get_passwd())
            
        ns=network_sql(self.cdb)

        if self.v:
            print ns.fetch_all_network()
            
        xml_handle=handle_xml_ipallocation()
        
        #print "filename:"
        #print self.xml_file
        #sys.exit(0)

        xml_handle.set_filename(self.xml_file)
        xml_handle.load_xml()
        xml_handle.check_rules()
        rules=xml_handle.get_rules()
        rules.sort_logical_port()
        rules.sort_logical_port_name()
        rules.sort_first_ip()

        #print rules
	#print xml_handle.get_rules()
        #sys.exit(0)
        if self.v:
            print(xml_handle.get_rules())
            
            
        i=0
        narl=[]
        print("Number of rules to apply : "+str(rules.number_of_rule()))
        print("Please wait...")
        for r in rules.get_rules():
            nar=network_allocation_rule(self.cdb)
            nar.set_verbose(self.v)
            nar.refresh_network_list_from_db()
            nar.set_rule(r)
            check_net=nar.check_networks()
            if self.v:
                print("")
                print("")
                print("")
                print("")
                print("")
                print("*Rule to apply")
                print(r)
                print("*Check_network result : "+str(check_net))

	    # Extra check on network name size...
	    if len(r.get_network().get_name()) > 32 and len(r.get_logical_network().get_name()) > 32:
		print("WARNING: your network name can't exceed 32 chars.")

            if check_net==False:
                try:
                    #nar.update_networks()
		    nar.update_logical_network()
		    nar.update_range()
		    nar.update_member()
                    self.cdb.session.commit()
                    narl.append(nar)
                except:
                    print("I am very sorry, unexpected error commiting transaction")
                    print(r)
                    sys.exit(1)

        print("Opening script file : "+self.spath)

	try:
	        script_file = open(self.spath, 'w')
	except:
		print("ERROR: can't write to the intermediary shell script.")
		sys.exit(1)

        script_file.write("#!/bin/sh\n")
        script_file.write("export PGPASSWORD="+self.cdb.get_passwd()+"\n")
        script_file.write("export PGUSER="+self.cdb.get_user()+"\n")
        script_file.write("psql -U "+self.cdb.get_user()+" "+self.cdb.get_name()+" -c 'ALTER TABLE node_port DROP CONSTRAINT u_node_port_switch_id_slot_port' 2>/dev/null\n")

        for r in rules.get_rules():
            str_cmd="/usr/lib/clustmngt/clusterdb/bin/ipallocation"

            if (r.get_network().get_name()!=r.get_logical_network().get_name() and 
                r.get_logical_network().get_type()=="primary"):
                str_cmd_line=str_cmd
                str_cmd_line=str_cmd_line+" --dbname "+self.cdb.get_name()
                str_cmd_line=str_cmd_line+" --change-network"
                str_cmd_line=str_cmd_line+" --src-net-name '"+r.get_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --dst-net-name '"+r.get_logical_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --object '"+r.get_member().get_object()+"'"
                str_cmd_line=str_cmd_line+" --type '"+r.get_member().get_type()+"'"
                str_cmd_line=str_cmd_line+" --hwgroup '"+r.get_member().get_group()+"'"
                str_cmd_line=str_cmd_line+" --iname '"+r.get_member().get_iname()+"'"

                #print str_cmd_line
                script_file.write(str_cmd_line+"\n")

                str_cmd_line=str_cmd
                str_cmd_line=str_cmd_line+" --dbname "+self.cdb.get_name()
                str_cmd_line=str_cmd_line+" --extend"
                str_cmd_line=str_cmd_line+" --net-name '"+r.get_logical_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --object '"+r.get_member().get_object()+"'"
                str_cmd_line=str_cmd_line+" --type '"+r.get_member().get_type()+"'"
                str_cmd_line=str_cmd_line+" --hwgroup '"+r.get_member().get_group()+"'"
                str_cmd_line=str_cmd_line+" --iname '"+r.get_member().get_iname()+"'"
                str_cmd_line=str_cmd_line+" --first-ip '"+r.get_range().get_first_ip()+"'"
                if self.saferange:
                    str_cmd_line=str_cmd_line+" --saferange "

                #print str_cmd_line
                script_file.write(str_cmd_line+"\n")

            if (r.get_network().get_name()!=r.get_logical_network().get_name() and 
                r.get_logical_network().get_type()=="secondary"):
                str_cmd_line=str_cmd
                str_cmd_line=str_cmd_line+" --dbname "+self.cdb.get_name()
                str_cmd_line=str_cmd_line+" --create-logical-port"
                str_cmd_line=str_cmd_line+" --src-net-name '"+r.get_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --dst-net-name '"+r.get_logical_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --object '"+r.get_member().get_object()+"'"
                str_cmd_line=str_cmd_line+" --type '"+r.get_member().get_type()+"'"
                str_cmd_line=str_cmd_line+" --hwgroup '"+r.get_member().get_group()+"'"
                str_cmd_line=str_cmd_line+" --iname '"+r.get_member().get_iname()+"'"

                #print str_cmd_line
                script_file.write(str_cmd_line+"\n")

                str_cmd_line=str_cmd
                str_cmd_line=str_cmd_line+" --dbname "+self.cdb.get_name()
                str_cmd_line=str_cmd_line+" --extend"
                str_cmd_line=str_cmd_line+" --net-name '"+r.get_logical_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --object '"+r.get_member().get_object()+"'"
                str_cmd_line=str_cmd_line+" --type '"+r.get_member().get_type()+"'"
                str_cmd_line=str_cmd_line+" --hwgroup '"+r.get_member().get_group()+"'"
                str_cmd_line=str_cmd_line+" --iname '"+r.get_member().get_iname()+"'"
                str_cmd_line=str_cmd_line+" --first-ip '"+r.get_range().get_first_ip()+"'"
                if self.saferange:
                    str_cmd_line=str_cmd_line+" --saferange "

                #print str_cmd_line
                script_file.write(str_cmd_line+"\n")


            if (r.get_network().get_name()==r.get_logical_network().get_name()):
                str_cmd_line=str_cmd
                str_cmd_line=str_cmd_line+" --dbname "+self.cdb.get_name()
                str_cmd_line=str_cmd_line+" --extend"
                str_cmd_line=str_cmd_line+" --net-name '"+r.get_network().get_name()+"'"
                str_cmd_line=str_cmd_line+" --object '"+r.get_member().get_object()+"'"
                str_cmd_line=str_cmd_line+" --type '"+r.get_member().get_type()+"'"
                str_cmd_line=str_cmd_line+" --hwgroup '"+r.get_member().get_group()+"'"
                str_cmd_line=str_cmd_line+" --iname '"+r.get_member().get_iname()+"'"
                str_cmd_line=str_cmd_line+" --first-ip '"+r.get_range().get_first_ip()+"'"
                if self.saferange:
                    str_cmd_line=str_cmd_line+" --saferange "

                script_file.write(str_cmd_line+"\n")
                #print str_cmd_line

	# print the log file if it exists
	script_file.write("[ -s /tmp/ipallocation.log ] && echo && cat /tmp/ipallocation.log\n")

        print("Closing script file : "+self.spath)
        script_file.close()

        print("")
        print("Now you can execute the script : ")
        print("")
        print("/bin/sh "+self.spath)
        print("")

	try:
		pipe = Popen("date +%F-%H%M%S", shell=True, stdout=PIPE, stderr=PIPE)
		out = pipe.stdout.read()
		if len(out):
			isodate = out.rstrip()
	except:
		isodate = ""
	self.log.append("-> [%s] Network map applied: %s\n" % (isodate, self.xml_file))
	misc.logfile(self.log)
	del self.log[:]

        #if nar.new_network_created():
        	#print "**new_parent_network       : "+str(nar.get_new_parent_network_id())
                #print "**new_created_network      : "+str(nar.get_new_network_id())
                #print "**new_created_network type : "+str(nar.get_new_network_type())
                #print ""

