#include "main.h"

void free_coo(coo matrix){
	if(matrix.length > 0){
		free(matrix.elems);
	}
}
