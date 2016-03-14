#!/usr/bin/python
import sys,getopt,re,os
import sqlalchemy
from sqlalchemy import *
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import mapper
from sqlalchemy import Table, Column, Integer, String, MetaData, ForeignKey





class Cluster(object):
    def __repr__(self):
        return "<Cluster('%s','%s', '%s',%s)>" % (self.id, self.name, self.basename,self.mode)
    pass

class Network(object):
    def __repr__(self):
        return "<Network('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')>" % (self.id, self.name, self.type,self.function,self.subfunction,self.techno,self.subnet,self.gateway,self.cluster_id,self.network_id,self.comment)
    pass

class Node(object):
    def __repr__(self):
        return "<Node('%s','%s', '%s')>" % (self.id, self.name, self.model)
    pass

class Node_port(object):
    def __repr__(self):
        return "<Node_Port('%s','%s', '%s', '%s', '%s', '%s')>" % (self.ip, self.type, self.hwid, self.hba_rank, self.port_rank)
    pass

class Hwmanager(object):
    def __repr__(self):
        return "<Hwmanager('%s','%s', '%s')>" % (self.id, self.name, self.model)
    pass


class Hwmanager_port(object):
    def __repr__(self):
        return "<Hwmanager_Port('%s','%s', '%s' )>" % (self.ip, self.hwid)
    pass


class Switch(object):
    def __repr__(self):
        return "<Switch('%s','%s')>" % (self.name, self.type, self.model)
    pass


class Switch_port(object):
    def __repr__(self):
        return "<Switch_port('%s','%s')>" % (self.type, self.ipaddr)
    pass


class Disk_array(object):
    def __repr__(self):
        return "<Disk_array('%s','%s','%s')>" % (self.name, self.model,self.type)
    pass

class Disk_array_port(object):
    def __repr__(self):
        return "<Disk_array('%s','%s','%s')>" % (self.name, self.model,self.type)
    pass


class Da_port(object):
    def __repr__(self):
        return "<Da_port('%s','%s')>" % (self.name, self.ipaddr)
    pass

class Rack(object):
    def __repr__(self):
        return "<Rack('%s','%s')>" % (self.label, self.model)
    pass


class Node_ftype(object):
    def __repr__(self):
        return "<Node_ftype('%s','%s')>" % (self.node_id, self.ftype_id)
    pass


class Ftype(object):
    def __repr__(self):
        return "<Ftype('%s','%s')>" % (self.id, self.label)
    pass

class Hw_group(object):
    def __repr__(self):
        return "<Hw_group('%s','%s')>" % (self.name, self.type)
    pass


class Hw_group_member(object):
    def __repr__(self):
        return "<Hw_group_member('%s','%s', '%s' )>" % (self.hw_group_id, self.target_table,self.ref_id)
    pass

class Node_profile(object):
    def __repr__(self):
        return "<Node_profile('%s','%s', '%s','%s','%s', '%s','%s','%s', '%s')>" % (self.id,self.name,self.architecture,self.nb_local_disks,self.boot_loader,self.deploy_kernel_suffix,self.pm_enable_action,self.pm_priority,self.comment)
    pass

class Node_profile_device(object):
    def __repr__(self):
        return "<Node_profile_device('%s','%s', '%s','%s','%s', '%s','%s','%s')>" % (self.id,self.node_profile_id,self.type,self.iname,self.network_id,self.comment,self.hba_rank,self.port_rank)
    pass

class Metaservice_port(object):
    def __repr__(self):
        return "<Metaservice_port('%s','%s', '%s','%s','%s', '%s','%s','%s')>" % (self.id,self.name,self.type,self.ipaddr,self.hwaddr,self.network_id,self.node_id,self.comment)
    pass



class clusterdb_alchemy:
   
    def __init__(self):
        self.engine=False
        self.Session=False
        self.session=False
        self.metadata=False

        #self.sgbd="postgres"
	self.check_version() 	# -> self.sgdb
        self.dbuser="clusterdb"
        self.dbpasswd="clusterdb"
        self.dbhost="127.0.0.1"
        self.dbname="clusterdb"
        self.dbversion=None

    def check_version(self):
        """
        The old SGBD URL is deprecated. This is mainly to avoid a warning...
        """
        (major, minor, fix) = sqlalchemy.__version__.split('.')
        sqlalchemy_version = float("%s.%s" % (major, minor))
        if sqlalchemy_version >= 0.6:
                self.sgbd = "postgresql"
        else:
                self.sgbd = "postgres"
        return sqlalchemy_version

    def set_user(self,user):
        self.dbuser=user

    def set_passwd(self,passwd):
        self.dbpasswd=passwd

    def set_host(self,host):
        self.dbhost=host

    def set_name(self,name):
        self.dbname=name

    def set_db_version(self,dbversion):
        self.dbversion=dbversion
    

    def connect(self):
        if (self.dbversion==None):
            print("ERROR: can't find dbversion.")
            sys.exit(1)

        connect_engine_str=self.sgbd+"://"+self.dbuser+":"+self.dbpasswd+"@"+self.dbhost+":5432"+"/"+self.dbname

