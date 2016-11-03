#!/usr/bin/python

import commands
import re
import sys


class IBFabric:
    def __init__(self):
        self.sl=SwitchList()
        self.nl=NodeList()
        self.ibcl=IBCableList()
        

    def __repr__(self):
        return self.sl.__repr__()+"\n"+self.nl.__repr__()+"\n"+self.ibcl.__repr__()

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

    def setFromPortNumber(self,pn):
        self.fromPortNumber=pn

    def setToPortNumber(self,pn):
        self.toPortNumber=pn

    def setFromGuid(self,guid):
        self.fromGuid=guid

    def setToGuid(self,guid):
        self.toGuid=guid



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
        return "Cable fromPort:%.4d guid:%.20s toPort:%.4d guid:%.20s" % (self.fromPortNumber,
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
            if nd.equal(nodeToAdd):
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
        self.lid=-1
        self.name=None

    def setGuid(self,guid):
        self.guid=guid

    def setLid(self,lid):
        self.lid=lid

    def setName(self,name):
        self.name=name

    def getName(self):
        return self.name

    def getGuid(self):
        return self.guid

    def getLid(self):
        return self.lid
    
    def equal(self,sw):
        if (self.guid==sw.guid):
            return True
        else:
            return False

    def __repr__(self):
        return "SWITCH guid:%.20s lid:%.8s name:%.32s" % (str(self.guid),str(self.lid),str(self.name))
        
    def printSwitch(self):
        print self
        #print self.__repr__()
        
        #print "guid:%.16s lid:%.8s\n" % (str(self.guid),str(self.lid))

class Node:
    def __init__(self):
        self.guid=None
        self.lid=-1
        self.name=None
        
    def setGuid(self,guid):
        self.guid=guid

    def setName(self,name):
        self.name=name
        
    def setLid(self,lid):
        self.lid=lid

    def getLid(self):
        return self.lid

    def getGuid(self):
        return self.guid
        
    def equal(self,nd):
        if (self.guid==nd.guid):
            return True
        else:
            return False

    def __repr__(self):
        return "NODE guid:%.20s lid:%.8s name:%.32s"  % (str(self.guid),str(self.lid),str(self.name))
        
    def printSwitch(self):
        print self
        #print self.__repr__()
        
        #print "guid:%.16s lid:%.8s\n" % (str(self.guid),str(self.lid))

class EXPORT_IBLINKINFO:
    def __init__(self):
        self.ibf=IBFabric()

    def setibf(self,ibf):
        self.ibf=ibf
        
    def __repr__(self):
        #return str(self.ibf)
        string=""
        for sw in self.ibf.sl.switch_list:
            string=string+"Switch: "+sw.getGuid()+" "+sw.getName()+"\n"
            for switchport in range(1,36):
                switchLid=sw.getLid()
                #string=string+str("%d" % i)+"\n"
                string=string+str("%d" % switchport)+"\n"
        return string
        
        
class IMPORT_OPAEXTRACTSELLINKS:
    def __init__(self):
        self.loaded=0
        self.ibf=IBFabric()
    
    def loadFile(self,fileName):
        try:
            fp = open(fileName,'r')
            file_content = fp.readlines()
            # if   re.match('[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+',self.dict['host']):
            for line_content in file_content:
                fromGuid=None
                toGuid=None
                fromPort=None
                toPort=None
                fromType=None
                toType=None
                fromName=None
                toName=None
                #title_search = re.search('<title>(.*)</title>', html, re.IGNORECASE)
                #if title_search:
                #    title = title_search.group(1)
                opaextract_search=re.search('(.+);(.+);(.+);(.+);(.+);(.+);(.+);(.+)',line_content)
                if opaextract_search:
                    if (opaextract_search.group(3) == "FI"):
                        nd=Node()
                        nd.setGuid(opaextract_search.group(1))
                        nd.setName(opaextract_search.group(4))
                        self.ibf.nl.add(nd)

                    if (opaextract_search.group(3) == "SW"):
                        sw=Switch()
                        sw.setGuid(opaextract_search.group(1))
                        sw.setName(opaextract_search.group(4))
                        self.ibf.sl.add(sw)
                        
                    if (opaextract_search.group(7) == "FI"):
                        nd=Node()
                        nd.setGuid(opaextract_search.group(5))
                        nd.setName(opaextract_search.group(8))
                        self.ibf.nl.add(nd)

                    if (opaextract_search.group(3) == "SW"):
                        sw=Switch()
                        sw.setGuid(opaextract_search.group(5))
                        sw.setName(opaextract_search.group(8))
                        self.ibf.sl.add(sw)
                    
                    ibc=IBCable()
                    ibc.setFromGuid(opaextract_search.group(1))
                    ibc.setToGuid(  opaextract_search.group(5))
                    ibc.setFromPortNumber(int(opaextract_search.group(2)))
                    ibc.setToPortNumber(  int(opaextract_search.group(6)))
                    self.ibf.ibcl.add(ibc)
                    
                #print self.ibf
    
 
            fp.close
        except (OSError, IOError), (errno, strerror):
            sys.stderr.write("File %s error %s : %s\n" %(fileName, errno, strerror))
            sys.exit(1)


if __name__ == '__main__':

    IOPAE=IMPORT_OPAEXTRACTSELLINKS()
    EIBLI=EXPORT_IBLINKINFO()
    
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

    
    if (0):
        print S1.__repr__()
        print S2.__repr__()

        print S1.equal(S1)
        print "IBCable equal %s" % (str(IBC1.equal(IBC2)))
        #print IBC1

        IBCL.add(IBC1)
        IBCL.add(IBC1)
        IBCL.add(IBC2)
        print IBCL

    if (0):
        SL.add(S1)
        SL.add(S1)
        SL.add(S2)
        SL.add(S2)
        print SL

    if (1):
        IOPAE.loadFile("opaextractsellinks.txt")
        #print IOPAE.ibf
        EIBLI.setibf(IOPAE.ibf)
        print EIBLI
    #print "salut"
