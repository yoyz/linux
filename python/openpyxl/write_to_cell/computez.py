#!/usr/bin/python
#!/home_nfs/peyrardj/anaconda2/bin/python
#!~/anaconda2/bin/python
#!/usr/bin/python

from openpyxl import Workbook
import sys
import time

book = Workbook()
sheet = book.active

a=1
b=2
c=24
sheet.cell(row=a,column=b,value=c)

book.save("sample.xlsx")
