#!/usr/bin/python
import sys,getopt,re,os
from pg                                             import *
from CLM.DB.ipallocation.clusterdb_sqlalchemy         import clusterdb_alchemy,Cluster,Network,Node,Node_port,Hwmanager,Hwmanager_port,Node_ftype,Ftype,Hw_group,Hw_group_member,Switch,Switch_port,Node_profile,Node_profile_device,Disk_array,Metaservice_port
import socket

class clusterdb_glue:

    def __init__(self):
        self.db_config_file="/BCM/conf/clusterdb/clusterdb.conf"
        self.db_host=None
        self.db_name=None
        self.db_user=None
        self.db_pass=None
        self.db_handler=None
        self.dict={}
        self.tables=[]
        self.is_connected=False
        self.db_version=None

        self.alchemy=None
        self.engine=None
        self.Session=None
        self.session=None
        self.metadata=None
        

    def set_host(self,host):
        self.db_host=host

    def set_name(self,name):
        self.db_name=name

    def set_user(self,user):
        self.db_user=user

    def set_passwd(self,db_pass):
        self.db_pass=db_pass

    def set_db_version(self,db_version):
        self.db_version=db_version
                      
    def get_host(self):
        return self.db_host

    def get_name(self):
        return self.db_name

    def get_user(self):
        return self.db_user

    def get_passwd(self):
        return self.db_pass

    def get_db_version(self):
        return self.db_version

    def get_handle(self):
        return self.db_handler


    def connect_sqlalchemy(self):
        self.alchemy=clusterdb_alchemy()
        self.alchemy.set_user(self.get_user())
        self.alchemy.set_passwd(self.get_passwd())
        self.alchemy.set_host(self.get_host())
        self.alchemy.set_name(self.get_name())
        self.alchemy.set_db_version(self.get_db_version())
        self.alchemy.connect()
        self.session=self.alchemy.session
        self.Session=self.alchemy.Session
        self.engine=self.alchemy.engine
        self.metadata=self.alchemy.metadata
        #print("connected to postgresql://"+self.get_user()+"@"+self.get_host()+"/"+self.get_name())


    def disconnect_sqlalchemy(self):
        print("disconnect sqlalchemy")


    def connect(self):
        try:
            self.db_handler = DB(host   = self.get_host(), 
                                 dbname = self.get_name(), 
                                 user   = self.get_user(),
                                 passwd = self.get_passwd())
        except InternalError,e:
            print("Can not connect to database "+self.get_name()+" on host "+self.get_host())
            print(str(e))
            sys.exit(1)
        self.is_connected=True
        self.fetch_tables()
        self.fetch_dbversion()
        self.connect_sqlalchemy()


    def disconnect(self):
        if self.is_connected==False:
            return True        
        try:
            self.db_handler.close()
            self.disconnect_sqlalchemy()
            self.is_connected=False
        except Exception,e:
            print("Trying to close an already closed connection")
            print(e)


    def try_to_open_config_file(self):
        try:
            fp = open(self.db_config_file, 'r')
            params = fp.readlines()
            fp.close()
            
        except (OSError, IOError), (errno, strerror):
            sys.stderr.write("File %s error %s : %s\n" %(self.db_config_file, errno, strerror))
            sys.exit(1)
        return True
        


    def read_config_file(self):
        #print "READ"
        try:
            fp = open(self.db_config_file, 'r')
            file_content = fp.readlines()
            fp.close()
            
        except (OSError, IOError), (errno, strerror):
            sys.stderr.write("File %s error %s : %s\n" %(self.db_config_file, errno, strerror))
            sys.exit(1)
            
        for comp in file_content:
            if not re.match('^\s*$|^\s*#', comp):
                tmp = comp.split('\n')
                tmp = tmp[0].replace(' ', '')
                list = tmp.split('=')
                if len(list) == 2 :
                    self.dict[list[0]] = list[1]

        if self.check_config_info():
            if self.db_host==None:
                self.db_host=     self.dict['host']
            if self.db_name==None:
                self.db_name=     self.dict['name']
            if self.db_user==None:
                self.db_user=     self.dict['user']
            if self.db_pass==None:
                self.db_pass=     self.dict['password']
            return True
        else:
            return False
            

    def check_config_info(self):
        try_ping=True
        if (self.dict['host'] and 
            self.dict['name'] and 
            self.dict['user'] and 
            self.dict['password']):
            if   re.match('[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+',self.dict['host']):
                try_ping=True
            elif re.match('[a-zA-Z][a-zA-Z\.]+'          ,self.dict['host']):
                try:
                    data=socket.gethostbyname_ex(self.dict['host'])
                    return True
                except socket.gaierror:
                    print("Could not resolve "+self.dict['host'])
                    print("Try to reach "+self.dict['host']+" with 127.0.0.1")
                    self.dict['host']="127.0.0.1"
            else:
                print("Wrong address "+dict['host'])
                sys.exit(1)
            return True
                
                    
    def exec_query(self, query):
        if self.is_connected==False:
            print("Not connected")
            sys.exit(1)

	try:
	        queryobject = self.db_handler.query(query)
	except:
		print("query Failed : "+query)
		return None
        if type(queryobject) != type(None):
            #print "query OK"
            return queryobject.dictresult()
        else:          
            print("query Failed : "+query)
            return None 

    def fetch_tables(self):
        tables=self.db_handler.get_tables()
        i=0
        for t in tables:
            s=re.search('(?<=public.)\w+', t)
            if (s!=None):
                self.tables.append(s.group(0))
                i=i+1
        #print(self.tables)
        if i>0:
            return True
        else:
            return False

        
    def fetch_dbversion(self):
        query="SELECT dbversion FROM cluster"
        db_version_dict = self.exec_query(query)
        if db_version_dict!=None:
            self.set_db_version(db_version_dict[0]['dbversion'])
            return True
        else:
            return False


    def fetch_and_check(self):
        self.try_to_open_config_file() 
        self.read_config_file()   
        self.connect()
        return True



if __name__ == '__main__':
    cdb=clusterdb_glue()
    if (cdb.fetch_and_check()==True):
        print("Ok")
    cdb.disconnect()

