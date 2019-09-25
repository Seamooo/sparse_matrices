import os
import sys
import json
import time

input_files = [
	'int64.in',
	'float64.in',
	'int128.in',
	'float128.in',
	'int256.in',
	'float256.in',
	'int512.in',
	'float512.in',
	'int1024.in',
	'float1024.in'
]

formats = [
	'',
	'--format COO',
	'--format CSR',
	'--format CSC'
]
sf_operations = [
	'sm 3.5',
	'tr',
	'ts'
]

df_operations = [
	'ad',
	'mm'
]
number_of_threads = 16

cmd_start = 'sparse_matrix.exe' if sys.platform == 'win32' else './sparse_matrix.bin'

filename = 'timing'
files = os.listdir(os.getcwd())
i = 1
while '%s%s' % (filename, '.out') in files:
	filename = 'timing (%d)' % i
	i += 1
for input_file in input_files:
	if input_file not in files:
		print(input_file, 'not found')
		sys.exit()
output = {}
start = time.process_time()
with open('%s%s' % (filename, '.out'), 'w') as fp:
	for file in input_files:
		if file not in output:
			output[file] = {}
		for mat_format in formats:
			if mat_format not in output[file]:
				output[file][mat_format] = [{} for _ in range(number_of_threads + 1)]
			for num_threads in range(number_of_threads + 1):
				for operation in sf_operations:
					if operation not in output[file][mat_format][num_threads]:
						output[file][mat_format][num_threads][operation] = {'TOTAL_PT': 0.0, 'TOTAL_CT': 0.0, 'AVG_PT': 0.0, 'AVG_CT': 0.0}
					cmd = '%s --%s -f %s %s %s --timing' % (cmd_start, operation, file, '--nothreading' if num_threads == 0 else '-t %d' % num_threads, mat_format)
					print(cmd, file=fp)
					print(cmd)
					total_process_time = 0.0
					total_construction_time = 0.0
					for _ in range(100):
						#print(os.popen(cmd).read().encode())
						construction_time, process_time = list(map(float, os.popen(cmd).read().rstrip('\n').split('\n')[-2:]))
						total_process_time += process_time
						total_construction_time += construction_time
						print('%f %f' % (process_time, construction_time), file=fp)
					print('TOTAL_PT=%f TOTAL_CT=%f' % (total_process_time, total_construction_time), file=fp)
					print('AVG_PT=%f AVG_CT=%f' % (total_process_time / 100, total_construction_time / 100), file=fp)
					output[file][mat_format][num_threads][operation]['TOTAL_PT'] = total_process_time
					output[file][mat_format][num_threads][operation]['TOTAL_CT'] = total_construction_time
					output[file][mat_format][num_threads][operation]['AVG_PT'] = total_process_time / 100
					output[file][mat_format][num_threads][operation]['AVG_CT'] = total_construction_time / 100

				for operation in df_operations:
					if operation not in output[file][mat_format]:
						output[file][mat_format][num_threads][operation] = {'TOTAL_PT': 0.0, 'TOTAL_CT': 0.0, 'AVG_PT': 0.0, 'AVG_CT': 0.0}
					cmd = '%s --%s -f %s %s %s %s --timing' % (cmd_start, operation, file, file, '--nothreading' if num_threads == 0 else '-t %d' % num_threads, mat_format)
					print(cmd, file=fp)
					print(cmd)
					total_process_time = 0.0
					total_construction_time = 0.0
					for _ in range(100):
						construction_time, process_time = list(map(float, os.popen(cmd).read().rstrip('\n').split('\n')[-2:]))
						total_process_time += process_time
						total_construction_time += construction_time
						print('%f %f' % (process_time, construction_time), file=fp)
					print('TOTAL_PT=%f TOTAL_CT=%f' % (total_process_time, total_construction_time), file=fp)
					print('AVG_PT=%f AVG_CT=%f' % (total_process_time / 100, total_construction_time / 100), file=fp)
					output[file][mat_format][num_threads][operation]['TOTAL_PT'] = total_process_time
					output[file][mat_format][num_threads][operation]['TOTAL_CT'] = total_construction_time
					output[file][mat_format][num_threads][operation]['AVG_PT'] = total_process_time / 100
					output[file][mat_format][num_threads][operation]['AVG_CT'] = total_construction_time / 100

with open('%s%s' % (filename, '.json'), 'w') as fp:
	json.dump(output, fp)

print('time elapsed: {}s'.format(time.process_time() - start))


