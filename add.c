#include "main.h"

mat_rv addition_coo(coo matrix1, coo matrix2)
{
	mat_rv rv;
	if(matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	if(matrix1.elems[0].type != matrix2.elems[0].type){
		rv.error = ERR_TYPE_MISSMATCH;
		return rv;
	}
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix1.cols;
	result.length = 0;
	int size = MALLOCINIT;
	if(!(result.elems = (coo_elem*)malloc(size*sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int matrix1_i = 0;
	int matrix2_i = 0;
	struct timespec start, end;
	get_cpu_time(&start);
	while(matrix1_i < matrix1.length && matrix2_i < matrix2.length){
		coo_elem next;
		next.type = matrix1.elems[0].type;
		if(matrix1.elems[matrix1_i].i == matrix2.elems[matrix2_i].i){
			if(matrix1.elems[matrix1_i].j == matrix2.elems[matrix2_i].j){
				if(next.type == MAT_INT)
					next.val.i = matrix1.elems[matrix1_i].val.i + matrix2.elems[matrix2_i].val.i;
				else
					next.val.f = matrix1.elems[matrix1_i].val.f + matrix2.elems[matrix2_i].val.f;
				next.i = matrix1.elems[matrix1_i].i;
				next.j = matrix1.elems[matrix1_i].j;
				++matrix1_i;
				++matrix2_i;
			}
			else if(matrix1.elems[matrix1_i].j > matrix2.elems[matrix2_i].j)
				next = matrix2.elems[matrix2_i++];
			else
				next = matrix1.elems[matrix1_i++];
		}
		else if(matrix1.elems[matrix1_i].i > matrix2.elems[matrix2_i].i)
			next = matrix2.elems[matrix2_i++];
		else
			next = matrix1.elems[matrix1_i++];
		if(size == result.length){
			size *= 2;
			if(!(result.elems = realloc(result.elems, size*sizeof(coo_elem)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
		result.elems[result.length++] = next;
	}
	while(matrix1_i < matrix1.length){
		if(size == result.length){
			size *= 2;
			if(!(result.elems = realloc(result.elems, size*sizeof(coo_elem)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
		result.elems[result.length++] = matrix1.elems[matrix1_i++];
	}
	while(matrix2_i < matrix2.length){
		if(size == result.length){
			size *= 2;
			if(!(result.elems = realloc(result.elems, size*sizeof(coo_elem)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
		result.elems[result.length++] = matrix2.elems[matrix2_i++];
	}
	get_cpu_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv addition(FILE* file1, FILE* file2, FORMAT format, bool nothreading)
{
	mat_rv rv;
	//not error checking type or format in this function
	switch(format){
	case COO:{
		struct timespec start, end;
		get_cpu_time(&start);
		coo matrix1 = read_coo(file1);
		coo matrix2 = read_coo(file2);
		get_cpu_time(&end);
		struct timespec delta = time_delta(end, start);
		mat_rv rv = addition_coo(matrix1, matrix2);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix1);
		free_coo(matrix2);
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
