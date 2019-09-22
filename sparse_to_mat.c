#include "main.h"

mat_rv coo_to_mat(coo matrix)
{
	mat_rv rv;
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	if(matrix.type == MAT_INT){
		rv.type = MAT_INT;
		if(!(rv.vals.i = (int*)malloc(rv.rows * rv.cols * sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		rv.type = MAT_LDOUBLE;
		if(!(rv.vals.f = (long double*)malloc(rv.rows * rv.cols * sizeof(long double)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	struct timespec start, end;
	get_utc_time(&start);
	for(int i = 0; i < rv.rows; ++i){
		for(int j = 0; j < rv.cols; ++j){
			if (rv.type == MAT_INT)
				rv.vals.i[i*rv.cols + j] = 0;
			else
				rv.vals.f[i*rv.cols + j] = 0.0;
		}
	}
	for(int i = 0; i < matrix.length; ++i){
		if(!(matrix.elems[i].i < rv.rows && matrix.elems[i].j < rv.cols)){
			rv.error = ERR_DIM_MISSMATCH;
			break;
		}
		if(rv.type == MAT_INT){
			if (rv.vals.i[matrix.elems[i].i * rv.rows + matrix.elems[i].j] != 0){
				printf("(%d, %d)\n", matrix.elems[i].i, matrix.elems[i].j);
				rv.error = ERR_DUPLICATE;
				break;
			}
			rv.vals.i[matrix.elems[i].i * rv.rows + matrix.elems[i].j] = matrix.elems[i].val.i;
		}
		else{
			if (rv.vals.f[matrix.elems[i].i * rv.rows + matrix.elems[i].j] != 0.0){
				rv.error = ERR_DUPLICATE;
				break;
			}
			rv.vals.f[matrix.elems[i].i * rv.rows + matrix.elems[i].j] = matrix.elems[i].val.f;
		}
	}
	get_utc_time(&end);
	rv.t_construct = time_delta(end, start);
	return rv;
}

mat_rv csr_to_mat(csr matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	if(matrix.type == MAT_INT){
		rv.type = MAT_INT;
		if(!(rv.vals.i = (int*)malloc(rv.rows * rv.cols * sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		rv.type = MAT_LDOUBLE;
		if(!(rv.vals.f = (long double*)malloc(rv.rows * rv.cols * sizeof(long double)))){
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

mat_rv csc_to_mat(csc matrix)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	rv.error = ERR_NONE;
	rv.rows = matrix.rows;
	rv.cols = matrix.cols;
	if(matrix.type == MAT_INT){
		rv.type = MAT_INT;
		if(!(rv.vals.i = (int*)malloc(rv.rows * rv.cols * sizeof(int)))){
			fprintf(stderr,"Ran out of virtual memory while allocating mat_rv struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		rv.type = MAT_LDOUBLE;
		if(!(rv.vals.f = (long double*)malloc(rv.rows * rv.cols * sizeof(long double)))){
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
				rv.vals.i[j*rv.cols + i] = matrix.nnz.i[a_i];
			else
				rv.vals.f[j*rv.cols + i] = matrix.nnz.f[a_i];
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
