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
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}
//does not implement threading yet
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
	rv = coo_to_mat(result);
	rv.t_process = time_delta(end, start);
	free_coo(result);
	return rv;
}

mat_rv addition(OPERATIONARGS args)
{
	mat_rv rv;
	//not error checking type or format in this function
	switch(args.format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix1 = read_coo(args.file1);
		coo matrix2 = read_coo(args.file2);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args.nothreading)
			rv = addition_coo_nothreading(matrix1, matrix2);
		else
			rv = addition_coo(matrix1, matrix2, args.num_threads);
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
