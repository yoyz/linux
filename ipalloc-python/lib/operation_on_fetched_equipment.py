from   CLM.DB.ipallocation.error_code                   import error_code
from   CLM.DB.ipallocation.clusterdb_sqlalchemy         import clusterdb_alchemy,Cluster,Network,Node,Node_port,Hwmanager,Hwmanager_port,Node_ftype,Ftype,Hw_group,Hw_group_member,Switch,Switch_port,Node_profile,Node_profile_device,Disk_array,Metaservice_port
from   CLM.DB.ipallocation.clusterdb_sql_request        import member_sql,interface_member_sql,network_sql,node_profile_device_sql,node_sql,node_port_sql,hwmanager_port_sql,hwmanager_sql,switch_sql,switch_port_sql,disk_array_sql,disk_array_port_sql,hw_group_sql,metaservice_port_sql,load_ip_from_db
from   CLM.DB.ipallocation.equipment                    import equipment_list,network_list,network,equipment,network_interface
from   CLM.DB.ipallocation.xml_element                  import networkXML,memberXML,rangeXML
from   CLM.DB.ipallocation.ip_range                     import ip_range
from   CLM.DB.ipallocation.ipaddr                       import ipaddr,ip_set,network
from   CLM.DB.ipallocation.network_range                import network_range_before_2140
from   CLM.DB.ipallocation.rule                         import rule,rule_list
from   CLM.DB.ipallocation.allocation_rule              import network_allocation_rule
import misc
import sys,getopt,re


################################################################################

