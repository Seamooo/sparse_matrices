#include "main.h"

mat_rv scalar_multiply_coo_nothreading(coo matrix, SCALAR *scalar)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.type == MAT_INT){
			if(scalar->type == MAT_INT)
				matrix.elems[i].val.i = matrix.elems[i].val.i * scalar->val.i;
			else
				matrix.elems[i].val.f = (long double)matrix.elems[i].val.i * scalar->val.f;
		}
		else{
			if(scalar->type == MAT_INT)
				matrix.elems[i].val.f = matrix.elems[i].val.f * (long double)scalar->val.i;
			else
				matrix.elems[i].val.f = matrix.elems[i].val.f * scalar->val.f;
		}
		if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
			matrix.elems[i].type = MAT_LDOUBLE;
	}
	get_utc_time(&end);
	//change matrix type if necessary
	if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
		matrix.type = MAT_LDOUBLE;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply_coo(coo matrix, SCALAR *scalar, int thread_count)
{
	mat_rv rv;
	struct timespec start, end;
	get_utc_time(&start);
	int i;
	#pragma omp parallel num_threads(thread_count) shared(scalar, matrix)
	{
		MAT_TYPE local_mat_type = matrix.type;
		//store scalar on the local stack
		SCALAR local_scalar;
		local_scalar.type = scalar->type;
		local_scalar.val = scalar->val;
		#pragma omp for private(i)
		for(i = 0; i < matrix.length; ++i){
			if(local_mat_type == MAT_INT){
				if(local_scalar.type == MAT_INT)
					matrix.elems[i].val.i = matrix.elems[i].val.i * local_scalar.val.i;
				else
					matrix.elems[i].val.f = (long double)matrix.elems[i].val.i * local_scalar.val.f;
			}
			else{
				if(local_scalar.type == MAT_INT)
					matrix.elems[i].val.f = matrix.elems[i].val.f * (long double)local_scalar.val.i;
				else
					matrix.elems[i].val.f = matrix.elems[i].val.f * local_scalar.val.f;
			}
			if((local_mat_type == MAT_INT && local_scalar.type == MAT_LDOUBLE))
				matrix.elems[i].type = MAT_LDOUBLE;
		}
	}
	get_utc_time(&end);
	//change matrix type if necessary
	if((matrix.type == MAT_INT && scalar->type == MAT_LDOUBLE))
		matrix.type = MAT_LDOUBLE;
	rv = coo_to_mat(matrix);
	rv.t_process = time_delta(end, start);
	return rv;
}

mat_rv scalar_multiply(OPERATIONARGS *args)
{
	mat_rv rv;
	//default = COO
	if (args->format == FORM_DEFAULT)
		args->format = COO;
	switch(args->format){
	case COO:{
		struct timespec start, end;
		get_utc_time(&start);
		coo matrix = read_coo(args->file1);
		get_utc_time(&end);
		struct timespec delta = time_delta(end, start);
		if(args->nothreading)
			rv = scalar_multiply_coo_nothreading(matrix, &(args->scalar));
		else
			rv = scalar_multiply_coo(matrix, &(args->scalar), args->num_threads);
		rv.t_construct = time_sum(rv.t_construct, delta);
		free_coo(matrix);
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
