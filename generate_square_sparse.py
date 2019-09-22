import sys
import os
from random import random

if len(sys.argv) < 2:
	print('expected power of 2 as argument')
	sys.exit()
try:
	power = int(sys.argv[1])
except ValueError:
	print('could not convert %s to integer' % sys.argv[i])
	sys.exit()
if power > 14:
	print('%d outside of bounds')
	sys.exit

if len(sys.argv) < 3:
	print('expected type as argument')
	sys.exit()
isfloat = False
if 'float' in sys.argv[2].lower():
	isfloat = True
elif 'int' not in sys.argv[2].lower():
	print('could not assign type to argument %s' % sys.argv[2])
	sys.exit()

filename = 'untitledmatrix.in'
files = os.listdir(os.getcwd())
i = 1
while filename in files:
	filename = "untitledmatrix (%d).in" % i
	i += 1
if len(sys.argv) > 3:
	filename = sys.argv[3]

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

dim = pow(2, power)
with open(filename, 'w') as fp:
	print('float' if isfloat else 'int', file=fp)
	print(dim, file=fp)
	print(dim, file=fp)
	for i in range(dim*dim):
		if(random() >= prob):
			print('0',file=fp, end='')
		else:
			print('%f' % (random() * 10000) if isfloat else '%d' % int(random() * 10000), file=fp, end='')
		if i < (dim*dim - 1):
			print(' ', file=fp, end='')

print('generated %dx%d %s matrix in %s' % (dim, dim, 'float' if isfloat else 'int', filename))

