#include "main.h"

//debug representations
void print_mat_rv(mat_rv matrix)
{
	printf("[");
	for(int i = 0; i < matrix.rows; ++i){
		printf("[");
		for(int j = 0; j < matrix.cols; ++j){
			if(matrix.type == MAT_INT)
				printf("%d",matrix.vals.i[i*matrix.cols+j]);
			else
				printf("%f",matrix.vals.f[i*matrix.cols+j]);
			if(j < matrix.cols - 1){
				printf(", ");
			}
		}
		printf("]");
		if(i < matrix.rows - 1)
			printf(",\n");
	}
	printf("]\n");
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
			printf("[i:%d,j:%d,val:%d]",matrix.elems[i].i, matrix.elems[i].j, matrix.elems[i].val.i);
		else
			printf("[i:%d,j:%d,val:%f]",matrix.elems[i].i, matrix.elems[i].j, matrix.elems[i].val.f);
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
			printf("%f",matrix.nnz.f[i]);
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
			printf("%f",matrix.nnz.f[i]);
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
