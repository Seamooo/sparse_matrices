import sys

if len(sys.argv) < 2:
	print('expected file')
	sys.exit()

out = open('%s.make' % sys.argv[1], 'w')

with open(sys.argv[1], 'r') as fp:
	lines = fp.readlines()
	lines = list(map(lambda x: x.rstrip('\n'), lines))
	print('%s:' % sys.argv[1], file=out)
	firstline = True
	for i in range(len(lines)):
		newline = []
		for c in lines[i]:
			if c == '"':
				newline.append('\\"')
			elif c == '$':
				newline.append('\\$$')
			else:
				newline.append(c)
		newline = ''.join(newline)
		for i in range(0,len(newline), 80):
			if firstline:
				firstline = False
				print('\t@printf "%s%s" > %s' % (newline[i:(i+80)], '\\n' if i + 80 >= len(newline) else '', sys.argv[1]), file=out)
			else:
				print('\t@printf "%s%s" >> %s' % (newline[i:(i+80)], '\\n' if i + 80 >= len(newline) else '', sys.argv[1]), file=out)