class extend_ip:
    def __init__(self):
        self.firstip=None            # ipv4 mandatory
        self.lastip=None             # ipv4 not mandatory
        self.fse=None                # A fetch_store_equipment object
        self.saferange=False
        self.operation="extend_ip"
        self.msg="no message"
        self.ipr=None
        self.ipr_check_db=None
        self.ip_from_db=None
        self.found_range=None
        self.list_ip_to_deliver=None
        self.ip_to_deliver=None
        self.nw=None
        self.nws=None
        self.current_next_ip=None
        self.numberOfNetworkInterfaceToAlloc=0
	self.log = []

    def set_engine_argument(self,ea):
        self.ea=ea
	self.saferange = self.ea.get_saferange()

    def set_fetch_store_equipment(self,fse):
        self.fse=fse

    def set_firstip(self,firstip):
        self.firstip=firstip

    def set_lastip(self,lastip):
        self.lastip=lastip

    def set_saferange(self):
        self.saferange=True

    def prepare_object(self):
        """
        update the equipment port ipaddr in memory
        """
        if (self.ea.get_verbose()):
            print "-> update "+self.operation+" "+self.fse.object_type

        list_permited = ["node","hwmanager","switch","disk_array","metaservice","*"]
        r = re.compile("([a-zA-Z\*]+)\[([a-zA-Z\-1-9]+)\]") # match string 'bmc[OSS]'

        if (self.fse.object_type==None                           or
            self.fse.object_type in list_permited                or
            r.match(self.fse.object_type)
            ):
            pass
        else:
            warn_msg = "Error: the rule doesn't match, no permited object found."
	    self.msg = warn_msg
	    self.log.append(warn_msg)
	    misc.logfile(self.log)
	    del self.log[:]
            return False

        ok=True
        self.ip_from_db=load_ip_from_db(self.fse.cdb)
        self.ip_from_db.fetch()
        self.found_range=False

	# Match a range/subset to continue
        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()    == "range" and
                nw.get_next_ip() == self.firstip and
                nw.get_subset()  != None):
                for s in nw.get_subset().split(" "):
                    if (s == self.fse.get_member().get_subset()):
                        self.found_range=True
		if self.found_range == True:
			break

        if self.found_range == False:
	    warn_msg = "Error: the rule doesn't match, range or subset is not found!"
            self.msg = warn_msg
	    self.log.append(warn_msg)
	    misc.logfile(self.log)
	    del self.log[:]
            return self.found_range

	# CBNDEBUG check if useful
	current_subnet = ""
        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_name()    == self.ea.get_network_name()):
                self.current_next_ip=nw.get_next_ip()

        self.ip_to_deliver = []

        if self.saferange:
	    saferange_regex = re.compile(".*\.0$|.*\.254$|.*\.255$")
	else:
	    saferange_regex = None

        self.nw = self.fse.nwl.get_network_by_subnet(self.fse.subnet)
        self.nws = network_sql(self.fse.cdb)
        
	(_mynet, _mymask) = self.fse.subnet.split("/")
	_mynetwork = network(_mynet, _mymask)
	_netsize = _mynetwork.size() - 2
	_nextip = ipaddr(self.nw.get_next_ip())
	_computed_lastip = ipaddr(_mynetwork.ip.intrep + _netsize)
	if _nextip.intrep != None:
		_availableip = _computed_lastip.intrep - _nextip.intrep # Not used later.
	else:
		_availableip = _netsize
	if self.saferange:
		_availableip -= pow((24 - int(_mymask)), 2) * 3 # *3 => 0,254,255 for each /24
		_availableip += 2 # net and broadcast already taken into account

	if self.ea.get_verbose():
		print("Current NextIP for %s (%s): %s" \
			% (self.nw.get_name(), self.fse.subnet, self.nw.get_next_ip()))
		print("COMPUTED => lastip=%s, netsize=%s, availableip=%s" \
			% (_computed_lastip.ip_str(), _netsize, _availableip))
		print("DEFINED => firstip=%s, lastip=%s" % (self.firstip, self.lastip))

	# Find out the number of NICs to allocate
        self.numberOfNetworkInterfaceToAlloc=0
        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():
                self.numberOfNetworkInterfaceToAlloc=self.numberOfNetworkInterfaceToAlloc+1
                if (eq.get_table_name()=="node"):
                    break # ONE INTERFACE FOR NOW

	# find out the last IP (computed or user-defined?)
	_defined_lastip = ipaddr(self.lastip)
	if self.lastip and _computed_lastip.intrep >= _defined_lastip.intrep:
		_used_lastip = _defined_lastip.intrep
	else:
		# TODO computed last ip should take into account the first ip 
		# of the next range when this info is available
		_used_lastip = _computed_lastip.intrep

        # check if the network is properly defined
        if _mynet != _mynetwork.network_str():
            warn_msg = "Error: the network %s is defined with the subnet %s/%s instead of %s/%s." % (self.nw.get_name(), _mynet, _mymask, _mynetwork.network_str(), _mymask)
            self.msg = warn_msg
            self.log.append(warn_msg)
            misc.logfile(self.log)
            del self.log[:]
            return False

        # check if the firstip address is usable (not a network or a broadcast address)
        _defined_firstip = ipaddr(self.firstip)
        if self.firstip == _mynetwork.broadcast_str() or self.firstip == _mynetwork.network_str():
            warn_msg = "Error: a range has an invalid first IP: it is a network or a broadcast address! (netname: %s, range: %s)." % (self.nw.get_name(), self.firstip)
            self.msg = warn_msg
            self.log.append(warn_msg)
            misc.logfile(self.log)
            del self.log[:]
            return False

        # check if the firstip is inside the network
        if not _mynetwork.is_subset(_defined_firstip):
            warn_msg = "Error: a range has a first IP which is outside the network! (netname: %s, range: %s)." % (self.nw.get_name(), self.firstip)
            self.msg = warn_msg
            self.log.append(warn_msg)
            misc.logfile(self.log)
            del self.log[:]
            return False

	# Get other ranges which are superior in the network addressing scheme
	rng_list_first_ip = []
	for nw in self.fse.nwl.get_all_network_and_range():
		if nw.get_type() == "range" and ipaddr(nw.get_first_ip()).intrep > ipaddr(self.firstip).intrep:
			rng_list_first_ip.append(ipaddr(nw.get_first_ip()).intrep)
	rng_list_first_ip.sort()

	# List all available IP to use and check they are not in the DB
	# => container: self.ip_to_deliver
	_currentip = ipaddr(self.firstip).intrep	# needed for extend mode
	_already_exist = 0
	_warn_done = False
	while _currentip <= _used_lastip:
		_do_warn = True
		_str_ip = str(ipaddr(_currentip))
		if self.ip_from_db.already_exist(_str_ip) == False:
			if self.ea.get_verbose() and (not self.saferange or not saferange_regex.match(_str_ip)):
				print("--> saferange mode, skipping ipaddr %s" % (_str_ip))
			if (not self.saferange or not saferange_regex.match(_str_ip)) and _str_ip != _mynetwork.network_str() and _str_ip != _mynetwork.broadcast_str():
				self.ip_to_deliver.append(_str_ip)
				# Check whether this ip overlaps another range
				if not len(rng_list_first_ip):
					_do_warn = False
				elif _currentip < rng_list_first_ip[0]:
					_do_warn = False
					if self.ea.get_verbose():
						print("-> _current_ip=%s < r=%s" % (_str_ip, str(ipaddr(rng_list_first_ip[0]))))
				if _do_warn and not _warn_done:
					# just a warning, allocated anyway
					_warn_done = True
					warn_msg = "Warning: overlapping between ranges and/or not enough IP in the range: %s" % (self.firstip)
					print(warn_msg+"\nAllocating anyway...")
					#self.msg = warn_msg
					self.log.append(warn_msg)
					misc.logfile(self.log)
					del self.log[:]
		else:
			_already_exist += 1
		_currentip += 1
		if len(self.ip_to_deliver) >= self.numberOfNetworkInterfaceToAlloc:
			break

	if self.ea.get_verbose():
		print("-> ip_to_deliver=%s, already_exist=%s" % (len(self.ip_to_deliver), _already_exist))

	return True


    def update(self):
        """
        update the equipment port ipaddr in memory
        """
        i=0
	allocated = self.numberOfNetworkInterfaceToAlloc
        ok=False
        for eq in self.fse.eql.get_all():
            for net_int in eq.get_all_network_interface():
                if net_int.get_ipaddr()==None:                    
                    net_int.set_ipaddr(self.ip_to_deliver[i])
                    i=i+1
                    if i == allocated:
                        ok=True
                    if (eq.get_table_name()=="node"):
                        break # ONE INTERFACE PER NODE ONLY
		else:
			# skipping already existing IP
			allocated -= 1
			if self.ea.get_verbose():
				print("Skipping already existing address: %s" % (net_int.get_ipaddr()))

        if i > 0:
            ipdb=ipaddr(self.current_next_ip) 
            ipcl=ipaddr(self.ip_to_deliver[i-1])
	    #print("current_next_ip: %s, ip_to_deliver[i-1]: %s" % (str(ipdb), str(ipcl)))
            if ipdb.intrep < ipcl.intrep:                
                self.current_next_ip=str(ipcl)
		if ok:
		    self.nws.update_next_ip_network(self.nw.get_id(),self.current_next_ip)
		    print("-> allocated interfaces: %s" % (allocated))

        return ok


    def do_it(self):
        #iplist=load_ip(self.fse.cdb)
        #iplist.fetch_list()
        #iplist.fetch_list()
        #iplist.print_string()
        #sys.exit(0)
        if self.prepare_object()==False:
            print self.msg
            return False
        return self.update()




