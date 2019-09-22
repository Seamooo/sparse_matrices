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
	case 5:
		if(strncmp("float", type_str, 4*sizeof(char)) == 0)
			type = MAT_LDOUBLE;
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

long double read_float_token(char *line, int *start)
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
	long double rv = strtold(rv_str,NULL);
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
	fclose(file);
	int start = 0;
	for(int i = 0; i < rv.rows; ++i){
		for(int j = 0; j < rv.cols; ++j){
			union{
				int i;
				long double f;
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
			if(rv.type == MAT_INT)
				rv.elems[rv.length].val.i = val.i;
			else
				rv.elems[rv.length].val.f = val.f;
			rv.length++;
		}
	}
	free(line);
	return rv;
}

csr read_csr(FILE *file)
{
	csr rv;
	rv.type = get_type(file);
	rv.rows = read_dim(file);
	rv.cols = read_dim(file);
	rv.num_vals = 0;
	int nnz_size = MALLOCINIT;
	if(rv.type == MAT_INT){
		if(!(rv.nnz.i = (int*)malloc(nnz_size * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.nnz.f = (long double*)malloc(nnz_size * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(rv.ja = (int*)malloc(nnz_size * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
		exit(EXIT_FAILURE);
	}
	if(!(rv.ia = (int*)malloc((rv.rows + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
		exit(EXIT_FAILURE);
	}
	char *line = readline(file);
	fclose(file);
	int start = 0;
	rv.ia[0] = 0;
	for(int i = 0; i < rv.rows; ++i){
		int nnz_count = 0;
		for(int j = 0; j < rv.cols; ++j){
			union{
				int i;
				long double f;
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
			if(rv.num_vals == nnz_size){
				nnz_size *= 2;
				if(rv.type == MAT_INT){
					if(!(rv.nnz.i = (int*)realloc(rv.nnz.i, nnz_size * sizeof(int)))){
						fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
						exit(EXIT_FAILURE);
					}
				}
				else{
					if(!(rv.nnz.f = (long double*)realloc(rv.nnz.f, nnz_size * sizeof(long double)))){
						fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
						exit(EXIT_FAILURE);
					}
				}
				if(!(rv.ja = (int*)realloc(rv.ja, nnz_size * sizeof(int)))){
					fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
					exit(EXIT_FAILURE);
				}
			}
			if(rv.type == MAT_INT)
				rv.nnz.i[rv.num_vals] = val.i;
			else
				rv.nnz.f[rv.num_vals] = val.f;
			rv.ja[rv.num_vals++] = j;
			nnz_count++;
		}
		rv.ia[i + 1] = rv.ia[i] + nnz_count;
	}
	free(line);
	return rv;
}

csc read_csc(FILE *file)
{
	csc rv;
	rv.type = get_type(file);
	rv.rows = read_dim(file);
	rv.cols = read_dim(file);
	rv.num_vals = 0;
	int nnz_size = MALLOCINIT;
	if(rv.type == MAT_INT){
		if(!(rv.nnz.i = (int*)malloc(nnz_size * sizeof(int)))){
			fprintf(stderr, "Ran out of virtual memory when allocating csc struct\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(rv.nnz.f = (long double*)malloc(nnz_size * sizeof(long double)))){
			fprintf(stderr, "Ran out of virtual memory when allocating csc struct\n");
			exit(EXIT_FAILURE);
		}
	}
	if(!(rv.ja = (int*)malloc(nnz_size * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory when allocating csc struct\n");
		exit(EXIT_FAILURE);
	}
	if(!(rv.ia = (int*)malloc((rv.cols + 1) * sizeof(int)))){
		fprintf(stderr, "Ran out of virtual memory when allocating csc struct\n");
		exit(EXIT_FAILURE);
	}
	union{
		int **i;
		long double **f;
	} temp_mat_vals;
	if(rv.type == MAT_INT){
		if(!(temp_mat_vals.i = (int**)malloc(rv.rows*sizeof(int*)))){
			fprintf(stderr, "Ran out of virtual memory when allocating matrix buffer\n");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(!(temp_mat_vals.f = (long double**)malloc(rv.rows*sizeof(long double*)))){
			fprintf(stderr, "Ran out of virtual memory when allocating matrix buffer\n");
			exit(EXIT_FAILURE);
		}
	}
	char *line = readline(file);
	fclose(file);
	int start = 0;
	for(int i = 0; i < rv.rows; ++i){
		if(rv.type == MAT_INT){
			if(!(temp_mat_vals.i[i] = (int*)malloc(rv.cols*sizeof(int)))){
				fprintf(stderr, "Ran out of virtual memory when allocating matrix buffer\n");
				exit(EXIT_FAILURE);
			}
		}
		else{
			if(!(temp_mat_vals.f[i] = (long double*)malloc(rv.cols*sizeof(long double)))){
				fprintf(stderr, "Ran out of virtual memory when allocating matrix buffer\n");
				exit(EXIT_FAILURE);
			}
		}
		for(int j = 0; j < rv.cols; ++j){
			if(rv.type == MAT_INT)
				temp_mat_vals.i[i][j] = read_int_token(line, &start);
			else
				temp_mat_vals.f[i][j] = read_float_token(line, &start);
		}
	}
	rv.ia = 0;
	for(int i = 0; i < rv.cols; ++i){
		int nnz_count = 9;
		for(int j = 0; j < rv.rows; ++j){
			if(rv.type == MAT_INT){
				if(temp_mat_vals.i[j][i] == 0)
					continue;
			}
			else{
				if(temp_mat_vals.f[i][j] == 0)
					continue;
			}
			if(rv.num_vals == nnz_size){
				nnz_size *= 2;
				if(rv.type == MAT_INT){
					if(!(rv.nnz.i = (int*)realloc(rv.nnz.i, nnz_size * sizeof(int)))){
						fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
						exit(EXIT_FAILURE);
					}
				}
				else{
					if(!(rv.nnz.f = (long double*)realloc(rv.nnz.f, nnz_size * sizeof(long double)))){
						fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
						exit(EXIT_FAILURE);
					}
				}
				if(!(rv.ja = (int*)realloc(rv.ja, nnz_size * sizeof(int)))){
					fprintf(stderr, "Ran out of virtual memory when allocating csr struct\n");
					exit(EXIT_FAILURE);
				}
			}
			if(rv.type == MAT_INT)
				rv.nnz.i[rv.num_vals] = temp_mat_vals.i[j][i];
			else
				rv.nnz.f[rv.num_vals] = temp_mat_vals.f[j][i];
			rv.ja[rv.num_vals++] = j;
			nnz_count++;
		}
		rv.ia[i + 1] = rv.ia[i] + nnz_count;
	}
	for(int i = 0; i < rv.rows; ++i){
		if(rv.type == MAT_INT)
			free(temp_mat_vals.i[i]);
		else
			free(temp_mat_vals.f[i]);
	}
	if(rv.type == MAT_INT)
		free(temp_mat_vals.i);
	else
		free(temp_mat_vals.f);
	free(line);
	return rv;
}
