#include "main.h"

mat_rv trace_coo_nothreading(coo matrix)
{
	mat_rv rv;
	if(matrix.rows != matrix.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	coo result;
	result.type = matrix.type;
	result.cols = 1;
	result.rows = 1;
	result.length = 1;
	if(!(result.elems = (coo_elem*)malloc(1*sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.elems[0].type = matrix.type;
	result.elems[0].i = 0;
	result.elems[0].j = 0;
	if (result.elems[0].type == MAT_INT)
		result.elems[0].val.i = 0;
	else
		result.elems[0].val.f = 0.0;
	struct timespec start, end;
	get_cpu_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].i == matrix.elems[i].j){
			if (result.elems[0].type == MAT_INT)
				result.elems[0].val.i += matrix.elems[i].val.i;
			else
				result.elems[0].val.f += matrix.elems[i].val.f;
		}
	}
	get_cpu_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}
mat_rv trace_coo(coo matrix)
{
	mat_rv rv;
	if(matrix.rows != matrix.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	coo result;
	result.type = matrix.type;
	result.cols = 1;
	result.rows = 1;
	result.length = 1;
	if(!(result.elems = (coo_elem*)malloc(1*sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.elems[0].type = matrix.type;
	result.elems[0].i = 0;
	result.elems[0].j = 0;
	if (result.elems[0].type == MAT_INT)
		result.elems[0].val.i = 0;
	else
		result.elems[0].val.f = 0.0;
	struct timespec start, end;
	get_cpu_time(&start);
	int i;
	#pragma omp parallel for private(i) shared(result, matrix)
	for(i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].i == matrix.elems[i].j){
			if (result.elems[0].type == MAT_INT)
				result.elems[0].val.i += matrix.elems[i].val.i;
			else
				result.elems[0].val.f += matrix.elems[i].val.f;
		}
	}
	get_cpu_time(&end);
	//add parallelism here later
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv trace(FILE* file, FORMAT format, bool nothreading)
{
	mat_rv rv;
	switch(format){
	case COO:{
		struct timespec start, end;
		get_cpu_time(&start);
		coo matrix = read_coo(file);
		get_cpu_time(&end);
		struct timespec delta = time_delta(end, start);
		if(nothreading)
			rv = trace_coo_nothreading(matrix);
		else
			rv = trace_coo(matrix);
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
