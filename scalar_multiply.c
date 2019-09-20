#include "main.h"

mat_rv scalar_multiply_coo_nothreading(coo matrix, float scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.type == MAT_INT)
			matrix.elems[i].val.f = (float)matrix.elems[i].val.i * scalar;
		else
			matrix.elems[i].val.f = matrix.elems[i].val.f * scalar;
		matrix.elems[i].type = MAT_FLOAT;
	}
	get_utc_time(&end);
	matrix.type = MAT_FLOAT;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

//slow down at this stage
mat_rv scalar_multiply_coo(coo matrix, float scalar, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	int i;
	get_utc_time(&start);
	#pragma omp parallel for num_threads(thread_count) private(i) shared(rv, matrix, scalar)
	for(i = 0; i < matrix.length; ++i){
		if(matrix.type == MAT_INT)
			matrix.elems[i].val.f = (float)matrix.elems[i].val.i * scalar;
		else
			matrix.elems[i].val.f = matrix.elems[i].val.f * scalar;
		matrix.elems[i].type = MAT_FLOAT;
	}
	get_utc_time(&end);
	matrix.type = MAT_FLOAT;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply(OPERATIONARGS args)
{
	mat_rv rv;
	//not error checking type or format in this function
	switch(args.format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix = read_coo(args.file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args.nothreading)
			rv = scalar_multiply_coo_nothreading(matrix, args.scalar);
		else
			rv = scalar_multiply_coo(matrix, args.scalar, args.num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
		rv.isval = false;
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
