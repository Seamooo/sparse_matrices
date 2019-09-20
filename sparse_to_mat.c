#include "main.h"

mat_rv coo_to_mat(coo matrix){
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
		rv.type = MAT_FLOAT;
		if(!(rv.vals.f = (float*)malloc(rv.rows * rv.cols * sizeof(float)))){
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
