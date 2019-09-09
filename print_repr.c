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
	printf("rows: %d\n", matrix.rows);
	printf("cols: %d\n", matrix.cols);
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
