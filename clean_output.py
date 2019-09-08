from __future__ import print_function
import os

filecount = 0
for file in os.listdir(os.getcwd()):
	last_dot = -1
	for i in range(len(file)):
		if file[i] == '.':
			last_dot = i
	if last_dot == -1:
		continue
	try:
		if file[last_dot + 1:] == 'out':
			os.remove(file)
			filecount += 1
	except:
		pass

print('removed %d files' % filecount)
