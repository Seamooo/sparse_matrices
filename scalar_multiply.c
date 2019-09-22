#include "main.h"

mat_rv scalar_multiply_coo_nothreading(coo matrix, SCALAR *scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.type == MAT_INT){
			if(scalar->type == MAT_INT)
				matrix.elems[i].val.i = matrix.elems[i].val.i * scalar->val.i;
			else
				matrix.elems[i].val.f = (long double)matrix.elems[i].val.i * scalar->val.f;
		}
		else{
			if(scalar->type == MAT_INT)
				matrix.elems[i].val.f = matrix.elems[i].val.f * (long double)scalar->val.i;
			else
				matrix.elems[i].val.f = matrix.elems[i].val.f * scalar->val.f;
		}
		if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
			matrix.elems[i].type = MAT_LDOUBLE;
	}
	get_utc_time(&end);
	//change matrix type if necessary
	if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
		matrix.type = MAT_LDOUBLE;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply_coo(coo matrix, SCALAR *scalar, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	int i;
	#pragma omp parallel num_threads(thread_count) shared(scalar, matrix)
	{
		MAT_TYPE local_mat_type = matrix.type;
		//store scalar on the local stack
		SCALAR local_scalar;
		local_scalar.type = scalar->type;
		local_scalar.val = scalar->val;
		#pragma omp for private(i)
		for(i = 0; i < matrix.length; ++i){
			if(local_mat_type == MAT_INT){
				if(local_scalar.type == MAT_INT)
					matrix.elems[i].val.i = matrix.elems[i].val.i * local_scalar.val.i;
				else
					matrix.elems[i].val.f = (long double)matrix.elems[i].val.i * local_scalar.val.f;
			}
			else{
				if(local_scalar.type == MAT_INT)
					matrix.elems[i].val.f = matrix.elems[i].val.f * (long double)local_scalar.val.i;
				else
					matrix.elems[i].val.f = matrix.elems[i].val.f * local_scalar.val.f;
			}
			if((local_mat_type == MAT_INT && local_scalar.type == MAT_LDOUBLE))
				matrix.elems[i].type = MAT_LDOUBLE;
		}
	}
	get_utc_time(&end);
	//change matrix type if necessary
	if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
		matrix.type = MAT_LDOUBLE;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply_csr_nothreading(csr matrix, SCALAR *scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	if(matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE)
		result.type = MAT_LDOUBLE;
	result.cols = matrix.cols;
	result.rows = matrix.rows;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)malloc((result.rows + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(matrix.num_vals * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(matrix.num_vals * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(matrix.num_vals * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < matrix.num_vals; ++i){
		if(matrix.type == MAT_INT){
			if(scalar->type == MAT_INT)
				result.nnz.i[i] = matrix.nnz.i[i] * scalar->val.i;
			else{
				result.nnz.f[i] = (long double)matrix.nnz.i[i] * scalar->val.f;
			}
		}
		else{
			if(scalar->type == MAT_INT)
				result.nnz.f[i] = matrix.nnz.f[i] * (long double)scalar->val.i;
			else
				result.nnz.f[i] = matrix.nnz.f[i] * scalar->val.f;
		}
		result.ja[i] = matrix.ja[i];
	}
	for(int i = 0; i < matrix.cols + 1; ++i){
		result.ia[i] = matrix.ia[i];
	}
	get_utc_time(&end);
	rv = csr_to_mat(result);
	free_csr(result);
	rv.t_process = time_delta(end, start);
	return rv;
}
mat_rv scalar_multiply_csr(csr matrix, SCALAR *scalar, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	if(matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE)
		result.type = MAT_LDOUBLE;
	result.cols = matrix.cols;
	result.rows = matrix.rows;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)malloc((result.rows + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(matrix.num_vals * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(matrix.num_vals * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(matrix.num_vals * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix, result, scalar)
	{
		MAT_TYPE local_mat_type = matrix.type;
		//store scalar on the local stack
		SCALAR local_scalar;
		local_scalar.type = scalar->type;
		local_scalar.val = scalar->val;
		#pragma omp for private(i)
		for(i = 0; i < matrix.num_vals; ++i){
			if(local_mat_type == MAT_INT){
				if(local_scalar.type == MAT_INT)
					result.nnz.i[i] = matrix.nnz.i[i] * local_scalar.val.i;
				else{
					result.nnz.f[i] = (long double)matrix.nnz.i[i] * local_scalar.val.f;
				}
			}
			else{
				if(local_scalar.type == MAT_INT)
					result.nnz.f[i] = matrix.nnz.f[i] * (long double)local_scalar.val.i;
				else
					result.nnz.f[i] = matrix.nnz.f[i] * local_scalar.val.f;
			}
			result.ja[i] = matrix.ja[i];
		}
		#pragma omp for private(i)
		for(i = 0; i < matrix.cols + 1; ++i)
			result.ia[i] = matrix.ia[i];
	}
	get_utc_time(&end);
	rv = csr_to_mat(result);
	free_csr(result);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply_csc_nothreading(csc matrix, SCALAR *scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	if(matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE)
		result.type = MAT_LDOUBLE;
	result.cols = matrix.cols;
	result.rows = matrix.rows;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)malloc((result.cols + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(matrix.num_vals * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(matrix.num_vals * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(matrix.num_vals * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < matrix.num_vals; ++i){
		if(matrix.type == MAT_INT){
			if(scalar->type == MAT_INT)
				result.nnz.i[i] = matrix.nnz.i[i] * scalar->val.i;
			else
				result.nnz.f[i] = (long double)matrix.nnz.i[i] * scalar->val.f;
		}
		else{
			if(scalar->type == MAT_INT)
				result.nnz.f[i] = matrix.nnz.f[i] * (long double)scalar->val.i;
			else
				result.nnz.f[i] = matrix.nnz.f[i] * scalar->val.f;
		}
		result.ja[i] = matrix.ja[i];
	}
	for(int i = 0; i < matrix.cols + 1; ++i)
		result.ia[i] = matrix.ia[i];
	get_utc_time(&end);
	rv = csc_to_mat(result);
	free_csc(result);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply_csc(csc matrix, SCALAR *scalar, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	csr result;
	result.type = matrix.type;
	if(matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE)
		result.type = MAT_LDOUBLE;
	result.cols = matrix.cols;
	result.rows = matrix.rows;
	result.num_vals = matrix.num_vals;
	if(!(result.ia = (int*)malloc((result.cols + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ja = (int*)malloc(matrix.num_vals * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(matrix.num_vals * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(matrix.num_vals * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix, result, scalar)
	{
		MAT_TYPE local_mat_type = matrix.type;
		//store scalar on the local stack
		SCALAR local_scalar;
		local_scalar.type = scalar->type;
		local_scalar.val = scalar->val;
		#pragma omp for private(i)
		for(i = 0; i < matrix.num_vals; ++i){
			if(local_mat_type == MAT_INT){
				if(local_scalar.type == MAT_INT)
					result.nnz.i[i] = matrix.nnz.i[i] * local_scalar.val.i;
				else
					result.nnz.f[i] = (long double)matrix.nnz.i[i] * local_scalar.val.f;
			}
			else{
				if(local_scalar.type == MAT_INT)
					result.nnz.f[i] = matrix.nnz.f[i] * (long double)local_scalar.val.i;
				else
					result.nnz.f[i] = matrix.nnz.f[i] * local_scalar.val.f;
			}
			result.ja[i] = matrix.ja[i];
		}
		#pragma omp for private(i)
		for(i = 0; i < matrix.cols + 1; ++i)
			result.ia[i] = matrix.ia[i];
	}
	get_utc_time(&end);
	rv = csc_to_mat(result);
	free_csc(result);
	rv.t_process = time_delta(end, start);
	return rv;
}

//will send address of scalar to functions to avoid
//incompatibility with versions of GCC earlier than 4.4
mat_rv scalar_multiply(OPERATIONARGS *args)
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
			rv = scalar_multiply_coo_nothreading(matrix, &(args->scalar));
		else
			rv = scalar_multiply_coo(matrix, &(args->scalar), args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
		rv.isval = false;
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
			rv = scalar_multiply_csr_nothreading(matrix, &(args->scalar));
		else
			rv = scalar_multiply_csr(matrix, &(args->scalar), args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_csr(matrix);
		rv.isval = false;
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
			rv = scalar_multiply_csc_nothreading(matrix, &(args->scalar));
		else
			rv = scalar_multiply_csc(matrix, &(args->scalar), args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_csc(matrix);
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
