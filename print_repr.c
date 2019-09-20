#include "main.h"

//debug representations
void print_int_arr(int *arr, int length)
{
	printf("[");
	for(int i = 0; i < length; ++i){
		printf("%d", arr[i]);
		if(i < length - 1){
			printf(", ");
		}
	}
	printf("]\n");
}

void print_mat_rv(mat_rv matrix)
{
	if(matrix.error != ERR_NONE){
		printf("error = ");
		switch(matrix.error){
		case ERR_CONSTRUCTION:
			printf("ERROR_CONSTRUCTION\n");
			return;
			break;
		case ERR_WRONG_DIM:
			printf("ERROR_WRONG_DIM\n");
			return;
			break;
		case ERR_NOT_SET:
			printf("ERROR_NOT_SET\n");
			return;
			break;
		case ERR_TYPE_MISSMATCH:
			printf("ERROR_TYPE_MISSMATCH\n");
			return;
			break;
		case ERR_DUPLICATE:
			printf("ERROR_DUPLICATE\n");
			return;
			break;
		case ERR_DIM_MISSMATCH:
			printf("ERROR_DIM_MISSMATCH");
			return;
			break;
		case ERR_NONE:
			break;
		default:
			fprintf(stderr, "UNKNOWN\n");
			return;
		}
	}
	if(matrix.isval){
		if(matrix.type == MAT_INT)
			printf("%d\n", matrix.vals.i[0]);
		else
			printf("%Lf\n", matrix.vals.f[0]);
	}
	else{
		printf("[");
		for(int i = 0; i < matrix.rows; ++i){
			printf("[");
			for(int j = 0; j < matrix.cols; ++j){
				if(matrix.type == MAT_INT)
					printf("%d",matrix.vals.i[i*matrix.cols+j]);
				else
					printf("%Lf",matrix.vals.f[i*matrix.cols+j]);
				if(j < matrix.cols - 1){
					printf(", ");
				}
			}
			printf("]");
			if(i < matrix.rows - 1)
				printf(",\n");
		}
		printf("]\n");
	}
	printf("Time to construct to format & convert from format: %Lfs\n", (long double)matrix.t_construct.tv_sec + (long double)matrix.t_construct.tv_nsec / 1E9);
	printf("Time to process: %Lfs\n", (long double)matrix.t_process.tv_sec + (long double)matrix.t_process.tv_nsec / 1E9);
}

void print_coo(coo matrix)
{
	printf("type: %s\n", (matrix.type == MAT_INT) ? "int" : "float");
	printf("rows: %d\n", matrix.rows);
	printf("cols: %d\n", matrix.cols);
	printf("length: %d\n", matrix.length);
	printf("[");
	for(int i = 0; i < matrix.length; ++i){
		if(matrix.elems[i].type == MAT_INT)
			printf("<i:%d, j:%d, val:%d>",matrix.elems[i].i, matrix.elems[i].j, matrix.elems[i].val.i);
		else
			printf("<i:%d, j:%d, val:%Lf>",matrix.elems[i].i, matrix.elems[i].j, matrix.elems[i].val.f);
		if(i < matrix.length - 1)
			printf(", ");
	}
	printf("]\n");
}

void print_csr(csr matrix)
{
	printf("type: %s\n", (matrix.type == MAT_INT) ? "int" : "float");
	printf("rows: %d\n", matrix.rows);
	printf("cols: %d\n", matrix.cols);
	printf("num_vals: %d\n", matrix.num_vals);
	printf("nnz: [");
	for(int i = 0; i < matrix.num_vals; ++i){
		if(matrix.type == MAT_INT)
			printf("%d",matrix.nnz.i[i]);
		else
			printf("%Lf",matrix.nnz.f[i]);
		if(i < matrix.num_vals - 1)
			printf(", ");
	}
	printf("]\n");
	printf("ia: [");
	for(int i = 0; i < matrix.rows + 1; ++i){
		printf("%d", matrix.ia[i]);
		if(i < matrix.rows)
			printf(", ");
	}
	printf("]\n");
	printf("ja: [");
	for(int i = 0; i < matrix.num_vals; ++i){
		printf("%d", matrix.ja[i]);
		if(i < matrix.num_vals - 1)
			printf(", ");
	}
	printf("]\n");
}

void print_csc(csc matrix)
{
	printf("type: %s\n", (matrix.type == MAT_INT) ? "int" : "float");
	printf("rows: %d\n", matrix.rows);
	printf("cols: %d\n", matrix.cols);
	printf("num_vals: %d\n", matrix.num_vals);
	printf("nnz: [");
	for(int i = 0; i < matrix.num_vals; ++i){
		if(matrix.type == MAT_INT)
			printf("%d",matrix.nnz.i[i]);
		else
			printf("%Lf",matrix.nnz.f[i]);
		if(i < matrix.num_vals - 1)
			printf(", ");
	}
	printf("]\n");
	printf("ia: [");
	for(int i = 0; i < matrix.rows + 1; ++i){
		printf("%d", matrix.ia[i]);
		if(i < matrix.rows)
			printf(", ");
	}
	printf("]\n");
	printf("ja: [");
	for(int i = 0; i < matrix.num_vals; ++i){
		printf("%d", matrix.ja[i]);
		if(i < matrix.num_vals - 1)
			printf(", ");
	}
	printf("]\n");
}

void print_bcsr(bcsr matrix)
{

}
