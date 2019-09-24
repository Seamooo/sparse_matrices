#include "main.h"

mat_rv addition_coo_nothreading(coo matrix1, coo matrix2)
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
	struct timespec start, end;
	get_utc_time(&start);
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix1.cols;
	result.type = matrix1.type;
	result.length = 0;
	int size = MALLOCINIT;
	if(!(result.elems = (coo_elem*)malloc(size*sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int matrix1_i = 0;
	int matrix2_i = 0;
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
	get_utc_time(&end);
	rv = coo_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv addition_coo(coo matrix1, coo matrix2, int thread_count)
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
	struct timespec start, end;
	get_utc_time(&start);
	coo result;
	result.rows = matrix1.rows;
	result.cols = matrix1.cols;
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
	//allocate for worst case such that additional calls to change allocation is not needed
	//by each thread
	for(int i  = 0; i < matrix1.rows; ++i){
		if(!(local_elems[i] = (coo_elem*)malloc(matrix1.cols * sizeof(coo_elem)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//row1 corresponds to matrix1
	//row2 corresponds to matrix2
	int *row1_entries;
	int *row1_lens;
	int *row2_entries;
	int *row2_lens;
	if(!(row1_entries = (int*)malloc(matrix1.rows * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(row1_lens = (int*)calloc(matrix1.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(row2_entries = (int*)malloc(matrix2.rows * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(row2_lens = (int*)calloc(matrix2.rows, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < matrix1.rows; ++i){
		row1_entries[i] = -1;
		row2_entries[i] = -1;
	}
	//Time constants for building makes threading redundant
	for(int i = 0; i < matrix1.length; ++i){
		if(row1_entries[matrix1.elems[i].i] == -1)
			row1_entries[matrix1.elems[i].i] = i;
		row1_lens[matrix1.elems[i].i]++;
	}
	for(int i = 0; i < matrix2.length; ++i){
		if(row2_entries[matrix2.elems[i].i] == -1)
			row2_entries[matrix2.elems[i].i] = i;
		row2_lens[matrix2.elems[i].i]++;
	}
	int i;
	#pragma omp parallel for num_threads(thread_count) private(i) shared(matrix1, matrix2, local_elems)
	for(i = 0; i < matrix1.rows; ++i){
		if(row1_lens[i] == 0 && row2_lens[i] == 0)
			continue;
		int row1_i = 0, row2_i = 0;
		while(row1_i < row1_lens[i] && row2_i < row2_lens[i]){
			if(matrix1.elems[row1_entries[i] + row1_i].j == matrix2.elems[row2_entries[i] + row2_i].j){
				if(result.type == MAT_INT)
					local_elems[i][local_elems_len[i]].val.i = matrix1.elems[row1_entries[i] + row1_i].val.i + matrix2.elems[row2_entries[i] + row2_i].val.i;
				else
					local_elems[i][local_elems_len[i]].val.f = matrix1.elems[row1_entries[i] + row1_i].val.f + matrix2.elems[row2_entries[i] + row2_i].val.f;
				local_elems[i][local_elems_len[i]].j = matrix1.elems[row1_entries[i] + row1_i].j;
				++row2_i;
				++row1_i;
			}
			else if(matrix1.elems[row1_entries[i] + row1_i].j > matrix2.elems[row2_entries[i] + row2_i].j){
				if(result.type == MAT_INT)
					local_elems[i][local_elems_len[i]].val.i = matrix2.elems[row2_entries[i] + row2_i].val.i;
				else
					local_elems[i][local_elems_len[i]].val.f = matrix2.elems[row2_entries[i] + row2_i].val.f;
				local_elems[i][local_elems_len[i]].j = matrix2.elems[row2_entries[i] + row2_i].j;
				++row2_i;
			}
			else{
				if(result.type == MAT_INT)
					local_elems[i][local_elems_len[i]].val.i = matrix1.elems[row1_entries[i] + row1_i].val.i;
				else
					local_elems[i][local_elems_len[i]].val.f = matrix1.elems[row1_entries[i] + row1_i].val.f;
				local_elems[i][local_elems_len[i]].j = matrix1.elems[row1_entries[i] + row1_i].j;
				++row1_i;
			}
			local_elems[i][local_elems_len[i]].i = i;
			local_elems[i][local_elems_len[i]].type = result.type;
			++local_elems_len[i];
		}
		while(row1_i < row1_lens[i]){
			if(result.type == MAT_INT)
				local_elems[i][local_elems_len[i]].val.i = matrix1.elems[row1_entries[i] + row1_i].val.i;
			else
				local_elems[i][local_elems_len[i]].val.f = matrix1.elems[row1_entries[i] + row1_i].val.f;
			local_elems[i][local_elems_len[i]].j = matrix1.elems[row1_entries[i] + row1_i].j;
			local_elems[i][local_elems_len[i]].i = i;
			local_elems[i][local_elems_len[i]].type = result.type;
			++local_elems_len[i];
			++row1_i;
		}
		while(row2_i < row2_lens[i]){
			if(result.type == MAT_INT)
				local_elems[i][local_elems_len[i]].val.i = matrix2.elems[row2_entries[i] + row2_i].val.i;
			else
				local_elems[i][local_elems_len[i]].val.f = matrix2.elems[row2_entries[i] + row2_i].val.f;
			local_elems[i][local_elems_len[i]].j = matrix2.elems[row2_entries[i] + row2_i].j;
			local_elems[i][local_elems_len[i]].i = i;
			local_elems[i][local_elems_len[i]].type = result.type;
			++local_elems_len[i];
			++row2_i;
		}
	}
	free(row1_entries);
	free(row1_lens);
	free(row2_entries);
	free(row2_lens);
	for(i = 0; i < result.rows; ++i)
		result.length += local_elems_len[i];
	if(!(result.elems = malloc(result.length * sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int index = 0;
	for(i = 0; i < result.rows; ++i){
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

mat_rv addition_csr_nothreading(csr matrix1, csr matrix2)
{
	mat_rv rv;
	if(matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols){
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
	result.cols = matrix1.cols;
	result.type = matrix1.type;
	result.num_vals = 0;
	//check worst case between all rows * cols or all elems
	int nnz_size = matrix1.cols*matrix1.rows;
	if(matrix1.num_vals + matrix2.num_vals < nnz_size)
		nnz_size = matrix1.num_vals + matrix2.num_vals;
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(nnz_size * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(nnz_size * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ja = (int*)malloc(nnz_size * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ia = (int*)malloc((result.rows + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.ia[0] = 0;
	for(int i = 0; i < result.rows; ++i){
		result.ia[i + 1] = result.ia[i];
		int mat1_i = matrix1.ia[i];
		int mat2_i = matrix2.ia[i];
		while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[i + 1]){
			if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
				if(result.type == MAT_INT)
					result.nnz.i[result.ia[i + 1]] = matrix1.nnz.i[mat1_i] + matrix2.nnz.i[mat2_i];
				else
					result.nnz.f[result.ia[i + 1]] = matrix1.nnz.f[mat1_i] + matrix2.nnz.f[mat2_i];
				result.ja[result.ia[i + 1]] = matrix1.ja[mat1_i];
				++result.ia[i + 1];
				++mat1_i;
				++mat2_i;
			}
			else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
				++mat2_i;
			else
				++mat1_i;
		}
		while(mat1_i < matrix1.ia[i + 1]){
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[i + 1]] = matrix1.nnz.i[mat1_i];
			else
				result.nnz.f[result.ia[i + 1]] = matrix1.nnz.f[mat1_i];
			result.ja[result.ia[i + 1]] = matrix1.ja[mat1_i];
			++result.ia[i + 1];
			++mat1_i;
		}
		while(mat2_i < matrix2.ia[i + 1]){
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[i + 1]] = matrix2.nnz.i[mat2_i];
			else
				result.nnz.f[result.ia[i + 1]] = matrix2.nnz.f[mat2_i];
			result.ja[result.ia[i + 1]] = matrix2.ja[mat2_i];
			++result.ia[i + 1];
			++mat2_i;
		}
	}
	result.num_vals = result.ia[result.rows];
	get_utc_time(&end);
	rv = csr_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csr(result);
	return rv;
}

mat_rv addition_csr(csr matrix1, csr matrix2, int thread_count)
{
	mat_rv rv;
	if(matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols){
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
	result.cols = matrix1.cols;
	result.type = matrix1.type;
	result.num_vals = 0;
	union{
		int **i;
		long double **f;
	} local_nnzs;
	int **local_jas;
	//allocate for worst case
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
			int mat1_i = matrix1.ia[i];
			int mat2_i = matrix2.ia[i];
			while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[i + 1]){
				if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
					if(local_type == MAT_INT)
						local_nnzs.i[i][result.ia[i + 1]] = matrix1.nnz.i[mat1_i] + matrix2.nnz.i[mat2_i];
					else
						local_nnzs.f[i][result.ia[i + 1]] = matrix1.nnz.f[mat1_i] + matrix2.nnz.f[mat2_i];
					local_jas[i][result.ia[i + 1]] = matrix1.ja[mat1_i];
					result.ia[i + 1]++;
					mat1_i++;
					mat2_i++;
				}
				else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
					mat2_i++;
				else
					mat1_i++;
			}
			while(mat1_i < matrix1.ia[i + 1]){
				if(local_type == MAT_INT)
					local_nnzs.i[i][result.ia[i + 1]] = matrix1.nnz.i[mat1_i];
				else
					local_nnzs.f[i][result.ia[i + 1]] = matrix1.nnz.f[mat1_i];
				local_jas[i][result.ia[i + 1]] = matrix1.ja[mat1_i];
				++result.ia[i + 1];
				++mat1_i;
			}
			while(mat2_i < matrix2.ia[i + 1]){
				if(local_type == MAT_INT)
					local_nnzs.i[i][result.ia[i + 1]] = matrix2.nnz.i[mat2_i];
				else
					local_nnzs.f[i][result.ia[i + 1]] = matrix2.nnz.f[mat2_i];
				local_jas[i][result.ia[i + 1]] = matrix2.ja[mat2_i];
				++result.ia[i + 1];
				++mat2_i;
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

mat_rv addition_csc_nothreading(csc matrix1, csc matrix2)
{
	mat_rv rv;
	if(matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	if(matrix1.type != matrix2.type){
		rv.error = ERR_TYPE_MISSMATCH;
		return rv;
	}
	struct timespec start, end;
	get_utc_time(&start);
	csc result;
	result.cols = matrix1.cols;
	result.rows = matrix1.rows;
	result.type = matrix1.type;
	result.num_vals = 0;
	//check worst case between all cols * rows or all elems
	int nnz_size = matrix1.rows*matrix1.cols;
	if(matrix1.num_vals + matrix2.num_vals < nnz_size)
		nnz_size = matrix1.num_vals + matrix2.num_vals;
	if(result.type == MAT_INT){
		if(!(result.nnz.i = (int*)malloc(nnz_size * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(result.nnz.f = (long double*)malloc(nnz_size * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(result.ja = (int*)malloc(nnz_size * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	if(!(result.ia = (int*)malloc((result.cols + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	result.ia[0] = 0;
	for(int i = 0; i < result.cols; ++i){
		result.ia[i + 1] = result.ia[i];
		int mat1_i = matrix1.ia[i];
		int mat2_i = matrix2.ia[i];
		while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[i + 1]){
			if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
				if(result.type == MAT_INT)
					result.nnz.i[result.ia[i + 1]] = matrix1.nnz.i[mat1_i] + matrix2.nnz.i[mat2_i];
				else
					result.nnz.f[result.ia[i + 1]] = matrix1.nnz.f[mat1_i] + matrix2.nnz.f[mat2_i];
				result.ja[result.ia[i + 1]] = matrix1.ja[mat1_i];
				++result.ia[i + 1];
				++mat1_i;
				++mat2_i;
			}
			else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
				++mat2_i;
			else
				++mat1_i;
		}
		while(mat1_i < matrix1.ia[i + 1]){
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[i + 1]] = matrix1.nnz.i[mat1_i];
			else
				result.nnz.f[result.ia[i + 1]] = matrix1.nnz.f[mat1_i];
			result.ja[result.ia[i + 1]] = matrix1.ja[mat1_i];
			++result.ia[i + 1];
			++mat1_i;
		}
		while(mat2_i < matrix2.ia[i + 1]){
			if(result.type == MAT_INT)
				result.nnz.i[result.ia[i + 1]] = matrix2.nnz.i[mat2_i];
			else
				result.nnz.f[result.ia[i + 1]] = matrix2.nnz.f[mat2_i];
			result.ja[result.ia[i + 1]] = matrix2.ja[mat2_i];
			++result.ia[i + 1];
			++mat2_i;
		}
	}
	result.num_vals = result.ia[result.cols];
	get_utc_time(&end);
	rv = csc_to_mat_nothreading(result);
	rv.t_process = time_delta(end, start);
	free_csc(result);
	return rv;
}

mat_rv addition_csc(csc matrix1, csc matrix2, int thread_count)
{
	mat_rv rv;
	if(matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols){
		rv.error = ERR_WRONG_DIM;
		return rv;
	}
	if(matrix1.type != matrix2.type){
		rv.error = ERR_TYPE_MISSMATCH;
		return rv;
	}
	struct timespec start, end;
	get_utc_time(&start);
	csc result;
	result.cols = matrix1.cols;
	result.rows = matrix1.rows;
	result.type = matrix1.type;
	result.num_vals = 0;
	union{
		int **i;
		long double **f;
	} local_nnzs;
	int **local_jas;
	//allocate for worst case
	if(result.type == MAT_INT){
		if(!(local_nnzs.i = (int**)malloc(result.cols*sizeof(int *)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < result.cols; ++i){
			if(!(local_nnzs.i[i] = (int*)malloc(result.rows*sizeof(int)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else{
		if(!(local_nnzs.f = (long double**)malloc(result.cols*sizeof(long double *)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < result.cols; ++i){
			if(!(local_nnzs.f[i] = (long double*)malloc(result.rows*sizeof(long double)))){
				fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	if(!(local_jas = (int**)malloc(result.cols*sizeof(int *)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < result.cols; ++i){
		if(!(local_jas[i] = (int*)malloc(result.rows*sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
			exit(EXIT_FAILURE);
		}
	}
	//using result.ia to store length of each row then iterate through at the end
	if(!(result.ia = (int*)calloc(result.cols + 1, sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory while allocating result matrix\n");
		exit(EXIT_FAILURE);
	}
	int i;
	#pragma omp parallel num_threads(thread_count) shared(matrix1, matrix2, result, local_nnzs, local_jas)
	{
		//store type on the local stack
		MAT_TYPE local_type = result.type;
		#pragma omp for private(i)
		for(i = 0; i < result.cols; ++i){
			int mat1_i = matrix1.ia[i];
			int mat2_i = matrix2.ia[i];
			while(mat1_i < matrix1.ia[i + 1] && mat2_i < matrix2.ia[i + 1]){
				if(matrix1.ja[mat1_i] == matrix2.ja[mat2_i]){
					if(local_type == MAT_INT)
						local_nnzs.i[i][result.ia[i + 1]] = matrix1.nnz.i[mat1_i] + matrix2.nnz.i[mat2_i];
					else
						local_nnzs.f[i][result.ia[i + 1]] = matrix1.nnz.f[mat1_i] + matrix2.nnz.f[mat2_i];
					local_jas[i][result.ia[i + 1]] = matrix1.ja[mat1_i];
					result.ia[i + 1]++;
					mat1_i++;
					mat2_i++;
				}
				else if(matrix1.ja[mat1_i] > matrix2.ja[mat2_i])
					mat2_i++;
				else
					mat1_i++;
			}
			while(mat1_i < matrix1.ia[i + 1]){
				if(local_type == MAT_INT)
					local_nnzs.i[i][result.ia[i + 1]] = matrix1.nnz.i[mat1_i];
				else
					local_nnzs.f[i][result.ia[i + 1]] = matrix1.nnz.f[mat1_i];
				local_jas[i][result.ia[i + 1]] = matrix1.ja[mat1_i];
				++result.ia[i + 1];
				++mat1_i;
			}
			while(mat2_i < matrix2.ia[i + 1]){
				if(local_type == MAT_INT)
					local_nnzs.i[i][result.ia[i + 1]] = matrix2.nnz.i[mat2_i];
				else
					local_nnzs.f[i][result.ia[i + 1]] = matrix2.nnz.f[mat2_i];
				local_jas[i][result.ia[i + 1]] = matrix2.ja[mat2_i];
				++result.ia[i + 1];
				++mat2_i;
			}
		}
	}
	for(int i = 1; i < result.cols + 1; ++i)
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
	for(int i = 0; i < result.cols; ++i){
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
	rv = csc_to_mat(result, thread_count);
	rv.t_process = time_delta(end, start);
	free_csc(result);
	return rv;
}

mat_rv addition(OPERATIONARGS *args)
{
	mat_rv rv;
	//default = COO
	if (args->format == FORM_DEFAULT)
		args->format = COO;
	switch(args->format){
	case COO:{
		struct timespec delta1, delta2;
		coo matrix1 = read_coo(args->file1, &delta1);
		coo matrix2 = read_coo(args->file2, &delta2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = addition_coo_nothreading(matrix1, matrix2);
		else
			rv = addition_coo(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		free_coo(matrix1);
		free_coo(matrix2);
		return rv;
		break;
	}
	case CSR:{
		struct timespec delta1, delta2;
		csr matrix1 = read_csr(args->file1, &delta1);
		csr matrix2 = read_csr(args->file2, &delta2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = addition_csr_nothreading(matrix1, matrix2);
		else
			rv = addition_csr(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
		free_csr(matrix1);
		free_csr(matrix2);
		return rv;
		break;
	}
	case CSC:{
		struct timespec delta1, delta2;
		csc matrix1 = read_csc(args->file1, &delta1);
		csc matrix2 = read_csc(args->file2, &delta2);
		struct timespec construct = time_sum(delta1, delta2);
		if(args->nothreading)
			rv = addition_csc_nothreading(matrix1, matrix2);
		else
			rv = addition_csc(matrix1, matrix2, args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, construct);
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
