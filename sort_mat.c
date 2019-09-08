#include "main.h"

int coo_row_cmp(const void *elem1, const void *elem2){
	coo_elem *elem1_struct = (coo_elem*) (elem1);
	coo_elem *elem2_struct = (coo_elem*) (elem2);
	if (elem1_struct->i == elem2_struct->i){
		if(elem1_struct->j == elem2_struct->j)
			return 0;
		else if(elem1_struct->j > elem2_struct->j)
			return 1;
		return -1;
	}
	else if(elem1_struct->i > elem2_struct->i)
		return 1;
	return -1;
}

int coo_col_cmp(const void *elem1, const void *elem2){
	coo_elem *elem1_struct = (coo_elem*) elem1;
	coo_elem *elem2_struct = (coo_elem*) elem2;
	if (elem1_struct->j == elem2_struct->j){
		if(elem1_struct->i == elem2_struct->i)
			return 0;
		else if(elem1_struct->i > elem2_struct->i)
			return 1;
		return -1;
	}
	else if(elem1_struct->j > elem2_struct->j)
		return 1;
	return -1;
}

void sort_coo(coo matrix, ORDER order){
	int (*cmp_func)(const void *,const void *);
	switch (order){
	case ORDER_COL:
		cmp_func = coo_col_cmp;
		break;
	case ORDER_ROW:
		cmp_func = coo_row_cmp;
		break;
	default:
		fprintf(stderr, "Unsupported sort orderring\n");
		exit(EXIT_FAILURE);
		break;
	}
	qsort(matrix.elems, matrix.length, sizeof(coo_elem), cmp_func);
}
