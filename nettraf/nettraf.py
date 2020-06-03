#!/usr/bin/python2

import commands
import re
import sys
import os.path
from os import path
import time
from os import listdir
from os.path import isfile, join


sci="/sys/class/infiniband/"
scn="/sys/class/net/"

intNameFound=[ ]
netIntList=[ ]

class net_int_obj:
    def __init__(self):
        self.intName=None
        self.portNumber=0
        self.rcv_data_0=0   # rcv  data in 2 buffer to do bw = (b1-b0)/time
        self.rcv_data_1=0   #
        self.xmit_data_0=0  # xmit data in 2 buffer to do bw = (b1-b0)/time
        self.xmit_data_1=0  # 
        self.ib=0           # it's an infiniband mlx/opa card
        self.ip=0           # it's an interface on the system, so can be visible by "ip a"/"ifconfig" etc.


    # define this interface as an ipv4 needed for updatePortState
    def setIP(self):
        self.ip=1
        self.ib=0

    # define this interface as an infiniband needed for updatePortState
    def setIB(self):
        self.ib=1
        self.ip=0



    def setName(self,name):
        self.intName=name

    def setPort(self,portNumber):
        self.portNumber=portNumber

    def updatePortState(self):
        if self.ib:
            self.updatePortState_ib()
        if self.ip:
            self.updatePortState_ip()

    # https://community.mellanox.com/s/article/understanding-mlx5-linux-counters-and-status-parameters
    # port_rcv_data Total number of data octets, divided by 4 (lanes), received on all VLs. This is 64 bit counter.  PortRcvData Informative
    # port_xmit_data Total number of data octets, divided by 4 (lanes), transmitted on all VLs. This is 64 bit counter. PortXmitData	Informative

    def updatePortState_ib(self):
        # rotate counter
        self.rcv_data_0=self.rcv_data_1
        self.xmit_data_0=self.xmit_data_1

        # read counter, and multiply them by 4 assuming we use 4 lane which should be the case if we are on SDR/QDR/FDR/EDR, need to check on HDR
        f=open(sci+"/"+self.intName+"/ports/"+str(self.portNumber)+"/counters/port_rcv_data")
        self.rcv_data_1=int(f.readline())*4
        f.close()

        f=open(sci+"/"+self.intName+"/ports/"+str(self.portNumber)+"/counters/port_xmit_data")    
        self.xmit_data_1=int(f.readline())*4
        f.close()
        
    def updatePortState_ip(self):
        # rotate counter
        self.rcv_data_0=self.rcv_data_1
        self.xmit_data_0=self.xmit_data_1
        f=open(scn+"/"+self.intName+"/statistics/rx_bytes")
        self.rcv_data_1=int(f.readline())
        f.close()

        f=open(scn+"/"+self.intName+"/statistics/tx_bytes")
        #out=commands.getoutput("cat "+scn+"/"+self.intName+"/statistics/tx_bytes")
        self.xmit_data_1=int(f.readline())
        f.close




    # print one line of performance status for this interface
    def printMe(self):
        print       "%10s %5s %2s %10s %8s %10s %9s" % (
                    self.intName,
                    "port",
                    str(self.portNumber),
                    " xmit MiB/s ",
                    str((((self.xmit_data_1-self.xmit_data_0)/1024)/1024)),
                     " rcv MiB/s ",
                    str((( self.rcv_data_1 - self.rcv_data_0 ) /1024 ) /1024 ))

if __name__ == '__main__':

    if ( path.exists(sci) ):
        # fetch the interface in /sys/class/infiniband
        for intName in listdir(sci):
            if ( path.exists(sci+"/"+intName+"/ports/1/") ):
                ni = net_int_obj()
                ni.setName(intName)
                ni.setPort(1)
                ni.setIB()
                netIntList.append(ni)

            if ( path.exists(sci+"/"+intName+"/ports/2/") ):
                ni = net_int_obj()
                ni.setName(intName)
                ni.setPort(2)
                ni.setIB()
                netIntList.append(ni)

    if ( path.exists(scn) ):
        # fetch the interface in /sys/class/net/
        for intName in listdir(scn):
            ni = net_int_obj()
            ni.setName(intName)
            ni.setPort(1)
            ni.setIP()
            netIntList.append(ni)

        for netInt in netIntList:
            netInt.updatePortState()

    if len(netIntList)==0:
        print "No interface found, exiting"
        sys.exit(1)
        
    # display the port state in a loop
    while True:
        time.sleep(1)
        for netInt in netIntList:
            netInt.updatePortState()
            netInt.printMe()
        print ""



