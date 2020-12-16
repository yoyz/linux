#!/usr/bin/env python3.6
#  author peyrard.johann@gmail.com
from __future__ import print_function
import re
import sys
import getopt
import time
from   os import path, listdir
import os
import signal

sci="/sys/class/infiniband/"
scn="/sys/class/net/"

useFloat=0                 # usefull if we want to see above 1MB/s or for example 0.4MiB/s
delay=1                    # wait how many second between each display
showOnlyIB=0
showOnlyIP=0
preffered_printMeColumn=1
preffered_printMeRow=0
nb_columns=0
nb_rows=0
################################################################################
#  net_int_obj : network interface object, which store name and counter
################################################################################

class net_int_obj:
    def __init__(self):
        self.intName=None   # interface name like eth0 mlx4_0 etc.
        self.portNumber=0   # the port number only usefull on infiniband which can have two port like this /sys/class/infiniband/mlx4_0/ports/2/
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
        self.rcv_data_1=float(f.readline())*4
        f.close()

        f=open(sci+"/"+self.intName+"/ports/"+str(self.portNumber)+"/counters/port_xmit_data")    
        self.xmit_data_1=float(f.readline())*4
        f.close()
        
    def updatePortState_ip(self):
        # rotate counter
        self.rcv_data_0=self.rcv_data_1
        self.xmit_data_0=self.xmit_data_1
        try:
            f=open(scn+"/"+self.intName+"/statistics/rx_bytes")
            self.rcv_data_1=float(f.readline())
            f.close()

            f=open(scn+"/"+self.intName+"/statistics/tx_bytes")
            self.xmit_data_1=float(f.readline())
            f.close
        except:
            pass
        


    def printMe(self):
        if preffered_printMeRow==1:
            self.printMeRow()
        if preffered_printMeColumn==1:
            self.printMeColumn()


    # print one block of line of performance status for this interface
    def printMeRow(self):
        if useFloat==0:
            print(       "%10s %2s %9d %9d " % (
                    self.intName,
                    str(self.portNumber),
                    ((((int(self.xmit_data_1 - self.xmit_data_0)) / 1024.0 ) / 1024.0 )/delay),
                    ((( int(self.rcv_data_1  - self.rcv_data_0))  / 1024.0 ) / 1024.0 )/delay),
                    end='')
        if useFloat==1:
            print(       "%10s %2s %9.3f %9.3f " % (
                    self.intName,
                    str(self.portNumber),
                    ((((float(self.xmit_data_1 - self.xmit_data_0)) / 1024.0 ) / 1024.0 /delay)),
                    ((( float(self.rcv_data_1  - self.rcv_data_0))  / 1024.0 ) / 1024.0 )/delay),
                    end='')
            
    # print one line of performance status for this interface
    def printMeColumn(self):
        if useFloat==1:
            print(       "%10s %5s %2s %10s %9.3f %10s %9.3f" % (
                    self.intName,
                    "port",
                    str(self.portNumber),
                    " xmit MiB/s ",
                    ((((float(self.xmit_data_1 - self.xmit_data_0)) / 1024.0 ) / 1024.0 /delay)),
                     " rcv MiB/s ",
                    ((( float(self.rcv_data_1  - self.rcv_data_0))  / 1024.0 ) / 1024.0 )/delay))
        if useFloat==0:
            print(       "%10s %5s %2s %10s %9d %10s %9d" % (
                    self.intName,
                    "port",
                    str(self.portNumber),
                    " xmit MiB/s ",
                    ((((int(self.xmit_data_1 - self.xmit_data_0)) / 1024.0 ) / 1024.0 )/delay),
                     " rcv MiB/s ",
                    ((( int(self.rcv_data_1  - self.rcv_data_0))  / 1024.0 ) / 1024.0 )/delay))


################################################################################
#  main and simple function
################################################################################
            
def usage():
    print( "nettraf.py [-f|-h|--ib|--ip|--row|--column] [ -d X ]")
    print( "            -h : help")
    print( "            -f : use float")
    print( "            -d : wait a number of second between each display 1 by default")
    print( "          --ib : only display the infiniband interface")
    print( "          --ip : only display the ip interface")
    print( "      --column : display in column ( default )")
    print( "         --row : display in row ")

def handler(signum, frame):
    sys.exit(0)
            
if __name__ == '__main__':

    netIntList=[ ]
    signal.signal(signal.SIGINT, handler)
    #nb_columns, nb_rows = os.get_terminal_size(0)
    #print("Your terminal's width is: %d %d" % ( columns, rows))
    # argument parsing
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hfd:", [ "ib", "ip", "row", "column" ] )
    except getopt.GetoptError as err:
        usage()
        sys.exit(2)

    for o, a in opts:
        if o == "-f":
            useFloat = True
        if o == "-h":
            usage()
            sys.exit(0)
        if o == "-d":
            delay=int(a)
        if o == "--ib":
            showOnlyIB=1
        if o == "--ip":
            showOnlyIP=1
        if o == "--row":
            preffered_printMeRow=1
            preffered_printMeColumn=0
        if o == "--column":
            preffered_printMeRow=0
            preffered_printMeColumn=1

    if preffered_printMeColumn==0 and preffered_printMeRow==0:
       preffered_printMeColumn=1

    if ( path.exists(sci) and showOnlyIP!=1 ):
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

    if ( path.exists(scn) and showOnlyIB!=1):
        # fetch the interface in /sys/class/net/
        for intName in listdir(scn):
            ni = net_int_obj()
            ni.setName(intName)
            ni.setPort(1)
            ni.setIP()
            netIntList.append(ni)
        
    if len(netIntList)==0:
        print("No interface found, exiting")
        sys.exit(1)
        
    # display the port state in a loop
    i=0

    for netInt in netIntList:
        netInt.updatePortState()
        netInt.updatePortState()

    
    while True:
        if i==0 and preffered_printMeRow==1:
            print("=interface=port=====xmit=====recv=")

        for netInt in netIntList:
            netInt.updatePortState()
            netInt.printMe()
        print("")
        time.sleep(delay)        
        i=i+1
