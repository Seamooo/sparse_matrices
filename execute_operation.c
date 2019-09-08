#include "main.h"

mat_rv execute_operation(OPERATIONARGS args){

	switch(args.operation){
	case SCAL_MUL:
		//set defaults
		//change after extensive testing for optimal
		if(args.format == FORM_DEFAULT)
			args.format = COO;
		return scalar_multiply(args.file1, args.format, args.scalar, args.nothreading);
		break;
	case TRACE:
		if(args.format == FORM_DEFAULT)
			args.format = COO;
		return trace(args.file1, args.format, args.nothreading);
		break;
	case ADD:
		if(args.format == FORM_DEFAULT)
			args.format = COO;
		return addition(args.file1, args.file2, args.format, args.nothreading);
		break;
	case TRANSPOSE:
		if(args.format == FORM_DEFAULT)
			args.format = COO;
		return transpose(args.file1, args.format, args.nothreading);
		break;
	case MAT_MUL:
		if(args.format == FORM_DEFAULT)
			args.format = COO;
		return matrix_multiply(args.file1, args.file2, args.format, args.nothreading);
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
