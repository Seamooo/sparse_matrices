#include "main.h"

mat_rv scalar_multiply_coo(coo matrix, float scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_cpu_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].type == MAT_INT)
			matrix.elems[i].val.f = (float)matrix.elems[i].val.i * scalar;
		else
			matrix.elems[i].val.f = matrix.elems[i].val.f * scalar;
		matrix.elems[i].type = MAT_FLOAT;
	}
	get_cpu_time(&end);
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply(FILE* file, FORMAT format, float scalar)
{
	mat_rv rv;
	//not error checking type or format in this function
	switch(format){
	case COO:{
		struct timespec start, end;
		get_cpu_time(&start);
		coo matrix = read_coo(file);
		get_cpu_time(&end);
		struct timespec delta = time_delta(end, start);
		mat_rv rv = scalar_multiply_coo(matrix, scalar);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
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
