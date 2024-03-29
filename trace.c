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
	rv = coo_to_mat_nothreading(result);
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
	#pragma omp parallel num_threads(thread_count) shared(result, matrix)
	{
		union{
			int i;
			long double f;
		} local_result;
		MAT_TYPE local_type = result.type;
		#pragma omp for private(i)
		for(i = 0; i < matrix.length; ++i){
			if(matrix.elems[i].i == matrix.elems[i].j){
				if (local_type == MAT_INT)
					local_result.i += matrix.elems[i].val.i;
				else
					local_result.f += matrix.elems[i].val.f;
			}
		}
		#pragma omp critical
		{
			if(local_type == MAT_INT)
				result.elems[0].val.i += local_result.i;
			else
				result.elems[0].val.f += local_result.f;
		}
	}

	get_utc_time(&end);
	//add parallelism here later
	rv = coo_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv trace_csr_nothreading(csr matrix)
{
	mat_rv rv;
	if(matrix.rows != matrix.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	result.rows = 1;
	result.cols = 1;
	result.num_vals = 1;
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(1*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(1*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allo0cating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ja = (int*)malloc(1*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ia = (int*)malloc(2*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.ja[0] = 0;
	result.ia[0] = 0;
	result.ia[1] = 1;
	if(result.type == MAT_INT)
		result.nnz.i[0] = 0;
	else
		result.nnz.f[0] = 0.0;
	for(int i = 0; i < matrix.rows; ++i){
		int j = matrix.ia[i];
		while(j < matrix.ia[i + 1] && matrix.ja[j] < i)
			++j;
		if(matrix.ja[j] == i){
			if(result.type == MAT_INT)
				result.nnz.i[0] += matrix.nnz.i[j];
			else
				result.nnz.f[0] += matrix.nnz.f[j];
		}
	}
	get_utc_time(&end);
	rv = csr_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv trace_csr(csr matrix, int thread_count)
{
	mat_rv rv;
	if(matrix.rows != matrix.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	result.rows = 1;
	result.cols = 1;
	result.num_vals = 1;
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(1*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(1*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allo0cating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ja = (int*)malloc(1*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ia = (int*)malloc(2*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.ja[0] = 0;
	result.ia[0] = 0;
	result.ia[1] = 1;
	if(result.type == MAT_INT)
		result.nnz.i[0] = 0;
	else
		result.nnz.f[0] = 0.0;
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix, result)
	{
		//store result type to not have every thread requesting this resource
		MAT_TYPE local_type = result.type;
		union{
			long double f;
			int i;
		} local_result;
		if(local_type == MAT_INT)
			local_result.i = 0;
		else
			local_result.f = 0.0;
		#pragma omp for private(i)
		for(i = 0; i < matrix.rows; ++i){
			int j = matrix.ia[i];
			while(j < matrix.ia[i + 1] && matrix.ja[j] < i)
				++j;
			if(matrix.ja[j] == i){
				if(local_type == MAT_INT)
					local_result.i += matrix.nnz.i[j];
				else
					local_result.f += matrix.nnz.f[j];
			}
		}
		#pragma omp critical
		{
			if(local_type == MAT_INT)
				result.nnz.i[0] += local_result.i;
			else
				result.nnz.f[0] += local_result.f;
		}
	}
	get_utc_time(&end);
	rv = csr_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

//identical behaviour for square matrices (as assumed otherwise errors)
mat_rv trace_csc_nothreading(csc matrix)
{
	return trace_csr_nothreading(matrix);
}
mat_rv trace_csc(csc matrix, int thread_count)
{
	return trace_csr(matrix, thread_count);
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
		struct timespec construct;
		struct timespec fileio_timer;
		coo matrix = read_coo(args->file1, &construct, &fileio_timer);
		if(args->nothreading)
			rv = trace_coo_nothreading(matrix);
		else
			rv = trace_coo(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = fileio_timer;
		free_coo(matrix);
		rv.isval = true;
		return rv;
		break;
	}
	case CSR:{
		struct timespec construct;
		struct timespec fileio_timer;
		csr matrix = read_csr(args->file1, &construct, &fileio_timer);
		if(args->nothreading)
			rv = trace_csr_nothreading(matrix);
		else
			rv = trace_csr(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = fileio_timer;
		free_csr(matrix);
		rv.isval = true;
		return rv;
		break;
	}
	case CSC:{
		struct timespec construct;
		struct timespec fileio_timer;
		csc matrix = read_csc(args->file1, &construct, &fileio_timer);
		if(args->nothreading)
			rv = trace_csc_nothreading(matrix);
		else
			rv = trace_csc(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = fileio_timer;
		free_csc(matrix);
		rv.isval = true;
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
