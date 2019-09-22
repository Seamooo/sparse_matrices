#include "main.h"

mat_rv execute_operation(OPERATIONARGS *args){

	//handle defaults in the execute_operation functions due to CSR*CSC default
	switch(args->operation){
	case SCAL_MUL:
		return scalar_multiply(args);
		break;
	case TRACE:
		return trace(args);
		break;
	case ADD:
		return addition(args);
		break;
	case TRANSPOSE:
		return transpose(args);
		break;
	case MAT_MUL:
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
