#include "main.h"

mat_rv transpose_coo(coo matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_cpu_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		int temp_i = matrix.elems[i].i;
		matrix.elems[i].i = matrix.elems[i].j;
		matrix.elems[i].j = temp_i;
	}
	get_cpu_time(&end);
	int temp_cols = matrix.cols;
	matrix.cols = matrix.rows;
	matrix.rows = temp_cols;
	//matrix isn't in ordered form but isn't important for conversion
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv transpose(FILE* file, FORMAT format)
{
	mat_rv rv;
	switch(format){
	case COO:{
		struct timespec start, end;
		get_cpu_time(&start);
		coo matrix = read_coo(file);
		get_cpu_time(&end);
		struct timespec delta = time_delta(end, start);
		mat_rv rv = transpose_coo(matrix);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
		return rv;
		break;
	}
	default:
		fprintf(stderr, "format not implemented\n");
		exit(EXIT_FAILURE);
		break;
	}
	//execution should never reach here
	rv.error = ERR_NOT_SET;
	return rv;
}
