#include "main.h"

mat_rv coo_to_mat_nothreading(coo matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < matrix.length; ++i){
		if(!(matrix.elems[i].i < rv.rows && matrix.elems[i].j < rv.cols)){
			rv.error = ERR_DIM_MISSMATCH;
			break;
		}
		if(rv.type == MAT_INT){
			if (rv.vals.i[matrix.elems[i].i * rv.cols + matrix.elems[i].j] != 0){
				pint(matrix.elems[i].i);
				pint(matrix.elems[i].j);
				rv.error = ERR_DUPLICATE;
				break;
			}
			rv.vals.i[matrix.elems[i].i * rv.cols + matrix.elems[i].j] = matrix.elems[i].val.i;
		}
		else{
			if (rv.vals.f[matrix.elems[i].i * rv.cols + matrix.elems[i].j] != 0.0){
				pint(matrix.elems[i].i);
				pint(matrix.elems[i].j);
				rv.error = ERR_DUPLICATE;
				break;
			}
			rv.vals.f[matrix.elems[i].i * rv.cols + matrix.elems[i].j] = matrix.elems[i].val.f;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv coo_to_mat(coo matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix, rv)
	{
		MAT_TYPE local_type = matrix.type;
		#pragma omp for private(i)
		for(i = 0; i < matrix.length; ++i){
			if(!(matrix.elems[i].i < rv.rows && matrix.elems[i].j < rv.cols))
				rv.error = ERR_DIM_MISSMATCH;
			if(local_type == MAT_INT){
				if(rv.vals.i[matrix.elems[i].i * rv.cols + matrix.elems[i].j] != 0)
					rv.error = ERR_DUPLICATE;
				rv.vals.i[matrix.elems[i].i * rv.cols + matrix.elems[i].j] = matrix.elems[i].val.i;
			}
			else{
				if (rv.vals.f[matrix.elems[i].i * rv.cols + matrix.elems[i].j] != 0.0)
					rv.error = ERR_DUPLICATE;
				rv.vals.f[matrix.elems[i].i * rv.cols + matrix.elems[i].j] = matrix.elems[i].val.f;
			}
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv csr_to_mat_nothreading(csr matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < rv.rows; ++i){
		//iterator for nnz/ja
		int a_i = matrix.ia[i];
		for(int j = 0; j < rv.cols; ++j){
			if(a_i < matrix.ia[i + 1]){
				if(matrix.ja[a_i] == j){
					if(rv.type == MAT_INT)
						rv.vals.i[i*rv.cols + j] = matrix.nnz.i[a_i];
					else
						rv.vals.f[i*rv.cols + j] = matrix.nnz.f[a_i];
					a_i++;
					continue;
				}
			}
			if(rv.type == MAT_INT)
				rv.vals.i[i*rv.cols + j] = 0;
			else
				rv.vals.f[i*rv.cols + j] = 0.0;
		}
		if(a_i != matrix.ia[i + 1]){
			rv.error = ERR_DIM_MISSMATCH;
			break;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv csr_to_mat(csr matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(rv, matrix)
	{
		MAT_TYPE local_type = rv.type;
		#pragma omp for private(i)
		for(i = 0; i < rv.rows; ++i){
			int a_i = matrix.ia[i];
			for(int j = 0; j < rv.cols; ++j){
				if(a_i < matrix.ia[i + 1]){
					if(matrix.ja[a_i] == j){
						if(local_type == MAT_INT)
							rv.vals.i[i*rv.cols + j] = matrix.nnz.i[a_i];
						else
							rv.vals.f[i*rv.cols + j] = matrix.nnz.f[a_i];
						a_i++;
						continue;
					}
				}
				if(local_type == MAT_INT)
					rv.vals.i[i*rv.cols + j] = 0;
				else
					rv.vals.f[i*rv.cols + j] = 0.0;
			}
			if(a_i != matrix.ia[i + 1])
				rv.error = ERR_DIM_MISSMATCH;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv csc_to_mat_nothreading(csc matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < rv.cols; ++i){
		//iterator for nnz/ja
		int a_i = matrix.ia[i];
		for(int j = 0; j < rv.rows; ++j){
			if(a_i < matrix.ia[i + 1]){
				if(matrix.ja[a_i] == j){
					if(rv.type == MAT_INT)
						rv.vals.i[j*rv.cols + i] = matrix.nnz.i[a_i];
					else
						rv.vals.f[j*rv.cols + i] = matrix.nnz.f[a_i];
					a_i++;
					continue;
				}
			}
			if(rv.type == MAT_INT)
				rv.vals.i[j*rv.cols + i] = 0;
			else
				rv.vals.f[j*rv.cols + i] = 0.0;
		}
		if(a_i != matrix.ia[i + 1]){
			rv.error = ERR_DIM_MISSMATCH;
			break;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv csc_to_mat(csc matrix, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.cols = matrix.cols;
	rv.rows = matrix.rows;
	rv.type = matrix.type;
	rv.isval = false;
	if(matrix.type == MAT_INT){
		if(!(rv.vals.i = (int*)calloc((rv.rows * rv.cols), sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.vals.f = (long double*)calloc((rv.rows * rv.cols), sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(rv, matrix)
	{
		MAT_TYPE local_type = rv.type;
		#pragma omp for private(i)
		for(i = 0; i < rv.cols; ++i){
			int a_i = matrix.ia[i];
			for(int j = 0; j < rv.rows; ++j){
				if(a_i < matrix.ia[i + 1]){
					if(matrix.ja[a_i] == j){
						if(local_type == MAT_INT)
							rv.vals.i[i*rv.rows + j] = matrix.nnz.i[a_i];
						else
							rv.vals.f[i*rv.rows + j] = matrix.nnz.f[a_i];
						a_i++;
						continue;
					}
				}
				if(local_type == MAT_INT)
					rv.vals.i[i*rv.rows + j] = 0;
				else
					rv.vals.f[i*rv.rows + j] = 0.0;
			}
			if(a_i != matrix.ia[i + 1])
				rv.error = ERR_DIM_MISSMATCH;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}
