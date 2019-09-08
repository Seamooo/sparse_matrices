#include "main.h"

mat_rv execute_operation(FILE *file1, FILE *file2, OPERATION operation, FORMAT format, float scalar){

	switch(operation){
	case SCAL_MUL:
		//set defaults
		//change after extensive testing for optimal
		if(format == FORM_DEFAULT)
			format = COO;
		return scalar_multiply(file1, format, scalar);
		break;
	case TRACE:
		if(format == FORM_DEFAULT)
			format = COO;
		return trace(file1, format);
		break;
	case ADD:
		if(format == FORM_DEFAULT)
			format = COO;
		return addition(file1, file2, format);
		break;
	case TRANSPOSE:
		if(format == FORM_DEFAULT)
			format = COO;
		return transpose(file1, format);
		break;
	case MAT_MUL:
		if(format == FORM_DEFAULT)
			format = COO;
		return matrix_multiply(file1, file2, format);
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
