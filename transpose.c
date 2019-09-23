#include "main.h"

mat_rv transpose_csr_csc_nothreading(csr matrix){
	mat_rv rv;
	rv = csc_to_mat_nothreading(matrix);
	//process time is 0 as treating csr as csc results in an already transposed matrix
	rv.t_process.tv_sec = 0;
	rv.t_process.tv_nsec = 0;
	return rv;
}

mat_rv transpose_csr_csc(csr matrix, int thread_count){
	mat_rv rv;
	rv = csc_to_mat(matrix, thread_count);
	//process time is 0 as treating csr as csc results in an already transposed matrix
	rv.t_process.tv_sec = 0;
	rv.t_process.tv_nsec = 0;
	return rv;
}

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
	rv = coo_to_mat_nothreading(matrix);
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
	rv = coo_to_mat(matrix, thread_count);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv transpose_csr_nothreading(csr matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.cols = matrix.rows;
	result.rows = matrix.cols;
	result.type = matrix.type;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)calloc(result.rows + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(result.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//creating a new array to contain lengths so don't have to search for
	//next nonzero element and make complexity n^2
	int *col_lens;
	if(!(col_lens = (int*)calloc(result.cols, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//count num_vals per row
	for(int i = 0; i < matrix.num_vals; ++i)
		result.ia[matrix.ja[i] + 1]++;
	for(int i = 0; i < result.rows + 1; ++i)
		result.ia[i+1] += result.ia[i];
	for(int i = 0; i < matrix.rows; ++i){
		for(int j = matrix.ia[i]; j < matrix.ia[i + 1]; ++j){
			//take the ia from whatever column it is currently + col_len as
			//we're iterating from smallest row to largest row
			result.ja[result.ia[matrix.ja[j]]+ col_lens[matrix.ja[j]]] = i;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[matrix.ja[j]] + col_lens[matrix.ja[j]]] = matrix.nnz.i[j];
			else
				result.nnz.f[result.ia[matrix.ja[j]] + col_lens[matrix.ja[j]]] = matrix.nnz.f[j];
			col_lens[matrix.ja[j]]++;
		}
	}
	free(col_lens);
	get_utc_time(&end);
	rv = csr_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv transpose_csr(csr matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.cols = matrix.rows;
	result.rows = matrix.cols;
	result.type = matrix.type;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)calloc(result.rows + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(result.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//creating a new array to contain lengths so don't have to search for
	//next nonzero element and make complexity n^2
	int *col_lens;
	if(!(col_lens = (int*)calloc(result.cols, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//count num_vals per row (can't be parallelised)
	for(int i = 0; i < matrix.num_vals; ++i)
		result.ia[matrix.ja[i] + 1]++;
	for(int i = 0; i < result.rows + 1; ++i)
		result.ia[i+1] += result.ia[i];
	//below can't be parallelised due to requiring iterator go by smallest
	//row to largest row and random access dependancies
	for(int i = 0; i < matrix.rows; ++i){
		for(int j = matrix.ia[i]; j < matrix.ia[i + 1]; ++j){
			//take the ia from whatever column it is currently + col_len as
			//we're iterating from smallest row to largest row
			result.ja[result.ia[matrix.ja[j]]+ col_lens[matrix.ja[j]]] = i;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[matrix.ja[j]] + col_lens[matrix.ja[j]]] = matrix.nnz.i[j];
			else
				result.nnz.f[result.ia[matrix.ja[j]] + col_lens[matrix.ja[j]]] = matrix.nnz.f[j];
			col_lens[matrix.ja[j]]++;
		}
	}
	free(col_lens);
	get_utc_time(&end);
	rv = csr_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv transpose_csc_nothreading(csc matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csc result;
	result.rows = matrix.cols;
	result.cols = matrix.rows;
	result.type = matrix.type;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)calloc(result.cols + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(result.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//creating a new array to contain lengths so don't have to search for
	//next nonzero element and make complexity n^2
	int *row_lens;
	if(!(row_lens = (int*)calloc(result.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//count num_vals per col
	for(int i = 0; i < matrix.num_vals; ++i)
		result.ia[matrix.ja[i] + 1]++;
	for(int i = 0; i < result.cols + 1; ++i)
		result.ia[i+1] += result.ia[i];
	for(int i = 0; i < matrix.cols; ++i){
		for(int j = matrix.ia[i]; j < matrix.ia[i + 1]; ++j){
			result.ja[result.ia[matrix.ja[j]]+ row_lens[matrix.ja[j]]] = i;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[matrix.ja[j]] + row_lens[matrix.ja[j]]] = matrix.nnz.i[j];
			else
				result.nnz.f[result.ia[matrix.ja[j]] + row_lens[matrix.ja[j]]] = matrix.nnz.f[j];
			row_lens[matrix.ja[j]]++;
		}
	}
	free(row_lens);
	get_utc_time(&end);
	rv = csc_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csc(result);
	return rv;
}

mat_rv transpose_csc(csc matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csc result;
	result.rows = matrix.cols;
	result.cols = matrix.rows;
	result.type = matrix.type;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)calloc(result.cols + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(result.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//creating a new array to contain lengths so don't have to search for
	//next nonzero element and make complexity n^2
	int *row_lens;
	if(!(row_lens = (int*)calloc(result.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//count num_vals per col
	for(int i = 0; i < matrix.num_vals; ++i)
		result.ia[matrix.ja[i] + 1]++;
	for(int i = 0; i < result.cols + 1; ++i)
		result.ia[i+1] += result.ia[i];
	for(int i = 0; i < matrix.cols; ++i){
		for(int j = matrix.ia[i]; j < matrix.ia[i + 1]; ++j){
			result.ja[result.ia[matrix.ja[j]]+ row_lens[matrix.ja[j]]] = i;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[matrix.ja[j]] + row_lens[matrix.ja[j]]] = matrix.nnz.i[j];
			else
				result.nnz.f[result.ia[matrix.ja[j]] + row_lens[matrix.ja[j]]] = matrix.nnz.f[j];
			row_lens[matrix.ja[j]]++;
		}
	}
	free(row_lens);
	get_utc_time(&end);
	rv = csc_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_csc(result);
	return rv;
}

mat_rv transpose(OPERATIONARGS *args)
{
	mat_rv rv;
	//default CSR -> CSC
	switch(args->format){
	case FORM_DEFAULT:{
		struct timespec start, end;
		get_utc_time(&start);
		csr matrix = read_csr(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = transpose_csr_csc_nothreading(matrix);
		else
			rv = transpose_csr_csc(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_csr(matrix);
		return rv;
		break;
	}
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix = read_coo(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = transpose_coo_nothreading(matrix);
		else
			rv = transpose_coo(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
		return rv;
		break;
	}
	case CSR:{
		struct timespec start, end;
		get_utc_time(&start);
		csr matrix = read_csr(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = transpose_csr_nothreading(matrix);
		else
			rv = transpose_csr(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_csr(matrix);
		return rv;
		break;
	}
	case CSC:{
		struct timespec start, end;
		get_utc_time(&start);
		csc matrix = read_csc(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = transpose_csc_nothreading(matrix);
		else
			rv = transpose_csc(matrix, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_csc(matrix);
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
