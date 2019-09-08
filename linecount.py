from __future__ import print_function
import os

total = 0
for file in os.listdir(os.getcwd()):
	count = 0
	last_dot = -1
	for i in range(len(file)):
		if file[i] == '.':
			last_dot = i
	if last_dot == -1:
		continue
	elif file[last_dot + 1:] != 'c' and file[last_dot+1:] != 'h':
		continue
	try:
		count = len(open(file).readlines())
	except Exception as e:
		pass
	total += count

print(total,'lines')
