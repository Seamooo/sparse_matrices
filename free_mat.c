#include "main.h"

void free_coo(coo matrix){
	free(matrix.elems);
}
