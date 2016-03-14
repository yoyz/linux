import sys,getopt,re,os

from   CLM.DB.ipallocation.clusterdb_sql_request        import member_sql,interface_member_sql,network_sql,node_profile_device_sql,node_sql,node_port_sql,hwmanager_port_sql,hwmanager_sql
from   CLM.DB.ipallocation.clusterdb_sqlalchemy		import clusterdb_alchemy,Cluster,Network,Node,Node_port,Hwmanager,Hwmanager_port,Node_ftype,Ftype,Hw_group,Hw_group_member,Switch,Node_profile,Node_profile_device,Disk_array
import misc

class network_allocation_rule:
    """
    Check the network table in the db
    ||
    &&
    Apply modification in the network table
    ||
    Clean the network
    """


    def __init__(self,cdb):
        self.cdb=cdb
        self.network_from_db=None
        self.network_from_xml=None
        self.logical_network_from_xml=None
        self.member_xml=None
        self.nwl=None
        self.rule=None
        self.rule_number=0

        self.network_exist=None
        self.logical_network_exist=None
        self.range_exist=None
        self.subset_exist=None

        self.subset_pattern=""

        self.newly_created_network=False
        self.newly_created_network_type=None
        self.newly_created_network_id=-1
        self.newly_parent_network_id=-1
        self.v=False
        self.operation=False                   # update, clean


    def check_networks(self):
        """
        call the internal check
        """
        self.printv("*check_networks")
        ok=True
        if self._exist_network_in_db()==False:
            self.network_exist=False
            ok=False
            self.printv("**Network does not exist")
        else:
            self.printv("**Network exist")
            self.network_exist=True

        if self._exist_logical_network_in_db()==False:
            ok=False
            self.logical_network_exist=False
            self.printv("**Logical network does not exist")
        else:
            self.printv("**Logical network exist")
            self.logical_network_exist=True
            
        if (self.range and 
            self._exist_range_in_db()==False):
            ok=False
            self.range_exist=False
            self.printv("**Range in network does not exist")
        else:
            self.printv("**Range network exist")
            self.range_exist=True

        if (self.member and 
            self._exist_subset_in_db()==False):
            ok=False
            self.subset_exist=False
        elif (self.member and self._exist_subset_in_db()==True):
            self.subset_exist=True

        if self.subset_exist==None:
            self.subset_exist=False

        return ok


    def do_it(self):
        if self.operation == "create-logical-network":
            return self.update_logical_network()
        if self.operation == "create-range":
            return self.update_range()
        if self.operation == "create-member":
            return self.update_member()

        if self.operation == "remove-logical-network":
            return self.remove_logical_network()
        if self.operation == "remove-range":
            return self.remove_range()
        if self.operation == "remove-member":
            return self.remove_member()


    def update_logical_network(self):
        """
        update network with :
        a physical network as source
        a logical network  as destination
        a range            not mandatory
        a subset           not mandatory
        """
        self.printv("*update_networks")
        ok=False
        if self.network_exist==False:
            return False

        if self.logical_network_exist==False:
            if self.network_from_xml.get_name()!=self.logical_network_from_xml.get_name():
                self._insert_logical_network_in_db()
                self.refresh_network_list_from_db()
                self.keep_new_network_info()                
                ok=True

            elif (self._not_already_configured_logical_network_in_db()):
                self.set_value_to_logical_network_in_db()
                self.refresh_network_list_from_db()
                self.keep_new_network_info()
                ok=True
        return ok


    def update_range(self):
        self.printv("*update_range*")
        #print self.range_exist
        #print self.subset_exist
        #print self.range
        if self.range_exist==False and self.subset_exist==False and self.range:
            self.printv("**The range does not exist in the db, I will create the range entry")
            return self.add_range_subset_network_in_db()


    def update_member(self):
        self.printv("*update_member*")
        if self.subset_exist==False:
            self.printv("**I have to update the subset, is there a range ?")
            if self.range_exist==True:
                self.printv("**The range exist so is there a subset defined ?")
                if self.subset_exist==False:
                    self.printv("***The subset does not exist in the db, I will update the range entry***")
                    return self.update_range_subset_network_in_db()
                    #return self.add_range_subset_network_in_db()
                else:
                    self.printv("***add range entry***")
                    return self.add_range_subset_network_in_db()
        return False


    def remove_logical_network(self):
        """
        clean a : 
        logical network or 
        range           or
        subset
        with this parameters
        a physical network as source
        a logical network  as destination
        a range            not mandatory
        a subset           not mandatory
        """

        self.printv("*remove_logical_network*")

        if self.network_exist==False:
            return False

        if self.logical_network_exist==False:            
            return False
        else:
            return self._delete_logical_network_in_db()


    def remove_range(self):

        self.printv("*remove_range*")
        if self.range_exist==False:
            return False
        else:
            return self._delete_range_network_in_db()

        #self._delete_logical_network_in_db()
        self.refresh_network_list_from_db()


    def remove_member(self):
        #print "REMOVE MEMBER"
        ok=False
        if self._exist_logical_network_in_db():
            ok=True
        else:
            return False

        #print "REMOVE SUBSET NETWORK"
        nws=network_sql(self.cdb)
        self.refresh_network_list_from_db()
        ok=nws.remove_subset_network(self._get_id_range_in_db(),self.member.get_subset())
        #print ok
        return ok


    def set_verbose(self,verbose):
        self.v=verbose


    def printv(self,str):
        if self.v:
            print str


    def set_operation(self,operation):
        self.operation=operation
        operation_permited=[
            "create-member",
            "create-range",
            "create-logical-network",
            "remove-member",
            "remove-range",
            "remove-logical-network",
            ]
        if operation in operation_permited:
            return True
        else:
            print "***operation not found***"
            print "***operation permited***"
            print str(operation_permited)
            print "***used : "+str(operation)
            sys.exit(1)
        
            
    def set_rule(self,rule):
        self.network_from_xml=rule.get_network()
        self.logical_network_from_xml=rule.get_logical_network()
        self.member=rule.get_member()
        self.range=rule.get_range()
        self.rule_number=rule.get_rule_number()
        if self.member!=None:
            self.subset=self.member.get_subset()
	    # escaping *was* needed for a regex match later
            self.subset_pattern=self.member.get_subset() #.replace("[","\[").replace("]","\]")
        self.rule=rule


    def get_rule(self):
        return self.rule
    

    def refresh_network_list_from_db(self):
        nws=network_sql(self.cdb)
        self.nwl=nws.fetch_all_network()


    def add_range_subset_network_in_db(self):
        ok=False
        if self._exist_logical_network_in_db():
            ok=True
        else:
            return False

        for nw in self.nwl.get_all_network_and_range():
            if ( nw.get_subnet()   == self.logical_network_from_xml.get_subnet() and
                 nw.get_type()     == self.logical_network_from_xml.get_type()):
                nws=network_sql(self.cdb)
		_rngname = str(self.range.get_first_ip())+"-"+misc.do_md5(nw.get_name(), 12)
                if self.member:
		    return nws.add_range_network(nw.id, _rngname, self.member.get_subset(), self.range.get_first_ip())
                    #return nws.add_range_network(nw.id,nw.get_name()+"-"+str(self.range.get_first_ip()),self.member.get_subset(),self.range.get_first_ip())
                    #return nws.add_range_network(nw.id,nw.get_name()+"-"+str(self.rule_number),self.member.get_subset(),self.range.get_first_ip())
                else:
		    return nws.add_range_network(nw.id, _rngname, None, self.range.get_first_ip())
                    #return nws.add_range_network(nw.id,nw.get_name()+"-"+str(self.range.get_first_ip()),None,self.range.get_first_ip())
                    #return nws.add_range_network(nw.id,nw.get_name()+"-"+str(self.rule_number),None,self.range.get_first_ip())


    def update_range_subset_network_in_db(self):
        ok=False
        if self._exist_logical_network_in_db():
            ok=True
        else:
            return False

        nws=network_sql(self.cdb)
        self.refresh_network_list_from_db()
        ok=nws.update_subset_network(self._get_id_range_in_db(),self.member.get_subset())
        return ok


    def update_primary_network_db(self):
        ok=False
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.name     == self.network_from_xml.get_name() and
                 nw.function == self.network_from_xml.get_function()):
                nws=network_sql(self.cdb)
                nws.add_primary_network(nw,self.logical_network_from_xml)
                ok=True
        return ok


    def get_number_of_network_with_this_function(self):
        number=0
        for nw in self.nwl.get_all_network_and_range():
            if (nw.function==self.network_from_xml.get_function()):
                number=number+1
        return number


    def _exist_network_in_db(self):
        """
        return True or False
        """
        ok=False
        #print self.rule
        for nw in self.nwl.get_all_network_and_range():
            #print nw
            #print self.network_from_xml.get_name()
            #print self.network_from_xml.get_function()
            if ( nw.name     == self.network_from_xml.get_name() and
                 nw.function == self.network_from_xml.get_function()):
                ok=True
        return ok


    def _not_already_configured_logical_network_in_db(self):
        """
        return True or False
        """
        ok=False
        for nw in self.nwl.get_all_network_and_range():
            if (nw.name       == self.network_from_xml.get_name() and
                nw.function   == self.network_from_xml.get_function() and
                nw.subnet == None):
                ok=True
        return ok


    def _exist_logical_network_in_db(self):
        """
        return True or False
        """
        ok=False
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.subnet     == self.logical_network_from_xml.get_subnet() and
                 nw.type       == self.logical_network_from_xml.get_type()):
                ok=True
        return ok


    def _get_id_network_in_db(self):
        """
        return True or False
        """
        id=-1
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.name     == self.network_from_xml.get_name() and
                 nw.function == self.network_from_xml.get_function()):
                id=nw.id
        return id


    def _get_id_logical_network_in_db(self):
        """
        return True or False
        """
        id=-1
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.subnet     == self.logical_network_from_xml.get_subnet() and
                 nw.type       == self.logical_network_from_xml.get_type()):
                id=nw.id
        return id


    # TODO FIXME
    def _exist_range_in_db(self):
        """
        return True or False
        """
        self.printv("**Exist_range_in_db ?")
        ok=False
        id=-1
        exist=False

        exist=self._exist_logical_network_in_db()

        # We haven't found any logical network which met our requirement
        if exist==False:
            self.printv("**Logical network does not exist : self._exist_logical_network_in_db()==False ")
            return exist


        id=self._get_id_logical_network_in_db()
        # So we should have the parent network id
        for nw in self.nwl.get_all_network_and_range():
            if (nw.type             == "range" and
                nw.get_network_id() == id      and
                nw.get_first_ip()   == self.range.get_first_ip()):
                ok=True
        return ok


    def _get_id_range_in_db(self):
        """
        return True or False
        """
        self.printv("**Exist_range_in_db ?")
        ok=False
        id=-1
        
        if self._exist_range_in_db()==False:
            self.printv("**Range does not exist")
            return id

        # So we should have the parent network id
        for nw in self.nwl.get_all_network_and_range():
            if (nw.type             == "range" and
                nw.get_first_ip()   == self.range.get_first_ip()):
                id=nw.id
        return id


    def get_network_xml_from_id_in_db(self):
        """
        return an xml network entry from the id of this entry 
        """
        ret_xml=None
        my_id=self._get_id_range_in_db()
        for nw in self.nwl.get_all_network_and_range():
            if (nw.id               == my_id):
                ret_xml=nw
        return ret_xml


    def _exist_subset_in_db(self):
        """
        return True or False
        """
        self.printv("**Exist_subset_in_db ?")
        if len(self.subset_pattern)==0:
            return False
        ok=False
        id=-1

	# BUGGY/DEPRECATED
        #r=re.compile(".+"+self.subset_pattern+".+")
        #r=re.compile("[\ ^]"+self.subset_pattern+"[\ $]")

        id=self._get_id_logical_network_in_db()

        # We haven't found any logical network which met our requirement
        if id==-1:
            return False

        # So we should have the parent network id
        for nw in self.nwl.get_all_network_and_range():
            if (nw.get_network_id()==id):
                if (nw.get_subset()!=None):
		    subset_list = nw.get_subset().split(" ")
		    for s in subset_list:
			if (s == self.subset_pattern):
	                    ok = True
			    break

        if ok:
            self.printv("**Exist_subset_in_db OK")
        else:
            self.printv("**Exist_subset_in_db Not OK")
        return ok

    # TODO check if this match is the better method...
    def _get_id_subset_in_db(self):
        """
        return id > 0 or -1 if not found
        """
        ok=False
        id=-1

        #r=re.compile(".+"+self.subset_pattern+".+")
        r=re.compile("[\s ^]"+self.subset_pattern+"[\ $]")

        id=self._get_id_logical_network_in_db()

        # We haven't found any logical network which met our requirement
        if id==-1:
            return id

        # So we should have the parent network id
        for nw in self.nwl.get_all_network_and_range():
            if (nw.get_network_id()==id):
                if (r.match(".+"+nw.get_subset()+".+")):
                    self.printv("_get_id_subset_in_db : "+str(nw.id))
                    id=nw.id
        return id


    def new_network_created(self):
        return self.newly_created_network

    def get_new_network_id(self):
        id=-1
        if self.newly_created_network==True:
            id=self.newly_created_network_id
        return id

    def get_new_parent_network_id(self):
        id=-1
        if self.newly_created_network==True:
            id=self.newly_parent_network_id
        return id

    def get_new_network_type(self):
        type=""
        if self.newly_created_network==True:
            type=self.newly_created_network_type
        return type


    def keep_new_network_info(self):
        self.refresh_network_list_from_db()
        self.newly_created_network=True
        self.newly_created_network_id=self._get_id_logical_network_in_db()
        self.newly_parent_network_id=self._get_id_network_in_db()
        self.newly_created_network_type=self.logical_network_from_xml.get_type()
    

    def _insert_logical_network_in_db(self):
        """
        return True or False
        """
        ok=False
        nws=network_sql(self.cdb)
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.name     == self.network_from_xml.get_name() and
                 nw.function == self.network_from_xml.get_function()):
                ok=nws.add_logical_network(nw,self.logical_network_from_xml)
                if ok==False:
                    return False            
                self.printv("***insert_logical_network_in_db : "+str(ok))

        self.refresh_network_list_from_db()
        id=self._get_id_logical_network_in_db()
        if self.range:
            ok=nws.update_next_ip_network(id,self.range.get_first_ip())
        else:
            if id!=-1:
                ok=True

        if ok==False:
            return False
        else:
            return True


    def _delete_logical_network_in_db(self):
        """
        return True or False
        """
        ok=False
        nws=network_sql(self.cdb)
        for nw in self.nwl.get_all_network_and_range():
            if ( nw.name     == self.logical_network_from_xml.get_name()):
                ok=nws.delete_logical_network(self.logical_network_from_xml)
                self.printv("***delete_logical_network_in_db : "+str(ok))
        self.refresh_network_list_from_db()
        return ok


    def _delete_range_network_in_db(self):
        """
        return True or False
        """
        ok=False
        nws=network_sql(self.cdb)
        ok=nws.delete_logical_network(self.get_network_xml_from_id_in_db())
        return ok


    def set_value_to_logical_network_in_db(self):
        """
        must be inserted first
        return True or False
        """
        ok=False
        if self._exist_network_in_db():
            id=self._get_id_network_in_db()
            nws=network_sql(self.cdb)
            ok=nws.update_subnet_network(id,self.logical_network_from_xml.get_subnet())
            #print str(ok)+" "+str(id)+" "+self.logical_network_from_xml.get_subnet()
            if ok==False:
                return False


            if (self.logical_network_from_xml.get_suffix()!=None):
                ok=nws.update_suffix_network(id,self.logical_network_from_xml.get_suffix())

            if ok==False:
                return False

            if (self.logical_network_from_xml.get_vpartition()!=None):
                ok=nws.update_vpartition_network(id,self.logical_network_from_xml.get_vlanid())

            if ok==False:
                return False


            if (self.range!= None and self.range.get_first_ip()!=None):
                ok=nws.update_next_ip_network(id,self.range.get_first_ip())

            if ok==False:
                return False

            if (self.logical_network_from_xml.get_gateway()!=None):
                ok=nws.update_gateway_network(id,self.logical_network_from_xml.get_gateway())
        return ok


    def clean_network_db(self):
        """
        return True or False

        subset subnet gateway network_id next_ip suffix

        """
        ok=True
        nws=network_sql(self.cdb)
        if nws.clean_secondary_network()==False:
            return False
        
        if nws.clean_range()==False:
            return False
                
        if nws.clean_son_network()==False:
            return False

        if nws.clean_primary_network()==False:
            return False

        return ok


class extract_xml_from_network:
    def __init__(self):
        pass


class associate_port_allocation_rule:
    """
    Associate node_port, hwmanager_port... to network_id
    """
    def __init__(self):
        pass


class equipment_port_allocation_rule:
    """
    """

    def __init__(self):
        pass