################################################################################


class clean_ip:
    def __init__(self):
        self.firstip=None            # ipv4 mandatory
        self.lastip=None             # ipv4 not mandatory
        self.fse=None                # A fetch_store_equipment object
        self.saferange=False
        self.operation="clean-ip"
        self.msg="no message"
        self.logical_network=None
        self.logical_network=None

    def set_engine_argument(self,ea):
        self.ea=ea


    def set_fetch_store_equipment(self,fse):
        self.fse=fse

    def set_firstip(self,firstip):
        self.firstip=firstip

    def set_lastip(self,lastip):
        self.firstip=lastip

    def set_saferange(self):
        self.saferange=True



    def prepare_object(self):
        self.rule=rule()
        self.physical_network=networkXML()
        self.logical_network=networkXML()
        self.found_physical=False
        self.found_logical=False
        self.found_range=False
        self.found_subnet=False

        for nw in self.fse.nwl.get_all_network():
            if (nw.get_name()    == self.ea.get_network_name() or
                (nw.get_subnet() == self.ea.get_subnet() and
                 nw.get_subnet() != None)):
                self.logical_network=nw
                self.found_logical=True


        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True

        
        if self.found_physical==False:
            for nw in self.fse.nwl.get_all_network():
                if nw.get_name() == self.ea.get_network_name():
                    self.physical_network=nw
                    self.found_physical=True
        


        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()    == "range" and
                nw.get_next_ip() == self.firstip and
                nw.get_subset()  != None):
                #print nw
                for s in nw.get_subset().split(" "):
                    if (s == self.fse.get_member().get_subset()):
                        self.found_range=True


        if (self.found_physical==False):
            self.physical_network=self.logical_network
            self.found_physical=True


        #print self.physical_network
        #print self.logical_network
        #print self.found_physical
        #print self.found_logical
        #print self.found_range
        #print self.found_subnet


        if self.found_range==False:
            self.msg="There is no matching member attached to this range"
            return self.found_range


        if (self.found_logical and
            self.logical_network.get_subnet()==None):          
            self.msg="There is no subnet on this logical network"
            self.found_subnet=False
        else:
            self.found_subnet=True




        if (self.found_physical==True     and 
            self.found_logical==True      and
            self.found_range==True       and
            self.found_subnet==True):
            self.prepare_ok=True
        else:
            self.prepare_ok=False



        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(None)
        self.rule.set_member(None)
        return self.prepare_ok


    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        return self.clean()

            



    def clean(self):
        """
        remove the ipaddr from the equipment port in memory
        """
        list_permited=["node","hwmanager","switch","disk_array","metaservice","*"]

        if (self.fse.verbose):
            print "**clean**"

        if (self.fse.object_type in list_permited):
            return self.clean_interface()
        else:
            return None



    def clean_interface(self):
        ok=False
        next_ip=None
        nws=network_sql(self.fse.cdb)
        ip_from_db=load_ip_from_db(self.fse.cdb)
        list_ip=[]

        first_ip=None
        last_ip=None
        next_ip=None
        find_ip_setted=False

        # Check to see if there is ip setted on interface, if there is quit this function with True
        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():
                if net.ipaddr!=None:
                    find_ip_setted=True

        if find_ip_setted==False:
            return True


        # Get the First ip corresponding to the range and subset
        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()        == "range"                              and
                nw.get_parent_name() == self.fse.get_logical_network_name()  and
                nw.get_subset()      != None):                
                for s in nw.get_subset().split(" "):
                    if (s == self.fse.get_member().get_subset()):
                        self.found_subset=True
                        first_ip=nw.get_next_ip()


        # Clean all the fetched interface ip address
        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():
                next_ip=net.ipaddr
                net.set_ipaddr(None)
                ok=True

        # Commit transaction to disable ORM caching
        # This is needed because the next_ip field need to be updated
        if ok: 
            self.fse.commit()

        # load all ip from the database
        ip_from_db.fetch()

        ipr_check_db=ip_range()
        ipr_check_db.setNetworkIPAddress(self.logical_network.get_subnet())
        ipr_check_db.setFirstAddress(first_ip)
        ipr_check_db.setLastAddress(self.logical_network.get_next_ip())

        while ipr_check_db.nextIpIsAvailable():
            list_ip.insert(0,ipr_check_db.getNextIp())
                    


        # Check if the ip doesn't exist in the range if yes, decrease next_ip by one
        for ip in list_ip:
            #print ip
            if ip_from_db.already_exist(ip)==False:
                nws.update_next_ip_network(self.logical_network.get_id(),ip)
                ok=True
            else:
                ok=True
                break

        return ok


