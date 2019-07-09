#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import commands
import re
import sys
import getopt

import sqlite3 as lite
import sys

con = None

try:
    con = lite.connect('test.db')
    cur = con.cursor()
    f = open("changelog1","r")
    p=re.compile(r'([0-9]+)\ (.+)')
    for line in f:

        _id=p.search(line).group(1)
        _chlg=p.search(line).group(2)
        cur.execute('insert or ignore into changelog values('+_id+',"'+_chlg+'");')
    
    con.commit()
    #cur.execute("SELECT * FROM changelog")
    #rows = cur.fetchall()

    #for row in rows:
    #    print row
except lite.Error, e:

    print "Error {}:".format(e.args[0])
    sys.exit(1)

finally:

    if con:
        con.close()
