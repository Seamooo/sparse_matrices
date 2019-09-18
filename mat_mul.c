#include "main.h"

//nothreading 100% works
mat_rv matrix_multiply_coo_nothreading(coo matrix1, coo matrix2)
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
	get_utc_time(&start);
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
	get_utc_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

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
	struct timespec start, end;
	get_utc_time(&start);
	sort_coo(matrix2,ORDER_COL);
	//don't need to sort matrix1 as it's already in reduced forms
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.length = 0;
	//local storgae elems
	coo_elem **local_elems;
	int *local_elems_len;
	if(!(local_elems = (coo_elem**)malloc(matrix1.rows * sizeof(coo_elem *)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(local_elems_len = (int*)calloc(matrix1.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	for(int i  = 0; i < matrix1.rows; ++i){
		if(!(local_elems[i] = (coo_elem*)malloc(matrix2.cols * sizeof(coo_elem)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	int *row_entries;
	int *row_lens;
	int *col_entries;
	int *col_lens;
	if(!(row_entries = (int*)malloc(matrix1.rows * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(row_lens = (int*)calloc(matrix1.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(col_entries = (int*)malloc(matrix2.cols * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(col_lens = (int*)calloc(matrix2.cols, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < matrix1.rows; ++i){
		row_entries[i] = -1;
		col_entries[i] = -1;
	}
	//my worrying unavoidable time constants
	for(int i = 0; i < matrix1.length; ++i){
		if(row_entries[matrix1.elems[i].i] == -1)
			row_entries[matrix1.elems[i].i] = i;
		row_lens[matrix1.elems[i].i]++;
	}
	for(int i = 0; i < matrix2.length; ++i){
		if(col_entries[matrix2.elems[i].j] == -1)
			col_entries[matrix2.elems[i].j] = i;
		col_lens[matrix2.elems[i].j]++;
	}
	int i, j;
	#pragma omp parallel for private(i, j) shared(matrix1, matrix2, local_elems)
	for(i = 0; i < matrix1.rows; ++i){
		for(j = 0; j < matrix2.cols; ++j){
			if(row_lens[i] == 0)
				break;
			if(col_lens[i] == 0)
				continue;
			union{
				int i;
				float f;
			} local_result;
			if(result.type == MAT_INT)
				local_result.i = 0;
			else
				local_result.f = 0.0;
			int mat1_i = 0;
			int mat2_i = 0;
			while(mat1_i < row_lens[i] && mat2_i < col_lens[j]){
				if(matrix1.elems[row_entries[i] + mat1_i].j == matrix2.elems[col_entries[j] + mat2_i].i){
					if(result.type == MAT_INT)
						local_result.i += matrix1.elems[row_entries[i] + mat1_i].val.i * matrix2.elems[col_entries[j] + mat2_i].val.i;
					else
						local_result.f += matrix1.elems[row_entries[i] + mat1_i].val.f * matrix2.elems[col_entries[j] + mat2_i].val.f;
					mat2_i++;
					mat1_i++;
				}
				else if(matrix1.elems[row_entries[i] + mat1_i].j > matrix2.elems[col_entries[j] + mat2_i].i)
					mat2_i++;
				else
					mat1_i++;
			}
			if(result.type == MAT_INT){
				if(local_result.i == 0)
					continue;
			}
			else{
				if(local_result.f == 0.0)
					continue;
			}
			local_elems[i][local_elems_len[i]].i = i;
			local_elems[i][local_elems_len[i]].j = j;
			local_elems[i][local_elems_len[i]].type = result.type;
			if(result.type == MAT_INT)
				local_elems[i][local_elems_len[i]].val.i = local_result.i;
			else
				local_elems[i][local_elems_len[i]].val.f = local_result.f;
			local_elems_len[i]++;
		}
	}
	free(row_entries);
	free(row_lens);
	free(col_entries);
	free(col_lens);
	for(i = 0; i < matrix1.rows; ++i)
		result.length += local_elems_len[i];
	if(!(result.elems = malloc(result.length * sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int index = 0;
	for(i = 0; i < matrix1.rows; ++i){
		memcpy(&result.elems[index], local_elems[i], local_elems_len[i] * sizeof(coo_elem));
		index += local_elems_len[i];
		free(local_elems[i]);
	}
	get_utc_time(&end);
	free(local_elems);
	free(local_elems_len);
	//get_utc_time(&end);
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv matrix_multiply(OPERATIONARGS args)
{
	mat_rv rv;
	switch(args.format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix1 = read_coo(args.file1);
		coo matrix2 = read_coo(args.file2);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args.nothreading)
			rv = matrix_multiply_coo_nothreading(matrix1, matrix2);
		else
			rv = matrix_multiply_coo(matrix1, matrix2);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix1);
		free_coo(matrix2);
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
