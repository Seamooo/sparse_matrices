from __future__ import print_function
import sys
import os
from random import random

#usage python generate_square_sparse <rows> <cols> <type> [filename] [probability] [max_elem_val]

if len(sys.argv) < 2:
	print('expected power of 2 as argument')
	sys.exit()
try:
	rows = int(sys.argv[1])
except ValueError:
	print('could not convert %s to integer' % sys.argv[1])
	sys.exit()
if not (0 < rows <= 16384):
	print('%d outside of bounds')
	sys.exit

if len(sys.argv) < 3:
	print('expected power of 2 as argument')
	sys.exit()
try:
	cols = int(sys.argv[2])
except ValueError:
	print('could not convert %s to integer' % sys.argv[2])
	sys.exit()
if not (0 < cols <= 16384):
	print('%d outside of bounds')
	sys.exit

if len(sys.argv) < 4:
	print('expected type as argument')
	sys.exit()
isfloat = False
if 'float' in sys.argv[3].lower():
	isfloat = True
elif 'int' not in sys.argv[3].lower():
	print('could not assign type to argument %s' % sys.argv[3])
	sys.exit()

fileinfo = '%s%dx%d' % ('float' if isfloat else 'int', rows, cols)
filename = '%s.in' % fileinfo
files = os.listdir(os.getcwd())
i = 1
while filename in files:
	filename = '%s (%d).in' % (fileinfo, i)
	i += 1

prob = 0.1
if len(sys.argv) > 4:
	try:
		temp = float(sys.argv[4])
		if not (0. <= temp <= 1.):
			raise Exception()
		prob = temp
	except Exception:
		print('ignoring probability val: %s' % sys.argv[4])

maxval = 10000
if len(sys.argv) > 5:
	try:
		temp = int(sys.argv[5])
		if maxval > 1000000:
			raise Exception
		maxval = temp
	except Exception:
		print('ignoring range val: %s' % sys.argv[5])

with open(filename, 'w') as fp:
	print('float' if isfloat else 'int', file=fp)
	print(rows, file=fp)
	print(cols, file=fp)
	for i in range(rows*cols):
		if(random() >= prob):
			print('0',file=fp, end='')
		else:
			print('%f' % (random() * 10000) if isfloat else '%d' % int(random() * 10000), file=fp, end='')
		if i < (rows*cols - 1):
			print(' ', file=fp, end='')

print('generated %dx%d %s matrix in %s' % (rows, cols, 'float' if isfloat else 'int', filename))

