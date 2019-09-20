#ifndef __MAIN_H
#define __MAIN_H

#ifdef _WIN32
#define OS "WINDOWS"

#elif defined __LINUX__
#define OS "LINUX"
#ifndef __unix__
#define __unix__
#endif

#elif defined __APPLE__ && __MACH__
#define OS "MAC"
#ifndef __unix__
#define __unix__
#endif

#else
#error Operating System not recognised
#endif

#ifdef __unix__
#include <sys/time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define MALLOCINIT 16

//debug macros:
#define p() printf("got here\n"); fflush(stdout);
#define pstr(x) printf("%s = %s\n",#x,x); fflush(stdout);
#define pint(x) printf("%s = %d\n",#x,x); fflush(stdout);
#define pld(x) printf("%s = %Lf\n",#x,x); fflush(stdout);
#define phex(x) for(int i = 0; x[i] != '\0'; ++i){printf("%02x ", x[i]);} printf("\n"); fflush(stdout);

typedef enum {
	NO_OPERATION,
	SCAL_MUL,
	TRACE,
	ADD,
	TRANSPOSE,
	MAT_MUL
} OPERATION;
typedef enum{
	FORM_DEFAULT,
	COO,
	CSR,
	CSC,
	BCSR,
	CDS,
	JDS,
	SKS
} FORMAT;
typedef enum {
	MAT_NONE,
	MAT_INT,
	MAT_LDOUBLE
} MAT_TYPE;
typedef enum{
	ERR_NONE,
	ERR_CONSTRUCTION,
	ERR_WRONG_DIM,
	ERR_TYPE_MISSMATCH,
	ERR_DUPLICATE,
	ERR_DIM_MISSMATCH,
	ERR_NOT_SET
} RV_ERROR;
typedef enum{
	ORDER_COL,
	ORDER_ROW
} ORDER;

//sparse matrice structs
typedef struct {
	MAT_TYPE type;
	int i;
	int j;
	union{
		long double f;
		int i;
	} val;
} coo_elem;
typedef struct{
	//store rows and cols to reconstruct matrix more easily
	int rows;
	int cols;
	//store type to handle 0 elem case
	MAT_TYPE type;
	int length;
	coo_elem *elems;
} coo;
typedef struct {
	MAT_TYPE type;
	//store rows and cols to reconstruct matrix more easily
	int rows;
	int cols;
	int num_vals;
	union{
		long double *f;
		int *i;
	} nnz;
	int *ja;
	//not storing ialength as ia_len == rows + 1
	int *ia;
} csr;
//ia_len == cols + 1 for below
typedef csr csc;
typedef struct {
	MAT_TYPE type;
	int index;
	int i;
	int j;
	union{
		long double f;
		int i;
	} val;
} bcsr_nnzb;
typedef struct {
	//store rows and columbns for easier reconstruction
	int rows;
	int cols;
	//store type to handle 0 elem case
	MAT_TYPE type;
	int num_blocks;
	int block_size;
	bcsr_nnzb *blocks;
	int *entries;
	int cols_arr_len;
	int *cols_arr;
} bcsr;
typedef struct {
	MAT_TYPE type;
	union{
		long double **f;
		int **i;
	} vals;
	int n;
	int half_bw_hi;
	int half_bw_lo;
} cds;
typedef struct {
	MAT_TYPE type;
	union{
		long double **f;
		int **i;
	} vals;
	int **col_ind;
} jds;
//mat_rv struct will always be returned
typedef struct{
	MAT_TYPE type;
	RV_ERROR error;
	struct timespec t_construct;
	struct timespec t_process;
	bool isval;
	int rows;
	int cols;
	union{
		long double *f;
		int *i;
	} vals;
} mat_rv;

typedef struct{
	FILE *file1;
	FILE *file2;
	OPERATION operation;
	FORMAT format;
	long double scalar;
	bool nothreading;
	int block_size;
	int num_threads;
} OPERATIONARGS;
//handle sks later
//main function externs
extern char *readline(FILE *);
extern mat_rv execute_operation(OPERATIONARGS);
extern void create_log_file(struct timespec, OPERATION, int, char *, char *, int, mat_rv);

//operation functions
extern mat_rv scalar_multiply(OPERATIONARGS);
extern mat_rv trace(OPERATIONARGS);
extern mat_rv addition(OPERATIONARGS);
extern mat_rv transpose(OPERATIONARGS);
extern mat_rv matrix_multiply(OPERATIONARGS);

//free mat functions
extern void free_coo(coo);

//read mat functions
extern coo read_coo(FILE*);
extern csr read_csr(FILE*);
extern csc read_csc(FILE*);
extern bcsr read_bcsr(FILE*, int);

//conversion functions
extern mat_rv coo_to_mat(coo);

//sort functions
extern void sort_coo(coo, ORDER);

//timer functions
extern int get_timezone_offset();
extern void get_utc_time(struct timespec*);
extern struct timespec time_delta(struct timespec, struct timespec);
extern struct timespec time_sum(struct timespec, struct timespec);

//print functions for debug
extern void print_int_arr(int *, int);
extern void print_mat_rv(mat_rv);
extern void print_coo(coo);
extern void print_csr(csr);
extern void print_csc(csc);
extern void print_bcsr(bcsr);
#endif

