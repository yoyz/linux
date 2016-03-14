from   CLM.DB.ipallocation.clusterdb_sql_request        import member_sql,interface_member_sql,network_sql,node_profile_device_sql,node_sql,node_port_sql,hwmanager_port_sql,hwmanager_sql,switch_sql,switch_port_sql,disk_array_sql,disk_array_port_sql,hw_group_sql,metaservice_port_sql

class network_range_before_2140:
    def __init__(self):
        self.create_logical_network=None
        self.fse=None
        self.nwl=None
        self.cdb=None

    def refresh_network_list_from_db(self):
        nws=network_sql(self.cdb)
        self.nwl=nws.fetch_all_network()


    def feed_me(self,fse,create_logical_network):
        self.fse=fse
        self.cdb=self.fse.cdb
        self.create_logical_network=create_logical_network
        self.refresh_network_list_from_db()
        

    def exist_name_logical_network(self):
        is_ok=False
        for nw in self.nwl.get_all_network_and_range():
            if (nw.name       == self.create_logical_network.get_name_of_the_logical_network()):
                is_ok=True
                return is_ok
        return is_ok

    def exist_full_logical_network(self):
        is_ok=False
        for nw in self.nwl.get_all_network_and_range():
            if (nw.name       == self.create_logical_network.get_name_of_the_logical_network() and
                nw.type       == self.create_logical_network.get_type_of_logical_network()     and 
                nw.subnet     == self.create_logical_network.get_subnet_of_logical_network()   and
                nw.gateway    == self.create_logical_network.get_gateway_of_logical_network()  and
                nw.suffix     == self.create_logical_network.get_suffix_of_logical_network()   and
                nw.vpartition == self.create_logical_network.get_vlan_pkey_of_logical_network()
                ):
                is_ok=True
                return is_ok
        return is_ok

    def exist_semi_logical_network(self):
        is_ok=False
        for nw in self.nwl.get_all_network_and_range():
            if (nw.name       == self.create_logical_network.get_name_of_the_logical_network() and
                nw.subnet     != None                                                          or
                nw.gateway    != None                                                          or 
                nw.suffix     != None                                                          or
                nw.vpartition != None
                ):
                is_ok=True
                return is_ok
        return is_ok

    def fill_logical_network(self):
        nw_xml=networkXML()
        nw_xml.set_name(create_logical_network.get_name_of_the_logical_network())
        nw_xml.set_type(create_logical_network.get_type_of_logical_network())
        nw_xml.set_subnet(create_logical_network.get_subnet_of_logical_network())
        nw_xml.set_gateway(create_logical_network.get_gateway_of_logical_network())
        nw_xml.set_suffix(create_logical_network.get_suffix_of_logical_network())
        nw_xml.set_vpartition(create_logical_network.get_vpartition())
    



    def apply(self):
        ok=True
        logical_network_name_exist=False
        full_logical_network_exist=False
        if self.exist_name_logical_network():
            print "logical_network_name_exist"
            logical_network_name_exist=True

        if logical_network_name_exist:
            if self.exist_full_logical_network():
                print "full_logical_network_exist"
                full_logical_network_exist=True
            else:
                if self.exist_semi_logical_network():
                    print "semi_logical_network_exist"
                    print "Please remove it first"
                    print ""
                else:
                    self.fill_logical_network()
                
        return full_logical_network_exist
    


class network_range_current_2140:
    def __init__(self):
        pass

class network_range_upgrade:
    def __init(self):
        pass
