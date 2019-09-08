#include "main.h"

//TODO:
//bug with strtoimax where extremely large values return -1
//add naive flag
//add silence output flag
//create threaded functions

//notes:
//not freeing memory before exitting as OS should release allocated memory on exit
//	for all OSes that are supported

int main(int argc, char *argv[])
{
	struct timespec call_time;
	get_utc_time(&call_time);
	float scalar;
	int numfiles = 0;
	OPERATION operation = NO_OPERATION;
	FORMAT format = FORM_DEFAULT;
	int num_threads = 2;
	bool logging = false;
	char *filename1 = NULL;
	char *filename2 = NULL;
	//parse command line args
	//not using getopt for compatibility
	if(argc < 2){
		fprintf(stderr,"nothing to do\n");
		exit(EXIT_SUCCESS);
	}
	int i = 1	;
	while(i < argc){
		if(argv[i][0] != '-'){
			fprintf(stderr,"expected option, instead recieved: %s\n",argv[i]);
			exit(EXIT_FAILURE);
		}
		switch(strlen(argv[i])){
		case 2:
			if(strncmp("-f",argv[i], 2*sizeof(char)) == 0){
				++i;
				while(i < argc){
					if(argv[i][0] == '-')
						break;
					if(numfiles == 0){
						filename1 = argv[i];
						numfiles++;
					}
					else if(numfiles == 1){
						filename2 = argv[i];
						numfiles++;
					}
					else{
						fprintf(stderr, "unexpected number of matrix files\n");
						exit(EXIT_FAILURE);
					}
					++i;
				}
				continue;
			}
			else if(strncmp("-t",argv[i], 2*sizeof(char)) == 0){
				++i;
				bool invalarg = false;
				if(i == argc)
					invalarg = true;
				else if(argv[i][0] == '-')
					invalarg = true;
				if(invalarg){
					fprintf(stderr,"-t missing argument\n");
					exit(EXIT_FAILURE);
				}
				num_threads = strtoimax(argv[i],NULL,10);
				if(num_threads == 0){
					if(errno == EINVAL){
						fprintf(stderr,"Conversion error %d occurred after -t option",errno);
						exit(EXIT_FAILURE);
					}
					if(errno == ERANGE){
						fprintf(stderr, "requested greater than max threads... defaulting to %d threads\n", omp_get_max_threads());
						num_threads = omp_get_max_threads();
					}
				}
				if(num_threads > omp_get_max_threads()){
					fprintf(stderr, "requested greater than max threads... defaulting to %d threads\n", omp_get_max_threads());
					num_threads = omp_get_max_threads();
				}
			}
			else if(strncmp("-l",argv[i], 2*sizeof(char)) == 0)
				logging = true;
			else{
				fprintf(stderr, "unkown option: %s\n", argv[i]);
			}
			break;
		case 4:{
			bool isset = (operation != NO_OPERATION);
			if(strncmp("--sc",argv[i], 4 * sizeof(char)) == 0){
				operation = SCAL_MUL;
				++i;
				scalar = strtof(argv[i],NULL);
				if(scalar == 0.0){
					if(errno == ERANGE){
						fprintf(stderr, "Element %s outside of range of specification\n", argv[i]);
						exit(EXIT_FAILURE);
					}
				}
				else if (scalar == HUGE_VALF || scalar == (-1)*HUGE_VALF){
					if(errno == ERANGE){
						fprintf(stderr, "Element %s outside of range of specification\n", argv[i]);
						exit(EXIT_FAILURE);
					}
				}
			}
			else if(strncmp("--tr",argv[i], 4 * sizeof(char)) == 0)
				operation = TRACE;
			else if(strncmp("--ad",argv[i], 4 * sizeof(char)) == 0)
				operation = ADD;
			else if(strncmp("--ts",argv[i], 4 * sizeof(char)) == 0)
				operation = TRANSPOSE;
			else if(strncmp("--mm",argv[i], 4 * sizeof(char)) == 0)
				operation = MAT_MUL;
			else{
				fprintf(stderr, "unrecognised operation: %s\n",argv[i]);
				exit(EXIT_FAILURE);
			}
			if(isset){
				fprintf(stderr, "only one operation may be specified\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
		case 8:
			if(strncmp("--format",argv[i], 8 * sizeof(char)) == 0){
				++i;
				bool invalarg = false;
				if(i == argc)
					invalarg = true;
				else if(argv[i][0] == '-')
					invalarg = true;
				if(invalarg){
					fprintf(stderr,"--format missing argument\n");
					exit(EXIT_FAILURE);
				}
				switch(strlen(argv[i])){
				case 3:
					if(strncmp("COO",argv[i], 3 * sizeof(char)) == 0){
						format = COO;
					}
					else if(strncmp("CSR",argv[i],3 * sizeof(char)) == 0)
						format = CSR;
					else if(strncmp("CSC",argv[i],3 * sizeof(char)) == 0)
						format = CSR;
					else if(strncmp("CSR",argv[i],3 * sizeof(char)) == 0)
						format = CSR;
					else if(strncmp("CDS",argv[i],3 * sizeof(char)) == 0)
						format = CDS;
					else if(strncmp("JDS",argv[i],3 * sizeof(char)) == 0)
						format = JDS;
					else if(strncmp("SKS",argv[i],3 * sizeof(char)) == 0)
						format = SKS;
					else{
						fprintf(stderr, "unrecognised format: %s\n",argv[i]);
						exit(EXIT_FAILURE);
					}
					break;
				case 4:
					if(strncmp("BCSR",argv[i],4 * sizeof(char)) == 0)
						format = BCSR;
					else{
						fprintf(stderr, "unrecognised format: %s\n",argv[i]);
						exit(EXIT_FAILURE);
					}
					break;
				default:
					fprintf(stderr, "unrecognised format: %s\n",argv[i]);
					exit(EXIT_FAILURE);
				}
			}
			break;
		default:
			fprintf(stderr,"unknown option: %s\n", argv[i]);
			exit(EXIT_FAILURE);
		}
		++i;
	}
	omp_set_num_threads(num_threads);
	//check that the right number of files were provided
	switch(operation){
	case SCAL_MUL:
		if(numfiles != 1){
			fprintf(stderr, "Scalar multiplication requires exactly 1 input matrix\n");
			exit(EXIT_FAILURE);
		}
		break;
	case TRACE:
		if(numfiles != 1){
			fprintf(stderr, "Trace requires exactly 1 input matrix\n");
			exit(EXIT_FAILURE);
		}
		break;
	case ADD:
		if(numfiles != 2){
			fprintf(stderr, "Matrix addition requires exactly 2 input matrices\n");
			exit(EXIT_FAILURE);
		}
		break;
	case TRANSPOSE:
		if(numfiles != 1){
			fprintf(stderr, "Transpose requires exactly 1 input matrix\n");
			exit(EXIT_FAILURE);
		}
		break;
	case MAT_MUL:
		if(numfiles != 2){
			fprintf(stderr, "Matrix multiplication requires exactly 2 input matrices\n");
			exit(EXIT_FAILURE);
		}
		break;
	default:
		break;
	}
	FILE *file1 = NULL;
	FILE *file2 = NULL;
	if(numfiles > 0){
		if(!(file1 = fopen(filename1,"rb"))){
			fprintf(stderr, "File %s couldn't be opened\n", filename1);
			exit(EXIT_FAILURE);
		}
	}
	if(numfiles > 1){
		if(!(file2 = fopen(filename2,"rb"))){
			fprintf(stderr, "File %s couldn't be opened\n", filename2);
			exit(EXIT_FAILURE);
		}
	}
	mat_rv result = execute_operation(file1, file2, operation, format, scalar);
	switch(result.error){
	case ERR_CONSTRUCTION:
		fprintf(stderr, "Internal Error constructing result matrix from sparse matrix\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_WRONG_DIM:
		fprintf(stderr, "Matrices given contained incompatible dimensions for given operation\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_NOT_SET:
		fprintf(stderr, "Internal Error resulted in matrix not set\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_TYPE_MISSMATCH:
		fprintf(stderr, "Type missmatch while processing matrix(es)\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_DUPLICATE:
		fprintf(stderr, "Internal Error resulted in duplicate entries in matrix\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_DIM_MISSMATCH:
		fprintf(stderr, "Internal Error resulted in dimension missmatch in matrix\n");
		exit(EXIT_FAILURE);
		break;
	case ERR_NONE:
		break;
	default:
		fprintf(stderr, "matrix returned with unimplemented error value\n");
		exit(EXIT_FAILURE);
		break;
	}
	if(numfiles > 0)
		fclose(file1);
	if(numfiles > 1)
		fclose(file2);
	if(logging){
		create_log_file(call_time, operation, numfiles, filename1, filename2, num_threads, result);
	}
	print_mat_rv(result);
}