################################################################################


class move_network_interface:
    def __init__(self):
        self.net_name_src=None             # a network name from network table
        self.net_name_dst=None             # another network name from network table
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="move_network_interface"
        self.msg="no message"

    def set_engine_argument(self,ea):
        self.ea=ea



    def set_fetch_store_equipment(self,fse):
        self.fse=fse

    def set_name_of_the_network_source(self,net_name_src):
        self.net_name_src=net_name_src

    def set_name_of_the_network_destination(self,net_name_dst):
        self.net_name_dst=net_name_dst


    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        return self.update()



    def update(self):
        ok=True
        nwl=self.fse.nwl
        id_nw_src=-1
        id_nw_dst=-1
#        print self.net_name_src
#        print self.net_name_dst
        for nw in self.fse.nwl.get_all_network():
            if nw.get_name()==self.net_name_src:
                id_nw_src=nw.get_id()
            if nw.get_name()==self.net_name_dst:
                id_nw_dst=nw.get_id()

        if (id_nw_src==-1 or
            id_nw_dst==-1):
            print "==> can not find network source or destination"
            return False
        
        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():                
                net.network_name=self.net_name_dst
                net.network_id=id_nw_dst
                ok=True

        return ok





    def prepare_object(self):
        """
        update the equipment port ipaddr in memory
        """

        if (self.fse.verbose):
            print "-> update "+self.operation+" "+self.fse.object_type

        r=re.compile("([a-zA-Z\*]+)\[([a-zA-Z\-1-9]+)\]") # match string 'bmc[OSS]'
        list_permited=["node","hwmanager","switch","disk_array","metaservice","*"]


        if (self.fse.object_type in list_permited or
            r.match(self.fse.object_type)):
            return self.update()
        else:
            return None
        



    def clean(self):
        """
        remove the ipaddr from the equipment port in memory
        """
        if (self.fse.verbose):
            print "**clean**"

        r=re.compile("([a-zA-Z\*]+)\[([a-zA-Z\-1-9]+)\]") # match string 'bmc[OSS]'
        list_permited=["node","hwmanager","switch","disk_array","metaservice","*"]


        if (self.fse.object_type in list_permited or
            r.match(self.fse.object_type)):
            return self.clean_interface()
        else:
            return False
        

################################################################################


