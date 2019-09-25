#include "main.h"

mat_rv matrix_multiply_csr_csc_nothreading(csr matrix1, csc matrix2)
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
	csr result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.num_vals = 0;
	if(!(result.ja = (int*)malloc(result.rows*result.cols*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.rows*result.cols*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.rows*result.cols*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ia = (int*)malloc(result.rows*result.cols*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
	}
	result.ia[0] = 0;
	for(int i = 0; i < result.rows; ++i){
		result.ia[i + 1] = result.ia[i];
		for(int j = 0; j < result.cols; ++j){
			int mat1_i = matrix1.ia[i];
			int mat2_i = matrix2.ia[j];
			union{
				int i;
				long double f;
			} val;
			if (result.type == MAT_INT)
				val.i = 0;
			else
				val.f = 0.0;
			while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[j + 1]){

				if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
					if(result.type == MAT_INT)
						val.i += matrix1.nnz.i[mat1_i] * matrix2.nnz.i[mat2_i];
					else
						val.f += matrix1.nnz.f[mat1_i] * matrix2.nnz.f[mat2_i];
					++mat1_i;
					++mat2_i;
				}
				else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
					++mat2_i;
				else
					++mat1_i;
			}
			if(result.type == MAT_INT){
				if(val.i == 0)
					continue;
			}
			else{
				if(val.f == 0.0)
					continue;
			}
			result.ja[result.ia[i + 1]] = j;
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[i + 1]] = val.i;
			else
				result.nnz.f[result.ia[i + 1]] = val.f;
			result.ia[i + 1]++;
		}
	}
	result.num_vals = result.ia[result.rows];
	get_utc_time(&end);
	rv = csr_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv matrix_multiply_csr_csc(csr matrix1, csc matrix2, int thread_count)
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
	csr result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.num_vals = 0;
	union{
		int **i;
		long double **f;
	} local_nnzs;
	int **local_jas;
	if(result.type == MAT_INT){
		if(!(local_nnzs.i = (int**)malloc(result.rows*sizeof(int *)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < result.rows; ++i){
			if(!(local_nnzs.i[i] = (int*)malloc(result.cols*sizeof(int)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else{
		if(!(local_nnzs.f = (long double**)malloc(result.rows*sizeof(long double *)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < result.rows; ++i){
			if(!(local_nnzs.f[i] = (long double*)malloc(result.cols*sizeof(long double)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	if(!(local_jas = (int**)malloc(result.rows*sizeof(int *)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < result.rows; ++i){
		if(!(local_jas[i] = (int*)malloc(result.cols*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//using result.ia to store length of each row then iterate through at the end
	if(!(result.ia = (int*)calloc(result.rows + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix1, matrix2, result, local_nnzs, local_jas)
	{
		//store type on the local stack
		MAT_TYPE local_type = result.type;
		#pragma omp for private(i)
		for(i = 0; i < result.rows; ++i){
			for(int j = 0; j < result.cols; ++j){
				int mat1_i = matrix1.ia[i];
				int mat2_i = matrix2.ia[j];
				union{
					int i;
					long double f;
				} val;
				if (result.type == MAT_INT)
					val.i = 0;
				else
					val.f = 0.0;
				while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[j + 1]){
					if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
						if(local_type == MAT_INT)
							val.i += matrix1.nnz.i[mat1_i] * matrix2.nnz.i[mat2_i];
						else
							val.f += matrix1.nnz.f[mat1_i] * matrix2.nnz.f[mat2_i];
						++mat1_i;
						++mat2_i;
					}
					else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
						++mat2_i;
					else
						++mat1_i;
				}
				if(local_type == MAT_INT){
					if(val.i == 0)
						continue;
				}
				else{
					if(val.f == 0.0)
						continue;
				}
				local_jas[i][result.ia[i + 1]] = j;
				if(local_type == MAT_INT)
					local_nnzs.i[i][result.ia[i + 1]] = val.i;
				else
					local_nnzs.f[i][result.ia[i + 1]] = val.f;
				result.ia[i + 1]++;
			}
		}
	}
	for(int i = 1; i < result.rows + 1; ++i)
		result.num_vals += result.ia[i];
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(result.num_vals * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(result.num_vals * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ja = (int*)malloc(result.num_vals * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	//can't parallelise this as it has previous dependencies
	//result.ia[i + 1] is storing length of row
	//end of the loop sets it to store actual ia val
	for(int i = 0; i < result.rows; ++i){
		if(result.ia[i + 1] > 0){
			if(result.type == MAT_INT){
				memcpy(&result.nnz.i[result.ia[i]],local_nnzs.i[i], result.ia[i + 1] * sizeof(int));
				free(local_nnzs.i[i]);
			}
			else{
				memcpy(&result.nnz.f[result.ia[i]],local_nnzs.f[i], result.ia[i + 1] * sizeof(long double));
				free(local_nnzs.f[i]);
			}
			memcpy(&result.ja[result.ia[i]], local_jas[i], result.ia[i + 1] * sizeof(int));
			free(local_jas[i]);
		}
		result.ia[i + 1] += result.ia[i];
	}
	free(local_jas);
	if(result.type == MAT_INT)
		free(local_nnzs.i);
	else
		free(local_nnzs.f);
	get_utc_time(&end);
	rv = csr_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

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
	struct timespec start, end;
	get_utc_time(&start);
	sort_coo(matrix2,ORDER_COL);
	//don't need to sort matrix1 as it's already in reduced forms
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.length = 0;
	int size = MALLOCINIT;
	if(!(result.elems = malloc(size * sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int matrix1_i = 0;
	int matrix2_i = 0;
	while(matrix1_i < matrix1.length){
		int row_start = matrix1_i;
		matrix2_i = 0;
		while(matrix2_i  < matrix2.length){
			matrix1_i = row_start;
			int curr_col = matrix2.elems[matrix2_i].j;
			int curr_row = matrix1.elems[matrix1_i].i;
			union{
				long double f;
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
						fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
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
	rv = coo_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

//check to see whether adding some vars to the local stack gives a speedup
mat_rv matrix_multiply_coo(coo matrix1, coo matrix2, int thread_count)
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
	//largest unthreaded complexity
	sort_coo(matrix2,ORDER_COL);
	//don't need to sort matrix1 as it's already in row column order
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix2.cols;
	result.type = matrix1.type;
	result.length = 0;
	//local storage elems
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
	//my worrying unavoidable time constants for COO
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
	#pragma omp parallel for num_threads(thread_count) private(i, j) shared(matrix1, matrix2, local_elems)
	for(i = 0; i < matrix1.rows; ++i){
		for(j = 0; j < matrix2.cols; ++j){
			if(row_lens[i] == 0)
				break;
			if(col_lens[i] == 0)
				continue;
			union{
				int i;
				long double f;
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
	free(local_elems);
	free(local_elems_len);
	get_utc_time(&end);
	rv = coo_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv matrix_multiply_csr_nothreading(csr matrix1, csr matrix2)
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
	//convert matrix2 to csc and send to mat_mul_csr_csc
	struct timespec start, end;
	get_utc_time(&start);
	csc csc_mat;
	csc_mat.cols = matrix2.cols;
	csc_mat.rows = matrix2.rows;
	csc_mat.type = matrix2.type;
	csc_mat.num_vals = matrix2.num_vals;
	if(!(csc_mat.ia = (int*)calloc(csc_mat.cols + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
		exit(EXIT_FAILURE);
	}
	if(!(csc_mat.ja = (int*)malloc(csc_mat.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
		exit(EXIT_FAILURE);
	}
	if(csc_mat.type == MAT_INT){
		if(!(csc_mat.nnz.i = (int*)malloc(csc_mat.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(csc_mat.nnz.f = (long double*)malloc(csc_mat.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	int *col_lens;
	if(!(col_lens = (int*)calloc(csc_mat.cols, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating col_lens\n");
		exit(EXIT_FAILURE);
	}
	//store lengths of columns in csc_mat.ia
	for(int i = 0; i < matrix2.num_vals; ++i)
		csc_mat.ia[matrix2.ja[i] + 1]++;
	//calculate ia
	for(int i = 0; i < csc_mat.cols; ++i)
		csc_mat.ia[i + 1] += csc_mat.ia[i];
	for(int i = 0; i < matrix2.rows; ++i){
		for(int j = matrix2.ia[i]; j < matrix2.ia[i + 1]; ++j){
			if(csc_mat.type == MAT_INT)
				csc_mat.nnz.i[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]] = matrix2.nnz.i[j];
			else
				csc_mat.nnz.f[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]] = matrix2.nnz.f[j];
			csc_mat.ja[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]++] = i;
		}
	}
	free(col_lens);
	get_utc_time(&end);
	rv = matrix_multiply_csr_csc_nothreading(matrix1, csc_mat);
	struct timespec delta = time_delta(end, start);
	rv.t_process = time_sum(rv.t_process, delta);
	free_csc(csc_mat);
	return rv;
}

mat_rv matrix_multiply_csr(csr matrix1, csr matrix2, int thread_count)
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
	//convert matrix2 to csc and send to mat_mul_csr_csc
	struct timespec start, end;
	get_utc_time(&start);
	csc csc_mat;
	csc_mat.cols = matrix2.cols;
	csc_mat.rows = matrix2.rows;
	csc_mat.type = matrix2.type;
	csc_mat.num_vals = matrix2.num_vals;
	if(!(csc_mat.ia = (int*)calloc(csc_mat.cols + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
		exit(EXIT_FAILURE);
	}
	if(!(csc_mat.ja = (int*)malloc(csc_mat.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
		exit(EXIT_FAILURE);
	}
	if(csc_mat.type == MAT_INT){
		if(!(csc_mat.nnz.i = (int*)malloc(csc_mat.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(csc_mat.nnz.f = (long double*)malloc(csc_mat.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csc_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	int *col_lens;
	if(!(col_lens = (int*)calloc(csc_mat.cols, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating col_lens\n");
		exit(EXIT_FAILURE);
	}
	//store lengths of columns in csc_mat.ia
	for(int i = 0; i < matrix2.num_vals; ++i)
		csc_mat.ia[matrix2.ja[i] + 1]++;
	//calculate ia
	for(int i = 0; i < csc_mat.cols; ++i)
		csc_mat.ia[i + 1] += csc_mat.ia[i];
	//cannot parallelise below
	for(int i = 0; i < matrix2.rows; ++i){
		for(int j = matrix2.ia[i]; j < matrix2.ia[i + 1]; ++j){
			//col_lens tracks smallest to largest
			if(csc_mat.type == MAT_INT)
				csc_mat.nnz.i[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]] = matrix2.nnz.i[j];
			else
				csc_mat.nnz.f[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]] = matrix2.nnz.f[j];
			csc_mat.ja[csc_mat.ia[matrix2.ja[j]] + col_lens[matrix2.ja[j]]++] = i;
		}
	}
	free(col_lens);
	get_utc_time(&end);
	rv = matrix_multiply_csr_csc(matrix1, csc_mat, thread_count);
	struct timespec delta = time_delta(end, start);
	rv.t_process = time_sum(rv.t_process, delta);
	free_csc(csc_mat);
	return rv;
}

mat_rv matrix_multiply_csc_nothreading(csc matrix1, csc matrix2)
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
	//convert matrix1 to csr and send to mat_mul_csr_csc
	struct timespec start, end;
	get_utc_time(&start);
	csr csr_mat;
	csr_mat.cols = matrix1.cols;
	csr_mat.rows = matrix1.rows;
	csr_mat.type = matrix1.type;
	csr_mat.num_vals = matrix1.num_vals;
	if(!(csr_mat.ia = (int*)calloc(csr_mat.rows + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
		exit(EXIT_FAILURE);
	}
	if(!(csr_mat.ja = (int*)malloc(csr_mat.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
		exit(EXIT_FAILURE);
	}
	if(csr_mat.type == MAT_INT){
		if(!(csr_mat.nnz.i = (int*)malloc(csr_mat.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(csr_mat.nnz.f = (long double*)malloc(csr_mat.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	int *row_lens;
	if(!(row_lens = (int*)calloc(csr_mat.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating row_lens\n");
		exit(EXIT_FAILURE);
	}
	//store lengths of columns in csr_mat.ia
	for(int i = 0; i < matrix1.num_vals; ++i)
		csr_mat.ia[matrix1.ja[i] + 1]++;
	//calculate ia
	for(int i = 0; i < csr_mat.rows; ++i)
		csr_mat.ia[i + 1] += csr_mat.ia[i];
	for(int i = 0; i < matrix1.cols; ++i){
		for(int j = matrix1.ia[i]; j < matrix1.ia[i + 1]; ++j){
			if(csr_mat.type == MAT_INT)
				csr_mat.nnz.i[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]] = matrix1.nnz.i[j];
			else
				csr_mat.nnz.f[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]] = matrix1.nnz.f[j];
			csr_mat.ja[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]++] = i;
		}
	}
	free(row_lens);
	get_utc_time(&end);
	rv = matrix_multiply_csr_csc_nothreading(csr_mat, matrix2);
	struct timespec delta = time_delta(end, start);
	rv.t_process = time_sum(rv.t_process, delta);
	free_csr(csr_mat);
	return rv;
}

mat_rv matrix_multiply_csc(csc matrix1, csc matrix2, int thread_count)
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
	//convert matrix1 to csr and send to mat_mul_csr_csc
	struct timespec start, end;
	get_utc_time(&start);
	csr csr_mat;
	csr_mat.cols = matrix1.cols;
	csr_mat.rows = matrix1.rows;
	csr_mat.type = matrix1.type;
	csr_mat.num_vals = matrix1.num_vals;
	if(!(csr_mat.ia = (int*)calloc(csr_mat.rows + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
		exit(EXIT_FAILURE);
	}
	if(!(csr_mat.ja = (int*)malloc(csr_mat.num_vals*sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
		exit(EXIT_FAILURE);
	}
	if(csr_mat.type == MAT_INT){
		if(!(csr_mat.nnz.i = (int*)malloc(csr_mat.num_vals*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(csr_mat.nnz.f = (long double*)malloc(csr_mat.num_vals*sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating csr_mat\n");
			exit(EXIT_FAILURE);
		}
	}
	int *row_lens;
	if(!(row_lens = (int*)calloc(csr_mat.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating row_lens\n");
		exit(EXIT_FAILURE);
	}
	//store lengths of columns in csr_mat.ia
	for(int i = 0; i < matrix1.num_vals; ++i)
		csr_mat.ia[matrix1.ja[i] + 1]++;
	//calculate ia
	for(int i = 0; i < csr_mat.rows; ++i)
		csr_mat.ia[i + 1] += csr_mat.ia[i];
	for(int i = 0; i < matrix1.cols; ++i){
		for(int j = matrix1.ia[i]; j < matrix1.ia[i + 1]; ++j){
			if(csr_mat.type == MAT_INT)
				csr_mat.nnz.i[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]] = matrix1.nnz.i[j];
			else
				csr_mat.nnz.f[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]] = matrix1.nnz.f[j];
			csr_mat.ja[csr_mat.ia[matrix1.ja[j]] + row_lens[matrix1.ja[j]]++] = i;
		}
	}
	free(row_lens);
	get_utc_time(&end);
	rv = matrix_multiply_csr_csc(csr_mat, matrix2, thread_count);
	struct timespec delta = time_delta(end, start);
	rv.t_process = time_sum(rv.t_process, delta);
	free_csr(csr_mat);
	return rv;
}

mat_rv matrix_multiply(OPERATIONARGS *args)
{
	mat_rv rv;
	//default = csr * csc
	switch(args->format){
	case FORM_DEFAULT:{
		struct timespec delta1, delta2;
		struct timespec fileio1, fileio2;
		csr matrix1 = read_csr(args->file1, &delta1, &fileio1);
		csc matrix2 = read_csc(args->file2, &delta2, &fileio2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = matrix_multiply_csr_csc_nothreading(matrix1, matrix2);
		else
			rv = matrix_multiply_csr_csc(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = time_sum(fileio1, fileio2);
		free_csr(matrix1);
		free_csc(matrix2);
		return rv;
		break;
	}
	case COO:{
		struct timespec delta1, delta2;
		struct timespec fileio1, fileio2;
		coo matrix1 = read_coo(args->file1, &delta1, &fileio1);
		coo matrix2 = read_coo(args->file2, &delta2, &fileio2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = matrix_multiply_coo_nothreading(matrix1, matrix2);
		else
			rv = matrix_multiply_coo(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = time_sum(fileio1, fileio2);
		free_coo(matrix1);
		free_coo(matrix2);
		return rv;
		break;
	}
	case CSR:{
		struct timespec delta1, delta2;
		struct timespec fileio1, fileio2;
		csr matrix1 = read_csr(args->file1, &delta1, &fileio1);
		csr matrix2 = read_csr(args->file2, &delta2, &fileio2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = matrix_multiply_csr_nothreading(matrix1, matrix2);
		else
			rv = matrix_multiply_csr(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = time_sum(fileio1, fileio2);
		free_csr(matrix1);
		free_csr(matrix2);
		return rv;
		break;
	}
	case CSC:{
		struct timespec delta1, delta2;
		struct timespec fileio1, fileio2;
		csc matrix1 = read_csc(args->file1, &delta1, &fileio1);
		csc matrix2 = read_csc(args->file2, &delta2, &fileio2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = matrix_multiply_csc_nothreading(matrix1, matrix2);
		else
			rv = matrix_multiply_csc(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		rv.t_fileio = time_sum(fileio1, fileio2);
		free_csc(matrix1);
		free_csc(matrix2);
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
