#include "main.h"

mat_rv transpose_coo_nothreading(coo matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		int temp_i = matrix.elems[i].i;
		matrix.elems[i].i = matrix.elems[i].j;
		matrix.elems[i].j = temp_i;
	}
	get_utc_time(&end);
	int temp_cols = matrix.cols;
	matrix.cols = matrix.rows;
	matrix.rows = temp_cols;
	//matrix isn't in ordered form but isn't important for conversion
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}
//no speedup currently
mat_rv transpose_coo(coo matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	int i;
	get_utc_time(&start);
	#pragma omp parallel for num_threads(thread_count) private(i) shared(matrix)
	for(i = 0; i < matrix.length; ++i){
		int temp_i = matrix.elems[i].i;
		matrix.elems[i].i = matrix.elems[i].j;
		matrix.elems[i].j = temp_i;
	}
	get_utc_time(&end);
	int temp_cols = matrix.cols;
	matrix.cols = matrix.rows;
	matrix.rows = temp_cols;
	//matrix isn't in ordered form but isn't important for conversion
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv transpose(OPERATIONARGS args)
{
	mat_rv rv;
	switch(args.format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix = read_coo(args.file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args.nothreading)
			rv = transpose_coo_nothreading(matrix);
		else
			rv = transpose_coo(matrix, args.num_threads);
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