class create_logical_network:

    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="create-logical-network"
        self.rule=None
        self.network=None
        self.logical_network=None
        self.prepare_ok=False
        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.network=networkXML()
        self.logical_network=networkXML()

        if self.ea.get_network_name() == self.ea.get_network_parent_name():
            self.network.set_name(self.ea.get_network_name())
            self.logical_network.set_name(self.ea.get_network_name())
            self.prepare_ok=True
        else:
            self.network.set_name(self.ea.get_network_parent_name())
            self.logical_network.set_name(self.ea.get_network_name())
            self.prepare_ok=True


        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_name()    == self.logical_network.get_name() and
                nw.get_subnet()  != None):
                if (nw.get_subnet() == self.ea.get_subnet()):
                    self.msg="logical network "+nw.get_name()+" is already defined : "+nw.get_subnet()
                else:
                    self.msg="logical network "+nw.get_name()+" already has a subnet defined : "+nw.get_subnet()+" != "+self.ea.get_subnet()
                return False

        self.network.set_function(self.ea.get_network_function())
        self.logical_network.set_function(self.ea.get_network_function())
        self.logical_network.set_suffix(self.ea.get_suffix())
        self.logical_network.set_subnet(self.ea.get_subnet())
        self.logical_network.set_type(self.ea.get_network_type())
        self.logical_network.set_gateway(self.ea.get_gateway())
        self.logical_network.set_vpartition(self.ea.get_vpartition())
        self.rule.set_fromXML()
        self.rule.set_network(self.network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(None)
        self.rule.set_member(None)
        return self.prepare_ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False

        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_rule(self.rule)
        self.nar.refresh_network_list_from_db()
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()
        


################################################################################

class remove_logical_network:

    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="remove-logical-network"
        self.rule=None
        self.physical_network=None
        self.logical_network=None
        self.prepare_ok=False
        self.update_ok=False
        self.msg="no message"

        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.physical_network=networkXML()
        self.logical_network=networkXML()
        self.found_physical=False
        self.found_logical=False
        self.found_range=False
        self.found_subnet=False

        for nw in self.fse.nwl.get_all_network():
            if (nw.get_name() == self.ea.get_network_name() or
                nw.get_subnet() == self.ea.get_subnet()):
                self.logical_network=nw
                self.found_logical=True

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True

        if (self.found_physical==False):
            self.physical_network=self.logical_network
            self.found_physical=True

        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()=="range" and
                nw.get_parent_name()==self.logical_network.get_name()):
                self.found_range=True
                self.msg="There is a range attached on this logical network, please remove it first"
                self.prepare_ok=False

        if (self.found_logical and
            self.logical_network.get_subnet()==None):            
            self.msg="There is no subnet on this logical network"
            self.found_subnet=False
        else:
            self.found_subnet=True

        if (self.found_physical==True     and 
            self.found_logical==True      and
            self.found_range==False       and
            self.found_subnet==True):
            self.prepare_ok=True
        else:
            self.prepare_ok=False



        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(None)
        self.rule.set_member(None)
        return self.prepare_ok


    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False

        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_rule(self.rule)
        print self.rule
        self.nar.refresh_network_list_from_db()
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()
        


################################################################################




class create_range:

    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="create-range"
        self.rule=None
        self.physical_network=None
        self.logical_network=None
        self.range=None
        self.found_physical=False
        self.found_logical=False
        self.prepare_ok=False
        self.msg=""
        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.network=networkXML()
        self.logical_network=networkXML()
        self.range=rangeXML()


        self.found_physical=False
        self.found_logical=False

        for nw in self.fse.nwl.get_all_network():
            if (nw.get_name() == self.ea.get_network_name() or
                (nw.get_subnet() != None                    and
                 nw.get_subnet() == self.ea.get_subnet())
                ):
                self.logical_network=nw
                self.found_logical=True

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True

        if self.found_physical==False:
            for nw in self.fse.nwl.get_all_network():
                if nw.get_name() == self.ea.get_network_name():
                    self.physical_network=nw
                    self.found_physical=True
                    

        if self.found_logical:
            ipr=ip_range()
            ipr.setNetworkIPAddress(self.logical_network.get_subnet())
            if ipr.contains(self.ea.get_first_ip())==False:
                self.msg="The subnet "+self.logical_network.get_subnet()+" does not contain this ip : "+self.ea.get_first_ip()
                self.prepare_ok=False
                return False

        #print self.physical_network
        #print self.logical_network

        self.range.set_first_ip(self.ea.get_first_ip())
        self.network.set_function(self.ea.get_network_function())
        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(self.range)
        self.rule.set_member(None)

        if ((self.found_physical and
             self.found_logical)==False):
            self.msg="Physical network or logical network not found"
            self.prepare_ok=False
            return self.prepare_ok
        

        if (self.found_physical and 
            self.found_logical  and
            self.logical_network.get_subnet()==None):
            self.prepare_ok=False
            self.msg="There is no subnet attached to this logical network"
        else:
            self.prepare_ok=True

        return self.prepare_ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        #self.ea.print_string()
        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_rule(self.rule)
        self.nar.refresh_network_list_from_db()
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()
        

