import sys

if len(sys.argv) < 2:
	print('expected file')
	sys.exit()

out = open('%s.make' % sys.argv[1], 'w')

with open(sys.argv[1], 'r') as fp:
	lines = fp.readlines()
	lines = list(map(lambda x: x.rstrip('\n'), lines))
	print('%s:' % sys.argv[1], file=out)
	print('\t@echo \'\' > %s' % sys.argv[1], file=out)
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
		if firstline:
			firstline = False
			continue
		print('\t@echo "%s" >> %s' % (''.join(newline), sys.argv[1]), file=out)
