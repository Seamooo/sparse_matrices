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
	get_utc_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].i == matrix.elems[i].j){
			if (result.elems[0].type == MAT_INT)
				result.elems[0].val.i += matrix.elems[i].val.i;
			else
				result.elems[0].val.f += matrix.elems[i].val.f;
		}
	}
	get_utc_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}
mat_rv trace_coo(coo matrix, int thread_count)
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
	get_utc_time(&start);
	int i;
	#pragma omp parallel for num_threads(thread_count) private(i) shared(result, matrix)
	for(i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].i == matrix.elems[i].j){
			if (result.elems[0].type == MAT_INT)
				result.elems[0].val.i += matrix.elems[i].val.i;
			else
				result.elems[0].val.f += matrix.elems[i].val.f;
		}
	}
	get_utc_time(&end);
	//add parallelism here later
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

//using trace to build every format initially
mat_rv trace(OPERATIONARGS *args)
{
	mat_rv rv;
	//default = COO
	if (args->format == FORM_DEFAULT)
		args->format = COO;
	switch(args->format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix = read_coo(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = trace_coo_nothreading(matrix);
		else
			rv = trace_coo(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
		rv.isval = true;
		return rv;
		break;
	}
	case CSR:{
		struct timespec start, end;
		get_utc_time(&start);
		csr matrix = read_csr(args->file1);
		get_utc_time(&end);
		//struct timespec delta = time_delta(end, start);
		print_csr(matrix);
		fprintf(stderr, "operation not implemented\n");
		exit(EXIT_FAILURE);
	}
	case CSC:{
		struct timespec start, end;
		get_utc_time(&start);
		csc matrix = read_csc(args->file1);
		get_utc_time(&end);
		//struct timespec delta = time_delta(end, start);
		print_csc(matrix);
		fprintf(stderr, "operation not implemented\n");
		exit(EXIT_FAILURE);
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
