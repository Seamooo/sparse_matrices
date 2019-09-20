#include "main.h"

mat_rv execute_operation(OPERATIONARGS *args){

	switch(args->operation){
	case SCAL_MUL:
		//set defaults
		//change after extensive testing for optimal
		if(args->format == FORM_DEFAULT)
			args->format = COO;
		return scalar_multiply(args);
		break;
	case TRACE:
		if(args->format == FORM_DEFAULT)
			args->format = COO;
		return trace(args);
		break;
	case ADD:
		if(args->format == FORM_DEFAULT)
			args->format = COO;
		return addition(args);
		break;
	case TRANSPOSE:
		if(args->format == FORM_DEFAULT)
			args->format = COO;
		return transpose(args);
		break;
	case MAT_MUL:
		if(args->format == FORM_DEFAULT)
			args->format = COO;
		return matrix_multiply(args);
		break;
	default:
		fprintf(stderr, "Internal error selecting operation\n");
		exit(EXIT_FAILURE);
	}
	//below should never be reached
	fprintf(stderr, "Control flow error @ execute_operation\n");
	exit(EXIT_FAILURE);
	//below will never be reacbed
	mat_rv rv;
	return rv;
}