################################################################################

class remove_range:

    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="remove-range"
        self.rule=None
        self.physical_network=None
        self.logical_network=None
        self.range=None
        self.found_physical=False
        self.found_logical=False
        self.found_range=False
        self.found_subset=False
        self.prepare_ok=False
        self.msg="no message"
        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.network=networkXML()
        self.logical_network=networkXML()
        self.range=rangeXML()

        self.found_physical=False
        self.found_logical=False
        self.found_range=False
        self.found_subset=False

        for nw in self.fse.nwl.get_all_network():
            if (nw.get_name() == self.ea.get_network_name() or 
                (nw.get_subnet() != None                    and
                 nw.get_subnet() == self.ea.get_subnet())):
                self.logical_network=nw
                self.found_logical=True

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True

        if self.found_physical==False:
            for nw in self.fse.nwl.get_all_network():
                if nw.get_name() == self.ea.get_network_name():
                    self.physical_network=nw
                    self.found_physical=True

        
        if self.found_logical:
            for nw in self.fse.nwl.get_all_network_and_range():
                if (nw.get_parent_name()==self.logical_network.get_name() and
                    nw.get_type()=="range"                                and
                    nw.get_next_ip() == self.ea.get_first_ip()            and
                    nw.get_subset()!=None):
                    self.found_subset=True
                    #self.msg="There is subset associated to this range"
                    #print nw
                    
        if self.ea.get_verbose():
            print self.physical_network
            print self.logical_network

        if ((self.found_physical and
             self.found_logical)==False):
            self.msg="Physical network or logical network not found"
            self.prepare_ok=False
            return self.prepare_ok


        if (self.found_physical==True and
            self.found_logical==True  and
            self.found_subset==True):
            self.prepare_ok=True
        else:
            self.prepare_ok=False




        self.range.set_first_ip(self.ea.get_first_ip())
        self.network.set_function(self.ea.get_network_function())
        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(self.range)
        self.rule.set_member(None)

        return self.prepare_ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_rule(self.rule)
        self.nar.refresh_network_list_from_db()
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()


################################################################################

class create_member:

    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="create-member"
        self.rule=None
        self.physical_network=None
        self.logical_network=None
        self.range=None
        self.found_physical=False
        self.found_logical=False
        self.found_range=False
        self.found_subset=False
        self.found_member=False
        self.prepare_ok=False
        self.msg="no message"
        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.network=networkXML()
        self.logical_network=networkXML()
        self.range=rangeXML()


        self.found_physical=False
        self.found_logical=False

        for nw in self.fse.nwl.get_all_network():
            #print nw
            if (nw.get_name() == self.ea.get_network_name() or 
                (nw.get_subnet() == self.ea.get_subnet() and
                 nw.get_subnet() != None)):
                self.logical_network=nw
                self.found_logical=True

        if self.found_logical==False:
            self.msg="Logical Network not found in database"
            self.prepare_ok=False
            return self.prepare_ok

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True


        if self.found_physical==False:
            for nw in self.fse.nwl.get_all_network():
                if nw.get_name() == self.ea.get_network_name():
                    self.physical_network=nw
                    self.found_physical=True

        #print self.physical_network
        #print self.logical_network


        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()        == "range" and
                nw.get_parent_name() == self.physical_network.get_name()):
                self.found_range=True
                break
            else:
                self.found_range=False
                self.msg="No range match this logical network"


        for nw in self.fse.nwl.get_all_network_and_range():
            if self.found_range==False:
                break
            if (nw.get_type()        == "range"                           and
                nw.get_parent_name() == self.physical_network.get_name()  and
                nw.get_subset()      != None                              and
                nw.get_next_ip()     == self.ea.get_first_ip()):
                #print "nw.get_next_ip => "+nw.get_next_ip()
                #print self.ea.print_string()
                for s in nw.get_subset().split(" "):
                    if (s == self.fse.get_member().get_subset()):
                        self.found_subset=True
                        self.msg="There is already a matching subset in this logical network"


        eql=self.fse.get_equipment_list()
        for eq in eql.get_all_equipment():
            for net in eq.get_all_network_interface():
                if (net.get_ipaddr() != None):
                    self.msg="There is ip on this subset so clean it first, equipment name : ["+eq.get_name()+"] ipaddr : ["+net.get_ipaddr()+"]"
                    self.prepare_ok=False
                    return self.prepare_ok


        if (self.found_physical==True and
            self.found_logical==True  and
            self.found_range==True    and
            self.found_subset==False):
            self.prepare_ok=True


        self.range.set_first_ip(self.ea.get_first_ip())
        self.network.set_function(self.ea.get_network_function())
        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(self.range)
        self.rule.set_member(self.fse.get_member())
        return self.prepare_ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False

        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_rule(self.rule)
        self.nar.refresh_network_list_from_db()
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()


