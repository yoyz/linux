#!/usr/bin/python

import commands
import re
import sys


class IBFabric:
    def __init__(self):
        self.SL=SwitchList()
        self.NL=NodeList()
        self.IBCL=IBCableList()
        
        

class IBCableList:
    def __init__(self):
        self.ibcable_list=[]

    def add(self,ibcableToAdd):
        found=0
        for ibc in self.ibcable_list:
            if ibc.equal(ibcableToAdd):
                found=1
                break
        if (found==0):
            self.ibcable_list.append(ibcableToAdd)

    def __repr__(self):
        str=""
        i=0
        for ibc in self.ibcable_list:
            if i>0:
                str=str+"\n"
            i=i+1
            str=str+ibc.__repr__()
        return str


        
        
class IBCable:
    def __init__(self):
        fromPortNumber=None
        fromGuid=None
        toPortNumber=None
        toGuid=None



    def equal(self,ibcable):
        if (
                (self.fromGuid       == ibcable.fromGuid       and
                 self.toGuid         == ibcable.toGuid         )   or
                (self.fromGuid       == ibcable.toGuid         and
                 self.toGuid         == ibcable.fromGuid)):
            if (            
                    (self.fromPortNumber == ibcable.fromPortNumber and
                     self.toPortNumber   == ibcable.toPortNumber) or
                    (self.fromPortNumber == ibcable.toPortNumber   and
                     self.toPortNumber   == ibcable.fromPortNumber)):
                return True
            else:
                return False
        else:
            return False

    def __repr__(self):
        return "Cable fromPort:%.4d guid:%.16s toPort:%.4d guid:%.16s" % (self.fromPortNumber,
                                                                          (str(self.fromGuid)),
                                                                          self.toPortNumber,
                                                                          (str(self.toGuid)))
    
        
class SwitchList:
    def __init__(self):
        self.switch_list=[]

    def add(self,switchToAdd):
        found=0
        for sw in self.switch_list:
            if sw.equal(switchToAdd):
                found=1
                break
        if (found==0):
            self.switch_list.append(switchToAdd)


    def __repr__(self):
        str=""
        i=0
        for sw in self.switch_list:
            if i>0:
                str=str+"\n"
            i=i+1
            str=str+sw.__repr__()
        return str


class NodeList:
    def __init__(self):
        self.node_list=[]

    def add(self,nodeToAdd):
        found=0
        for nd in self.node_list:
            if sw.equal(nodeToAdd):
                found=1
                break
        if (found==0):
            self.node_list.append(nodeToAdd)


    def __repr__(self):
        str=""
        i=0
        for nd in self.node_list:
            if i>0:
                str=str+"\n"
            i=i+1
            str=str+nd.__repr__()
        return str


    
    
class Switch:
    def __init__(self):
        self.guid=None
        self.lid=None

    def setGuid(self,guid):
        self.guid=guid

    def setLid(self,lid):
        self.lid=lid

    def equal(self,sw):
        if (self.guid==sw.guid and
            self.lid==sw.lid):
            return True
        else:
            return False

    def __repr__(self):
        return "SWITCH guid:%.16s lid:%.8s" % (str(self.guid),str(self.lid))
        
    def printSwitch(self):
        print self
        #print self.__repr__()
        
        #print "guid:%.16s lid:%.8s\n" % (str(self.guid),str(self.lid))

class Node:
    def __init__(self):
        self.guid=None
        self.lid=None

    def setGuid(self,guid):
        self.guid=guid

    def setLid(self,lid):
        self.lid=lid

    def equal(self,nd):
        if (self.guid==nd.guid and
            self.lid==nd.lid):
            return True
        else:
            return False

    def __repr__(self):
        return "NODE guid:%.16s lid:%.8s" % (str(self.guid),str(self.lid))
        
    def printSwitch(self):
        print self
        #print self.__repr__()
        
        #print "guid:%.16s lid:%.8s\n" % (str(self.guid),str(self.lid))


        

if __name__ == '__main__':

    S1=Switch()
    S1.setGuid("0x24")
    S1.setLid("32")

    S2=Switch()
    S2.setGuid("0x25")
    S2.setLid("34")

    IBC1=IBCable()
    IBC2=IBCable()

    IBC1.fromPortNumber=1
    IBC1.toPortNumber=1
    IBC1.fromGuid="0x25"
    IBC1.toGuid="0x26"

    IBC2.fromPortNumber=1
    IBC2.toPortNumber=2
    IBC2.fromGuid="0x25"
    IBC2.toGuid="0x26"

    SL=SwitchList()
    IBCL=IBCableList()

    
    if (1):
        print S1.__repr__()
        print S2.__repr__()

        print S1.equal(S1)
        print "IBCable equal %s" % (str(IBC1.equal(IBC2)))
        #print IBC1

        IBCL.add(IBC1)
        IBCL.add(IBC1)
        IBCL.add(IBC2)
        print IBCL

    if (1):
        SL.add(S1)
        SL.add(S1)
        SL.add(S2)
        SL.add(S2)
        print SL
        
    #print "salut"
