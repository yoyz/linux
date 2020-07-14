#!/usr/bin/python
import sys
import re
import getopt
import os
from pathlib import Path


def iteratOverDir(path,content):
    for file_dir in path.iterdir():
        if file_dir.is_dir():
            try:
                iteratOverDir(file_dir,content)
            except OSError:
                print("too deep "+str(file_dir))
        else:
            #print(str(file_dir))
            for line in content:
                if line in str(file_dir):
                    print("removing "+str(file_dir))
                    os.unlink(file_dir)
                    break
        

filelist="/tmp/list"
            
with open(filelist) as fin:
    content = fin.readlines()
content = [x.strip() for x in content]

#for line in content:
#    print("["+line+"]")

p = Path('/home/yoyz/slowtmp')
iteratOverDir(p,content)