################################################################################

class remove_member:
    def __init__(self):
        self.fse=None                      # A fetch_store_equipment object
        self.ea=None
        self.operation="remove-member"
        self.rule=None
        self.physical_network=None
        self.logical_network=None
        self.range=None
        self.found_physical=False
        self.found_logical=False
        self.prepare_ok=False
        

    def set_fetch_store_equipment(self,fse):
        self.fse=fse
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def prepare_object(self):
        self.rule=rule()
        self.network=networkXML()
        self.logical_network=networkXML()
        self.range=rangeXML()

        self.found_physical=False
        self.found_logical=False
        self.found_subset=False
        self.found_ip=False

        for nw in self.fse.nwl.get_all_network():
            if (nw.get_name() == self.ea.get_network_name() or
                nw.get_subnet() == self.ea.get_subnet()):
                self.logical_network=nw
                self.found_logical=True

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name() == self.logical_network.get_parent_name():
                self.physical_network=nw
                self.found_physical=True


        if self.found_physical==False:
            for nw in self.fse.nwl.get_all_network():
                if nw.get_name() == self.ea.get_network_name():
                    self.physical_network=nw
                    self.found_physical=True


        if self.found_logical:
            for nw in self.fse.nwl.get_all_network_and_range():
                if (nw.get_parent_name()==self.logical_network.get_name() and
                    nw.get_type()=="range"                                and
                    nw.get_subset()!=None):
                    #print nw
                    #print nw.get_subset()
                    #print self.fse.get_member().get_subset()
                    for s in nw.get_subset().split(" "):
                        if (s == self.fse.get_member().get_subset()):
                            self.found_subset=True
                    if self.found_subset==False:
                        self.msg="There is no member found on this range"
                            
        eql=self.fse.get_equipment_list()
        for eq in eql.get_all_equipment():
            for net in eq.get_all_network_interface():
                if (net.get_ipaddr() != None):
                    self.msg="There is ip on this subset so clean it first, equipment name : ["+eq.get_name()+"] ipaddr : ["+net.get_ipaddr()+"]"
                    
                    self.ok=False
                    return self.ok



        if (self.found_physical and 
            self.found_logical  and
            self.found_subset   and
            self.found_ip==False):
            self.prepare_ok=True
        else:
            self.prepare_ok=False

        self.range.set_first_ip(self.ea.get_first_ip())
        self.network.set_function(self.ea.get_network_function())
        self.rule.set_fromXML()
        self.rule.set_network(self.physical_network)
        self.rule.set_logical_network(self.logical_network)
        self.rule.set_range(self.range)
        self.rule.set_member(self.fse.get_member())
        return self.prepare_ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        self.nar=network_allocation_rule(self.fse.cdb)
        self.nar.set_verbose(self.ea.get_verbose())
        self.nar.set_rule(self.rule)
        self.nar.refresh_network_list_from_db()
        self.nar.set_operation(self.operation)
        self.nar.check_networks()
        return self.nar.do_it()



        

################################################################################



class create_logical_network_interface:
    def __init__(self):
        self.net_name_src=None             # a network name from network table
        self.net_name_dst=None             # another network name from network table
        self.fse=None                      # A fetch_store_equipment object
        self.operation="create-logical-network-interface"
        self.id_nw_src=-1
        self.id_nw_dst=-1
        self.port_type_nw_dest=None
    
    def set_engine_argument(self,ea):
        self.ea=ea

    def set_fetch_store_equipment(self,fse):
        self.fse=fse

    def set_name_of_the_network_source(self,net_name_src):
        self.net_name_src=net_name_src

    def set_name_of_the_network_destination(self,net_name_dst):
        self.net_name_dst=net_name_dst

    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False
        return self.update()


    def prepare_object(self):
        """
        update the equipment port ipaddr in memory
        """

        if (self.fse.verbose):
            print "-> update "+self.operation

        if self.net_name_src==self.net_name_dst:
            self.msg="Logical network port can not be on the same network as physical port"
            return False

        for nw in self.fse.nwl.get_all_network():
            if nw.get_name()==self.net_name_src:
                self.id_nw_src=nw.get_id()
            if nw.get_name()==self.net_name_dst:
                self.id_nw_dst=nw.get_id()
                self.port_type_nw_dest=nw.get_function()

        if (self.id_nw_src==-1 or
            self.id_nw_dst==-1):
            self.msg="Can not find network source or destination"
            return False
        return True



    def update(self):
        ok=True
        nwl=self.fse.nwl

        #self.fse.list_equipment()
        #sys.exit(0)
        
        for eq in self.fse.eql.get_all():
            for net_int in eq.get_all_network_interface():
                if (net_int.get_network_name()==self.net_name_src and
                    net_int.get_type()!="logical"):
                    new_net=net_int.clone()
                    new_net.set_type("logical")
                    new_net.set_id(None)
                    new_net.set_ipaddr(None)
                    new_net.set_network_id(self.id_nw_dst)
                    new_net.set_network_name(self.net_name_dst)
                    eq.add_network_interface(new_net)                    
                    ok=True
        return ok


