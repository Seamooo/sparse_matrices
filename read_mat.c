#include "main.h"

MAT_TYPE get_type(FILE *file){
	char *type_str;
	type_str = readline(file);
	MAT_TYPE type = MAT_NONE;
	switch(strlen(type_str)){
	case 3:
		if(strncmp("int", type_str, 3*sizeof(char)) == 0)
			type = MAT_INT;
		else{
			fprintf(stderr, "unknown type: %s provided in input file\n", type_str);
			exit(EXIT_FAILURE);
		}
		break;
	case 4:
		if(strncmp("float", type_str, 4*sizeof(char)) == 0)
			type = MAT_FLOAT;
		else{
			fprintf(stderr, "unknown type: %s provided in input file\n", type_str);
			exit(EXIT_FAILURE);
		}
		break;
	default:
		fprintf(stderr, "unknown type: %s provided in input file\n", type_str);
		exit(EXIT_FAILURE);
	}
	free(type_str);
	return type;
}

int read_int_token(char *line, int *start)
{
	int size = MALLOCINIT;
	char *rv_str;
	if(!(rv_str = malloc(size *sizeof(char)))){
		fprintf(stderr, "Ran out of virtual memory when reading integer from string\n");
		exit(EXIT_FAILURE);
	}
	while(line[*start] == ' ')
		++(*start);
	int length = 1;
	while(line[*start] != ' ' && line[*start] != '\0'){
		if(!isdigit(line[*start])){
			fprintf(stderr, "cannot read token as integer\n");
			fprintf(stderr, "unexpected character: %c\n", line[*start]);
			exit(EXIT_FAILURE);
		}
		if(length == size){
			size *= 2;
			if(!(rv_str = realloc(rv_str, size*sizeof(char)))){
				fprintf(stderr, "Ran out of virtual memory when reading integer from string\n");
				exit(EXIT_FAILURE);
			}
		}
		rv_str[length - 1] = line[*start];
		length++;
		(*start)++;
	}
	rv_str[length - 1] = '\0';
	int rv = strtoimax(rv_str,NULL,10);
	if(rv == 0){
		if(errno == EINVAL){
			fprintf(stderr, "Conversion error %d occurred when converting integer token %s\n", errno, rv_str);
			exit(EXIT_FAILURE);
		}
		if(errno == ERANGE){
			fprintf(stderr, "Element %s outside of range of specification\n", rv_str);
			exit(EXIT_FAILURE);
		}
	}
	free(rv_str);
	return rv;
}

float read_float_token(char *line, int *start)
{
	int size = MALLOCINIT;
	char *rv_str;
	if(!(rv_str = malloc(size *sizeof(char)))){
		fprintf(stderr, "Ran out of virtual memory when reading integer from string\n");
		exit(EXIT_FAILURE);
	}
	while(line[*start] == ' ')
		++(*start);
	int length = 1;
	bool found_decimal = false;
	while(line[*start] != ' ' && line[*start] != '\0'){
		if(!isdigit(line[*start])){
			if(line[*start] != '.' || found_decimal){
				fprintf(stderr, "cannot read token as floating point\n");
				fprintf(stderr, "unexpected character: %c\n", line[*start]);
				exit(EXIT_FAILURE);
			}
			found_decimal = true;
		}
		if(length == size){
			size *= 2;
			if(!(rv_str = realloc(rv_str, size*sizeof(char)))){
				fprintf(stderr, "Ran out of virtual memory when reading integer from string\n");
				exit(EXIT_FAILURE);
			}
		}
		rv_str[length - 1] = line[*start];
		length++;
		(*start)++;
	}
	rv_str[length - 1] = '\0';
	float rv = strtof(rv_str,NULL);
	if(rv == 0.0){
		if(errno == ERANGE){
			fprintf(stderr, "Element %s outside of range of specification\n", rv_str);
			exit(EXIT_FAILURE);
		}
	}
	else if (rv == HUGE_VALF || rv == (-1)*HUGE_VALF){
		if(errno == ERANGE){
			fprintf(stderr, "Element %s outside of range of specification\n", rv_str);
			exit(EXIT_FAILURE);
		}
	}
	free(rv_str);
	return rv;
}

int read_dim(FILE *file)
{
	char *line = readline(file);
	int start = 0;
	int rv = read_int_token(line, &start);
	free(line);
	return rv;
}
//made the decision to not write a function to convert to a 2d representation of the values
//as this way is more efficient. For some structures this couldn't be avoided
coo read_coo(FILE* file)
{
	coo rv;
	rv.type = get_type(file);
	rv.rows = read_dim(file);
	rv.cols = read_dim(file);
	int size = MALLOCINIT;
	if(!(rv.elems = malloc(size*sizeof(coo_elem)))){
		fprintf(stderr, "Ran out of virtual memory when allocating coo struct\n");
		exit(EXIT_FAILURE);
	}
	rv.length = 0;
	char *line = readline(file);
	int start = 0;
	for(int i = 0; i < rv.rows; ++i){
		for(int j = 0; j < rv.cols; ++j){
			union{
				int i;
				float f;
			} val;
			if(rv.type == MAT_INT){
				val.i = read_int_token(line, &start);
				if(val.i == 0)
					continue;
			}
			else{
				val.f = read_float_token(line, &start);
				if(val.f == 0.0)
					continue;
			}
			if(rv.length == size){
				size *= 2;
				if(!(rv.elems = realloc(rv.elems, size * sizeof(coo_elem)))){
					fprintf(stderr, "Ran out of virtual memory when allocating coo struct\n");
					exit(EXIT_FAILURE);
				}
			}
			rv.elems[rv.length].type = rv.type;
			rv.elems[rv.length].i = i;
			rv.elems[rv.length].j = j;
			if(rv.type == MAT_FLOAT)
				rv.elems[rv.length].val.f = val.f;
			else
				rv.elems[rv.length].val.i = val.i;
			rv.length++;
		}
	}
	free(line);
	return rv;
}
