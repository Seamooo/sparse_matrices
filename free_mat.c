#include "main.h"

void free_coo(coo matrix)
{
	free(matrix.elems);
}

void free_csr(csr matrix)
{
	if(matrix.type == MAT_INT)
		free(matrix.nnz.i);
	else
		free(matrix.nnz.f);
	free(matrix.ja);
	free(matrix.ia);
}

void free_csc(csc matrix)
{
	free_csr(matrix);
}