################################################################################



class remove_logical_network_interface:
    def __init__(self):
        self.fse=None                # A fetch_store_equipment object
        self.saferange=False
        self.operation="remove-logical-network-interface"
        self.msg="no message"

    def set_engine_argument(self,ea):
        self.ea=ea

    def set_fetch_store_equipment(self,fse):
        self.fse=fse

    def prepare_object(self):
        """
        update the equipment port ipaddr in memory
        """
        if (self.ea.get_verbose()):
            print "-> update "+self.operation+" "+self.fse.object_type

        list_permited=["node"]

        if (self.fse.object_type==None                           or
            self.fse.object_type in list_permited):
            return True
        else:
            self.msg="no permited object found"
            return False


    def update(self):
        """
        update the equipment port ipaddr in memory
        """

        ok=True
 

        for eq in self.fse.eql.get_all():
            for net_int in eq.get_all_network_interface():
                if (net_int.get_ipaddr()!=None):
                    self.msg="There is ipaddr set on this logical interface"




        return False

        ipr=ip_range()
        ipr_check_db=ip_range()
        ip_from_db=load_ip_from_db(self.fse.cdb)
        ip_from_db.fetch()
        found_range=False

        for nw in self.fse.nwl.get_all_network_and_range():
            if (nw.get_type()    == "range" and
                nw.get_next_ip() == self.firstip and
                nw.get_subset()  != None):
                for s in nw.get_subset().split(" "):
                    if (s == self.fse.get_member().get_subset()):
                        found_range=True

        if found_range==False:
            self.msg="Range or subset not found"
            return found_range

        list_ip_to_deliver=[]

        if self.saferange:
            ipr.setParanoid()

        ipr.setNetworkIPAddress(self.fse.subnet)
        ipr_check_db.setNetworkIPAddress(self.fse.subnet)

        nw=self.fse.nwl.get_network_by_subnet(self.fse.subnet)
        nws=network_sql(self.fse.cdb)
        

        nw_first_ip_available=nw.get_next_ip()
        ipdb=ipaddr(nw_first_ip_available)
        ipcl=ipaddr(self.firstip)

        if ipdb.intrep > ipcl.intrep:
            self.firstip=str(ipdb)

        if (self.firstip):
            ipr.setFirstAddress(self.firstip)
            ipr_check_db.setFirstAddress(self.firstip)

        print ipr.numberOfAvalaibleElement()

        if (self.lastip):
            ipr.setLastAddress(self.lastip)
            ipr_check_db.setLastAddress(self.lastip)




        numberOfNetworkInterfaceToAlloc=0
        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():
                numberOfNetworkInterfaceToAlloc=numberOfNetworkInterfaceToAlloc+1
                list_ip_to_deliver.append(ipr_check_db.getNextIp())
                #if net.
                if (eq.get_table_name()=="node"):
                    break # ONE INTERFACE FOR NOW

        for ip in list_ip_to_deliver:
            if ip_from_db.already_exist(ip)==True:
                print "Error in allocation rule "
                print "This ip "+str(ip)+" is already allocated to another equipment"
                return False
                


        if ( ipr.numberOfAvalaibleElement() < numberOfNetworkInterfaceToAlloc ):
            print "==> There is not enough IP"
            print "==> Number of ip available      : "+str(ipr.numberOfAvalaibleElement())
            print "==> Number of network interface : "+str(numberOfNetworkInterfaceToAlloc)
            ok=False
            return ok
        else:
            print "==> Number of network interface : "+str(numberOfNetworkInterfaceToAlloc)


        for eq in self.fse.eql.get_all():
            for net in eq.get_all_network_interface():
                if (ipr.nextIpIsAvailable()):
                    net.set_ipaddr(ipr.getNextIp())
                    ok=True
                    if (eq.get_table_name()=="node"):
                        break # ONE INTERFACE PER NODE ONLY
#        if ok:
#            nws.update_next_ip_network(nw.get_id(),ipr.getNextIp())

        return ok



    def do_it(self):
        if self.prepare_object()==False:
            print self.msg
            return False

        return self.update()

