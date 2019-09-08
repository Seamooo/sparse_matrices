#include "main.h"

mat_rv matrix_multiply_coo(coo matrix1, coo matrix2)
{
	mat_rv rv;
	if(matrix1.cols != matrix2.rows){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	if(matrix1.type != matrix2.type){
		rv.error = ERR_TYPE_MISSMATCH;
		return rv;
	}
	sort_coo(matrix2,ORDER_COL);
	//don't need to sort matrix1 as it's already in reduced forms
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.length = 0;
	int size = MALLOCINIT;
	if(!(result.elems = malloc(size * sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix");
		exit(EXIT_FAILURE);
	}
	int matrix1_i = 0;
	int matrix2_i = 0;
	struct timespec start, end;
	get_cpu_time(&start);
	while(matrix1_i < matrix1.length){
		int row_start = matrix1_i;
		matrix2_i = 0;
		while(matrix2_i  < matrix2.length){
			matrix1_i = row_start;
			int curr_col = matrix2.elems[matrix2_i].j;
			int curr_row = matrix1.elems[matrix1_i].i;
			union{
				float f;
				int i;
			} val;
			if(result.type == MAT_INT)
				val.i = 0;
			else
				val.f = 0.0;
			while(matrix1_i < matrix1.length && matrix2_i < matrix2.length){
				if(matrix1.elems[matrix1_i].j == matrix2.elems[matrix2_i].i){
					if(result.type == MAT_INT)
						val.i += matrix1.elems[matrix1_i].val.i * matrix2.elems[matrix2_i].val.i;
					else
						val.f += matrix1.elems[matrix1_i].val.f * matrix2.elems[matrix2_i].val.f;
					matrix1_i++;
					matrix2_i++;
				}
				else if(matrix1.elems[matrix1_i].j > matrix2.elems[matrix2_i].i)
					matrix2_i++;
				else
					matrix1_i++;
				if(!(matrix2.elems[matrix2_i].j == curr_col && matrix1.elems[matrix1_i].i == curr_row))
					break;
			}
			bool add_val = true;
			if (result.type == MAT_INT){
				if (val.i == 0)
					add_val = false;
			}
			else if(val.f == 0.0)
				add_val = false;
			if(add_val){
				if(result.length == size){
					size *= 2;
					if(!(result.elems = realloc(result.elems, size * sizeof(coo_elem)))){
						fprintf(stderr, "Ran out of virtual memory while allocating result matrix");
						exit(EXIT_FAILURE);
					}
				}
				result.elems[result.length].i = curr_row;
				result.elems[result.length].j = curr_col;
				result.elems[result.length].type = result.type;
				if(result.type == MAT_INT)
					result.elems[result.length].val.i = val.i;
				else
					result.elems[result.length].val.f = val.f;
				result.length++;
			}
			while(matrix2_i < matrix2.length){
				if(matrix2.elems[matrix2_i].j != curr_col)
					break;
				matrix2_i++;
			}
		}
		while(matrix1_i < matrix1.length){
			if(matrix1.elems[row_start].i != matrix1.elems[matrix1_i].i)
				break;
			matrix1_i++;
		}
	}
	get_cpu_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv matrix_multiply(FILE* file1, FILE* file2, FORMAT format)
{
	mat_rv rv;
	switch(format){
	case COO:{
		struct timespec start, end;
		get_cpu_time(&start);
		coo matrix1 = read_coo(file1);
		coo matrix2 = read_coo(file2);
		get_cpu_time(&end);
		struct timespec delta = time_delta(end, start);
		mat_rv rv = matrix_multiply_coo(matrix1, matrix2);
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
