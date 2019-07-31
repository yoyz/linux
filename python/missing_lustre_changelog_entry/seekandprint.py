#!/usr/bin/python
import sys
import re

fname=sys.argv[1]
re_tag=re.compile(r'^([0-9]+)')
current_id=-1
last_id=-1
fault=-1
last_l=""
with open(fname) as fin:
    content = fin.readlines()    
content = [x.strip() for x in content] 
for l in content:
    if re_tag.search(l):
        #print re_tag.search(l).group(1)
        current_id=int(re_tag.search(l).group(1))
        if (last_id+1)!=current_id:
            fault=fault+1
        if fault>=1:
            print "current_id: "+str(current_id)+" last_id: "+str(last_id)
            print "LAST: "+last_l
            print "CURR: "+l
            print ""
            last_id=current_id
            fault=fault-1
        last_id=current_id
        last_l=l
