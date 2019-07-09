#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sqlite3 as lite
import sys

con = None

try:
    con = lite.connect('test.db')
    cur = con.cursor()
    cur.execute("SELECT * FROM changelog")
    rows = cur.fetchall()
    for row in rows:
        print row


except lite.Error, e:

    print "Error {}:".format(e.args[0])
    sys.exit(1)

finally:

    if con:
        con.close()
