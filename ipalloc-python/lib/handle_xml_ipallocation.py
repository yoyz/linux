#!/usr/bin/python

import sys,getopt,re,os,sys
from   xml.etree.ElementTree                            import ElementTree,Element,SubElement
import xml.parsers.expat

from   CLM.DB.ipallocation.error_code                   import error_code
from   CLM.DB.ipallocation.rule                         import rule,rule_list
from   CLM.DB.ipallocation.xml_element                  import networkXML,memberXML,rangeXML

class handle_xml_ipallocation:
    """
    Load a xml file
    Create a 'rule_list' object composed of 'rule' object.
    Each rule represent a 'ip_allocation' file.
    """

    def __init__(self):
        self.filename=None
        self.rule_list=rule_list()
        self.v=False

    def set_verbose(self,verbose):
        self.v=verbose

    def printv(self,str):
        if self.v:
            print str

    def set_filename(self,filename):
        """
        Public method

        Set the path of the filename of the xml file

        Return code :
        if the file doesn't exist return False
        """
        ok=False
        if os.path.exists(filename):
            self.filename=filename
            ok=True
        return ok
 

    def handle_member(self,node,net,lnet,rng):
        """
        Private method
        called by 'handle_range'
        """
        dict=node.attrib
        mb=memberXML()
        for att,value in dict.iteritems():
            if att=="object":
                mb.set_object(value)
            if att=="type":
                mb.set_type(value)
            if att=="group":
                mb.set_group(value)
            if att=="interface_name":
                mb.set_iname(value)
                
        r=rule()
        
        if lnet.get_function()==None:
            lnet.set_function(net.get_function())

        r.set_network(net)
        r.set_logical_network(lnet)
        r.set_range(rng)
        r.set_member(mb)
        r.set_fromXML()
        if r.check_attribute_loaded():
            self.rule_list.add_rule(r)
        else:
            err=error_code()
            err.xml_rule_not_completed("load_xml_ip_alloc.handle_member()")


    def handle_range(self,node,net,lnet):
        """
        Private method
        called by 'handle_logical_network'
        """
        dict=node.attrib
        rng=rangeXML()
        for att,value in dict.iteritems():
            if att=="first_ip":
                rng.set_first_ip(value)

        i=0
        for elem in node:
            i=i+1
            new_net=net.clone_nw()
            new_lnet=lnet.clone_nw()
            new_rng=rng.clone_range()
            if elem.tag=="member":
                self.handle_member(elem,new_net,new_lnet,new_rng)

#        for elem in node:            
#            if elem.tag=="member":
                #self.handle_member(node,net,rng)
               

    def handle_logical_network(self,node,net):
        """
        Private method, called by 'handle_all'
        """
        dict=node.attrib
        lnet=networkXML()
        
        lnet.XMLparent_name=net.XMLname
        lnet.set_is_a_logical_network()

        for att,value in dict.iteritems():
            if att=="name":
                lnet.set_name(value)
            if att=="gateway":
                lnet.set_gateway(value)
            if att=="vlan_id":
                lnet.set_vlan_id(value)
            if att=="pkey":
                lnet.set_pkey(value)
            if att=="function":
                lnet.set_function(value)
            if att=="subnet":
                lnet.set_subnet(value)
            if att=="parent_name":
                lnet.set_parent_name(value)
            if att=="type":
                lnet.set_type(value)
            if att=="suffix":
                lnet.set_suffix(value)

        for elem in node:
            lnet=lnet.clone_nw()
            if elem.tag=="range":
                self.handle_range(elem,net,lnet)
            elif elem.tag=="member":
                rng=rangeXML()
                self.handle_member(elem,net,lnet,rng)


    def handle_network(self,node):
        """
        Private method, called by 'handle_all'
        """
        dict=node.attrib
        net=networkXML()
        net.set_is_a_network()

        for att,value in dict.iteritems():
            if att=="name":
                net.set_name(value)
            if att=="function":
                net.set_function(value)

        for elem in node:
            new_net=net.clone_nw()
            if elem.tag=="logical_network":
                self.handle_logical_network(elem,new_net)


    def handle_all(self,node,space):
        """
        Private method

        Handle all the xml tree object use 'xml.etree.ElementTree'        
        
        """
        if (node.tag=="network"):
            self.handle_network(node)
        

    def handle_root(self,tree):
        for elem in tree.getroot():
            self.handle_all(elem,"")


    def load_xml(self):
        """
        Public method

        Use it to load a xml file which match the 'ip_allocation.dtd'

        Return code :
        True if there is a list of rule loaded
        """
        ok=False
        if self.filename==None:        
            err=error_code()
            err.xml_file_not_found() # exit the program
	    sys.exit(1)

	try:
	        f = open(self.filename, 'r')
	        str=f.readline()
        	f.close()
	except:
		print("ERROR: the network map file is not found or not readable.")
		sys.exit(1)

        r=re.compile(".+DOCTYPE ip_allocation.+")
        if (not r.match(str)):
            err=error_code()
            err.xml_file_not_correct()
            err.sysexit()

	try:
	        tree=ElementTree(file=self.filename)
	except xml.parsers.expat.ExpatError as parse_err:
		print("ERROR: please check the consistency of your XML in %s.\nThe XML parsing error occurs here: %s." % (self.filename, parse_err))
		sys.exit(1)
        self.handle_root(tree)
        if self.rule_list.number_of_rule()>0:
            ok=True

        return ok


    def check_rules(self):
        """
        Public method

        Call a basic check on the rule list
        """
        return self.rule_list.check_rules()
            
    def get_rules(self):
        return self.rule_list


if __name__ == '__main__':
    print "__main__ xml_ip_alloc"
    filename=""
    if len(sys.argv) == 2:
        if os.path.exists(sys.argv[1]):
            filename=sys.argv[1]
        else:
            print("File not found "+sys.argv[1])
            sys.exit(1)
    else:
        print("Usage : "+sys.argv[0]+" <xmlfile>")
        sys.exit(1)
    ipalloc=handle_xml_ipallocation()
    ipalloc.set_filename(filename)
    ipalloc.load_xml()
    ipalloc.check_rules()
    print(ipalloc.get_rules())

