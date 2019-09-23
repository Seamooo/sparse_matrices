#include "main.h"

mat_rv transpose_csr_csc_nothreading(csr matrix){
	mat_rv rv;
	rv = csc_to_mat(matrix);
	//process time is 0 as treating csr as csc results in an already transposed matrix
	rv.t_process.tv_sec = 0;
	rv.t_process.tv_nsec = 0;
	return rv;
}

mat_rv transpose_csr_csc(csr matrix, int thread_count){
	mat_rv rv;
	//later add threaded destruction function to use thread_count
	//filler function for no compiler warning in the meantime
	if(thread_count == -1)
		p();
	rv = csc_to_mat(matrix);
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

mat_rv transpose_csr_nothreading(csr matrix)
{
	mat_rv rv;
	struct timespec start, end;
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
	//creating a new array to contain lengths so don't ahve to search for
	//next nonzero element and make complexity n^2
	int *row_lens;
	if(!(row_lens = (int*)calloc(result.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//count num_vals per row
	for(int i = 0; i < matrix.rows; ++i)
		result.ia[matrix.ja[i] + 1] += 1;
	for(int i = 0; i < result.rows + 1; ++i)
		result.ia[i+1] += result.ia[i];
	print_int_arr(result.ia, result.rows + 1);
	for(int i = 0; i < matrix.rows; ++i){
		for(int j = matrix.ia[i]; j < matrix.ia[i + 1]; ++j){
			//take the ia from whatever column it is currently + row_len as
			//we're iterating from smallest row to largest row
			result.ja[result.ia[matrix.ja[j]]+ row_lens[i]] = i;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[matrix.ja[j]] + row_lens[i]] = matrix.nnz.i[j];
			else
				result.nnz.f[result.ia[matrix.ja[j]] + row_lens[i]] = matrix.nnz.f[j];
			row_lens[i]++;
		}
	}
	free(row_lens);
	get_utc_time(&end);
	rv = csr_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv transpose_csr(csr matrix, int thread_count)
{
	mat_rv rv;
	rv.error = ERR_NOT_SET;
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
		//struct timespec delta = time_delta(end, start);
		print_csc(matrix);
		fprintf(stderr, "format not implemented\n");
		exit(EXIT_FAILURE);
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
