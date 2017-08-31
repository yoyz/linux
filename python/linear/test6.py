#!/usr/bin/python
import sys
import re
import numpy as np
import matplotlib.pyplot as plt
#import numpy.polynomial.polynomial as poly
from numpy.polynomial import Polynomial


array = []
#f=open('file')
f=open(sys.argv[1])
for line in f:
	line=re.sub(',',r'.',line)
	elem=line.split()
	#for elem in line.split:
	#print elem
	array.append(elem)

#exit
array[0] = map(float, array[0]) 
array[1] = map(float, array[1]) 
x = array[0]
#x = x.astype(np.float)
y = array[1]

print x
print y
#x.map(float, list_of_strings)  
#x1 = x.astype(np.float)


#z = np.polyfit(x, y, 3)

p = Polynomial.fit(x, y, 4)
print p
plt.plot(*p.linspace())
plt.xlim([x[0]-1, x[-1] + 1 ])

plt.show()