#        print connect_engine_str
        try:
            self.engine = create_engine(connect_engine_str, echo=False)
#        self.engine = create_engine(connect_engine_str, echo=True)

            self.Session = sessionmaker()
            self.Session.configure(bind=self.engine)
            self.session=self.Session()
            self.metadata=MetaData(bind=self.engine,reflect=True)

        except :
            print("I am very sorry, unexpected error connecting to db")
            print connect_engine_str
            sys.exit(1)
        
	(_major, _minor, _fix) = self.dbversion.split('.')
	_dbversion = float("%s.%s" % (_major, _minor))
        if _dbversion == 21.2:
	    print("Warning: this database version is not fully supported.")
            self.load_table_clusterdb_21xx()
            self.load_table_specific_clusterdb_2120()
        elif _dbversion == 21.3:
	    print("Warning: this database version is not fully supported.")
            self.load_table_clusterdb_21xx()
            self.load_table_specific_clusterdb_2130()
	elif _dbversion >= 21.4:
	    self.load_table_clusterdb_21xx()
	    self.load_table_specific_clusterdb_2140()
        else:
            print("ERROR: does not handle this dbversion.")
            sys.exit(1)
    


    def load_table_clusterdb_21xx(self):
        cluster_table                 = Table('cluster',              self.metadata, autoload=True)

        network_table                 = Table('network',              self.metadata, autoload=True)

        rack_table                    = Table('rack',                 self.metadata, autoload=True)

        node_ftype_table              = Table('node_ftype',           self.metadata, autoload=True)
        ftype_table                   = Table('ftype',                self.metadata, autoload=True)


        node_profile_table            = Table('node_profile',         self.metadata, autoload=True)
        node_profile_device_table     = Table('node_profile_device',  self.metadata, autoload=True)


        node_table                    = Table('node',                 self.metadata, autoload=True)
        node_port_table               = Table('node_port',            self.metadata, autoload=True)

        hwmanager_table               = Table('hwmanager',            self.metadata, autoload=True)
        hwmanager_port_table          = Table('hwmanager_port',       self.metadata, autoload=True)


        disk_array_table              = Table('disk_array',           self.metadata, autoload=True)
        disk_array_port_table         = Table('disk_array_port',      self.metadata, autoload=True)


        hw_group_table                = Table('hw_group',             self.metadata, autoload=True)
        hw_group_member_table         = Table('hw_group_member',      self.metadata, autoload=True)
        
        metaservice_port_table        = Table('metaservice_port',     self.metadata, autoload=True)

        mapper(Cluster,cluster_table)
        mapper(Network,network_table)
        mapper(Rack,rack_table)

        mapper(Node,node_table)
        mapper(Hwmanager,hwmanager_table)
        mapper(Disk_array,disk_array_table)

        mapper(Node_port,node_port_table)
        mapper(Hwmanager_port,hwmanager_port_table)
        mapper(Disk_array_port,disk_array_port_table)


        mapper(Node_ftype,node_ftype_table)
        mapper(Ftype,ftype_table)

        mapper(Hw_group,hw_group_table)
        mapper(Hw_group_member,hw_group_member_table)

        mapper(Node_profile,node_profile_table)
        mapper(Node_profile_device,node_profile_device_table)

        mapper(Metaservice_port,metaservice_port_table)


    def load_table_specific_clusterdb_2120(self):
        switch_table                  = Table('switch',               self.metadata, autoload=True)
        switch_port_table             = Table('switch_mngt_port',     self.metadata, autoload=True)
        mapper(Switch,switch_table)
        mapper(Switch_port,switch_port_table)
        

    def load_table_specific_clusterdb_2130(self):
        switch_table                  = Table('switch',               self.metadata, autoload=True)
        switch_port_table             =	Table('switch_mngt_port',     self.metadata, autoload=True)
        mapper(Switch,switch_table)
        mapper(Switch_port,switch_port_table)


    def load_table_specific_clusterdb_2140(self):
        switch_table             = Table('switch',               self.metadata, autoload=True)
        switch_port_table        = Table('switch_port',     self.metadata, autoload=True)
        mapper(Switch,switch_table)
        mapper(Switch_port,switch_port_table)


    def map_clusterdb_2140(self):
        pass




