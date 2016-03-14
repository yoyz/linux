#!/usr/bin/python

from   CLM.DB.ipallocation.ipaddr                       import ipaddr,ip_set,network
import sys,getopt,re,os

class ip_range:


    def __init__(self):
#        self.ips=ipaddr.ip_set()
#        self.ips=ipaddr()
#        self.ips.ip_set()
        self.ips=ip_set()
        self.cursor=0

        self.cursor_first_ip=None
        self.cursor_last_ip=None

        self.size=-1
        self.set=[]
        self.currentIp=None
        self.barrierSet=False
        self.ipBarrier=None
        self.imported=False
        self.saferange=False

        self.first_ip=None
        self.last_ip=None
        return None
    

    def setParanoid(self):
        self.saferange=True



    def setNetworkIPAddress(self,net):
        """
        Need :
          'net' must have this form 'a.b.c.d/xy'

        Implement : 
          set the network of the range
          the range has to be written this way for example : '10.1.2.0/24'
          For more information read 'pydoc ipaddr.py'
        """

        if net==None:
            print "setNetworkIPAddress(net) called with net == None"
            sys.exit(1)
        if net==None:
            print "setNetworkIPAddress(net) : net == None "
            print "There is an error in network subnet, subnet == None"
        lst=net.split("/")
        self.size=-1
#        self.ips=ipaddr.ip_set()
        self.ips=ip_set()
        self.ips.add_network(network(lst[0],lst[1]))
        size=self.__importSet()
        if size>0:
            return True
        else:
            return False


    def old__importSet(self):
        """
        Private method used by setNetworkIPAddress
        """

        self.imported=True
        self.size=self.ips.size()
        self.set=self.ips.as_list_of_ipaddr()
        #print self.set
        sys.exit(0)
        if self.saferange:
            r1=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.254$")
            r2=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.255$")
            r3=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.0$")
            newset=[]
            for ip in self.set:
                if ((r1.match(str(ip)) or
                    r2.match(str(ip)) or
                    r3.match(str(ip)))==None):
                    newset.append(ip)
            self.set=newset            
        self.cursor=0
        return self.size

    def __importSet(self):
        """
        Private method used by setNetworkIPAddress
        """

        self.imported=True
        self.size=0

#        r254=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.254$")
#        r255=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.255$")
#        r0=re.compile("[0-9]+\.[0-9]+\.[0-9]+\.0$")

        r254=re.compile(".+\.254$")
        r255=re.compile(".+\.255$")
        r0=re.compile(".+\.0$")

        for ip in self.ips.as_list_of_ipaddr():
            self.set.append(str(ip))
            #print str(ip)
        #print self.set
        #sys.exit(0)

        newset=[]        
        for ip in self.set:
            if self.saferange:
                if ((r255.match(ip) or
                     r254.match(ip) or
                     r0.match(ip))==None):
                    newset.append(ip)
                    self.size=self.size+1
            else:
                newset.append(ip)
                self.size=self.size+1
        self.set=newset
        self.cursor=0
        self.cursor_first_ip=0
        self.cursor_last_ip=self.size-1
        return self.size


    def numberOfElement(self):
        return self.cursor_last_ip-self.cursor_first_ip


    def numberOfAvailableElement(self):
	return self.cursor_last_ip - self.cursor
        #noae=(self.cursor_last_ip-self.cursor_first_ip)-(self.cursor-self.cursor_first_ip)
        #return noae

    def contains(self,ip):

        ip_to_check=ipaddr(ip)
        ok=False

        # ip_to_check must be >= first_ip
        if self.first_ip!=None:
            if ip_to_check < self.first_ip :
                return False

        # ip_to_check must be <= last_ip
        if self.last_ip!=None:
            if  ip_to_check > self.last_ip:
                return False
            
        return self.ips.contains(ip_to_check)
    

    def setFirstAddress(self,ip):
        """
        Need :
          setNetworkIPAddress('10.0.1.0/24') has been done
        
        Implement : 
          Set the first ip in the range

        Return:
          True if the ip is in the network and move the cursor
          False 
        """

        setted=False
        firstAddress=ipaddr(ip)
        self.first_ip=firstAddress

        if (self.ips.contains(firstAddress)==False):
            return setted

        my_cursor=0
        for ipsearch in self.set:
            if (ip  == ipsearch):
                self.cursor=my_cursor                
                self.cursor_first_ip=my_cursor
                setted=True
                break
            else:
                my_cursor=my_cursor+1
        return setted


    def setLastAddress(self,ip):
        """
        Need :
          setNetworkIPAddress('10.0.1.0/24') has been done
        
        Implement : 
          Set the first ip in the range

        Return:
          True if the ip is in the network and move the cursor
          False 
        """

        setted=False
        lastAddress=ipaddr(ip)
        self.last_ip=lastAddress
        #print self.ips.contains(lastAddress)
        if (self.ips.contains(lastAddress)==False):
            return setted

        my_cursor=0
        for ipsearch in self.set:
            if (ip  == ipsearch):
                setted=True
                self.cursor_last_ip=my_cursor
                self.barrierSet=True
                break
            else:
                my_cursor=my_cursor+1

        return setted

        

    
    def nextIpIsAvailable(self):
        #print self.numberOfAvailableElement()
        if (self.numberOfAvailableElement()>0):
            return True
        else:
            return False


    def getNextIp(self):
        if (self.nextIpIsAvailable()==False):
            return False
        ip=self.set[self.cursor]
        self.cursor=self.cursor+1
        return ip



    def getCurrentIp(self):
        return self.set[self.cursor]
    

    def str_info(self):
        mystr=""
        mystr=mystr+"Range Object\n"
        mystr=mystr+"============\n"
        mystr=mystr+"\n"
        mystr=mystr+"Network        \t"+self.ips.as_str_range()+"\n"
        mystr=mystr+"currentIp      \t"+self.set[self.cursor].ip_str()+"\n"
        mystr=mystr+"cursor         \t"+str(self.cursor)+"\n"
        mystr=mystr+"cursorBarrier  \t"+str(self.cursorBarrier)+"\n"
        mystr=mystr+"NextIpAvailable\t"+str(self.nextIpIsAvailable())+"\n"
        return mystr


    def check_range(self):
        return True
    

    def __repr__(self):
        return "<IPRange ('%s')" % self.ips.as_str_range()

################################################################################
################################### Unit Test ##################################
################################################################################

if __name__ == '__main__':    
    ipr=ip_range()
    ipr.setParanoid()
    print "setnetwork"
    print ipr.setNetworkIPAddress("10.0.0.0/16")
    print "set first ip"
    #print ipr.setFirstAddress("10.0.0.22")
    print ipr.setFirstAddress("10.0.0.40")
    print ipr.setLastAddress("10.0.1.25")
    print "Loading Done"
    print "contain"
    print ipr.contains("10.0.0.46")
    #while ipr.nextIpIsAvailable():
      #print ipr.numberOfAvailableElement()
      #print ipr.getNextIp()


