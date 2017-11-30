#!/home_nfs/peyrardj/anaconda2/bin/python
#!~/anaconda2/bin/python
#!/usr/bin/python

from openpyxl import Workbook
from openpyxl import load_workbook
import os
import sys
import time

def printf(format, *args):
    sys.stdout.write(format % args)

book = load_workbook("sample.xlsx")
sheet = book.active
mystr=""
for c in range(1,10):
        for r in range(1,10):
                mystr=str(sheet.cell(row=r,column=c).value)
                mystr=mystr.rstrip(os.linesep)
                printf("%8s ",mystr);
        printf("\n")
