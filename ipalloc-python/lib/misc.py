

def intersect_list(l1,l2):
    l3=[]
    for i in l1:
        for j in l2:
            if (i==j):
                if (j not in l3):                        
                    l3.append(i)
    return l3


def try_int(s):
    "Convert to integer if possible."
    try: return int(s)
    except: return s

def natsort_key(s):
    "Used internally to get a tuple by which s is sorted."
    import re
    return map(try_int, re.findall(r'(\d+|\D+)', s))

def natcmp(a, b):
    "Natural string comparison, case sensitive."
    return cmp(natsort_key(a), natsort_key(b))

def natcasecmp(a, b):
    "Natural string comparison, ignores case."
    return natcmp(a.lower(), b.lower())

def natsort(seq, cmp=natcmp):
    "In-place natural string sort."
    seq.sort(cmp)
    
def natsorted(seq, cmp=natcmp):
    "Returns a copy of seq, sorted by natural string sort."
    import copy
    temp = copy.copy(seq)
    natsort(temp, cmp)
    return temp

# If filename already exists, the content will be appended.
def logfile(strlist):
	""" Log the message to a file """
	import os
	try:
		filename = "/tmp/ipallocation.log"
		if os.path.exists(filename):
			mode = "a"
		else:
			mode = "w"
		FILE = open(filename, mode)
		for line in strlist:
			FILE.write(line+"\n")
		FILE.close()
	except:
		print("Can't write the log file %s!\n" % (filename))
		for line in strlist:
			print(line+"\n")

def delete_logfile():
	import os
	try:
		filename = "/tmp/ipallocation.log"
		if os.path.exists(filename):
			os.remove(filename)
	except:
		print("Can't remove the file %s!\n" % (filename))


def do_md5(data, size=0):
	try:
		import hashlib
		m = hashlib.md5()
		m.update(data)
		digest = m.hexdigest()
		if (size):
			return digest[0:size]
		return digest
	except:
		import sys
		print("ERROR: hashlib.md5() does not seem to be supported. Please check if your python version is >= 2.5.")
		sys.exit(1)

