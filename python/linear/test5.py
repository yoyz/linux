#!/usr/bin/python
import sys
import re
import numpy as np
import matplotlib.pyplot as plt

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
z = np.polyfit(x, y, 2)
f = np.poly1d(z)

# calculate new x's and y's
x_new = np.linspace(x[0], x[-1], 50)
y_new = f(x_new)

plt.plot(x,y,'o', x_new, y_new)
plt.xlim([x[0]-1, x[-1] + 1 ])
plt.show()

