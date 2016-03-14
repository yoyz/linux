#!/usr/bin/python
import subprocess
import os
import sys
import getopt

class manage_db_instance:

    def __init__(self):
        self.dumpdbsav="/tmp/dumpdb-sav.sql"
        self.dbuser=None
        self.dbpasswd=None
        self.dbhost=None
        self.dbnamesrc=None
        self.dbnamedst=None



    def set_dbnamesrc(self,dbsrc):
        self.dbnamesrc=dbsrc

    def set_dbnamedst(self,dbdst):
        self.dbnamedst=dbdst


    def set_dbuser(self,user):
        self.dbuser=user

    def set_dbpasswd(self,passwd):
        self.dbpasswd=passwd

    def set_dbhost(self,host):
        self.dbhost=host


    def get_dbnamesrc(self):
        return self.dbnamesrc

    def get_dbnamedst(self):
        return self.dbnamedst

    def get_dbuser(self):
        return self.dbuser


    def get_dbuser(self):
        return self.dbuser

    def get_dbpasswd(self):
        return self.dbpasswd

    def get_dbhost(self):
        return self.dbhost


    def is_exist(self,mydbname):
        ok=False

        if (mydbname==None):
            return ok

        self._set_env()

#        cmd="/usr/bin/psql -H "+self.get_dbhost()+" -l "+mydbname
        cmd="/usr/bin/psql -l "+mydbname
        devnull=" 2>/dev/null >/dev/null"
        fullcmd=cmd+devnull

        p1=subprocess.Popen(fullcmd,shell=True,env=os.environ)
        p1.wait()

        self._unset_env()

        if p1.returncode==0:
            ok=True
        else:
            ok=False
        return ok


    def dump_all_sql(self,dbname):
        ok=False

        self._set_env()
        cmd="/usr/bin/pg_dump -h "+self.get_dbhost()+" "+dbname
        dest=" 2>/dev/null > "+self.dumpdbsav

        fullcmd=cmd+dest
        

        p1=subprocess.Popen(fullcmd,shell=True,env=os.environ)

        p1.wait()

        self._unset_env()

        if p1.returncode==0:
            try:
                stat=os.stat(self.dumpdbsav)
                if stat.st_size>0:
                    ok=True       
            except OSError:
                pass

        else:
            ok=False


        if ok==False:
            print "Creating dump fail"

        return ok


    def import_all_sql(self,dbname):

        ok=False

        try:
            stat=os.stat(self.dumpdbsav)
            if stat.st_size>0:
                pass
        except OSError:
            return False


        self._set_env()
        cmd="/usr/bin/psql -f "+self.dumpdbsav+" -h "+self.get_dbhost()+" "+dbname
        dest=" >/dev/null 2>/dev/null"

        fullcmd=cmd+dest
#        print fullcmd
        p1=subprocess.Popen(fullcmd,shell=True,env=os.environ)
        p1.wait()

        self._unset_env()

        if p1.returncode==0:
            ok=True                   
        else:
            ok=False


        if ok==False:
            print "Importing dump fail"

        return ok

    

    def dropdb(self,dbname):
        ok=False
        self._set_env()
        cmd="/usr/bin/dropdb -h "+self.get_dbhost()+" "+dbname
        dest=" 2>/dev/null "

        fullcmd=cmd+dest
        

        p1=subprocess.Popen(fullcmd,shell=True,env=os.environ)
        p1.wait()

        self._unset_env()

        if p1.returncode==0:
            ok=True
        return ok


    def createdb(self,dbname):
        ok=False
        self._set_env()
        cmd="/usr/bin/createdb -h "+self.get_dbhost()+" "+dbname
        dest=" 2>/dev/null"

        fullcmd=cmd+dest
#        print fullcmd
        p1=subprocess.Popen(fullcmd,shell=True,env=os.environ)
        p1.wait()

        self._unset_env()

        if p1.returncode==0:
            ok=True
        return ok




    def _set_env(self):
        os.environ['PGPASSWORD']=self.get_dbpasswd()
        os.environ['PGUSER']=self.get_dbuser()

    def _unset_env(self):
        os.environ['PGPASSWORD']=""
        os.environ['PGUSER']=""


    def copy(self):
        ok=True

        if (self.dbnamesrc==None or self.dbnamedst==None):
            print "database source and destination not defined"
            return False


        exist=self.is_exist(self.dbnamesrc)
        print "database "+self.dbnamesrc+ " exist ? : "+str(exist)

        if exist==False:
            print "can not reach the database : "+self.dbnamesrc
            return False
        

        dump=self.dump_all_sql(self.dbnamesrc)
        print "dump database "+self.dbnamesrc+" : "+str(dump)

        if dump==False:
            print "can not dump the db : "+self.dbnamesrc
            return False

        exist=self.is_exist(self.dbnamedst)
        if exist:
            print "drop database "+self.dbnamedst+" : "+str(exist)
            dropdb=self.dropdb(self.dbnamedst)
            if dropdb==False:
                print "can not drop the database : "+self.dbnamedst


        createdb=self.createdb(self.dbnamedst)
        print "create database "+self.dbnamedst+": "+str(createdb)+" "

        if createdb==False:
            print "can not create the db : "+self.dbnamedst
        
        importdb=self.import_all_sql(self.dbnamedst)
        print "import database "+self.dbnamesrc+ " in "+self.dbnamedst+" : "+str(importdb)

        if importdb==False:
            print "can not import the db"

        # The db should be copied
        return ok

        




if __name__ == '__main__':
    print "__main_test_unit__"

    md=manage_db_instance()
    md.set_dbnamesrc("peyrardj2110")
    md.set_dbnamedst("ipallocation")

    md.set_dbuser("clusterdb")
    md.set_dbpasswd("clusterdb")

    md.set_dbhost("10.5.0.1")

    if md.copy():
        print "copydb OK"
    else:
        print "copydb Failed"
        
    

