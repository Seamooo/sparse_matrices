#include "main.h"

//TODO:
//bug/misunderstood implementation with strtoimax where extremely large values return -1
//create threaded functions
//add help flag and ability to display help page when no args given
//add ability to speed up with the stack rather than using the heap with lower amounts of elements
//need to be rigorous to check for memory leeks (check every size)
//add support for integer scalar multiplication

//notes:
//not freeing memory before exitting as OS should release allocated memory on exit
//	for all OSes that are supported

//main function handles argument parsing and control flow with regards to some flags

int main(int argc, char *argv[])
{
	struct timespec call_time;
	get_utc_time(&call_time);
	int numfiles = 0;
	OPERATIONARGS operation_args;
	operation_args.operation = NO_OPERATION;
	operation_args.format = FORM_DEFAULT;
	operation_args.nothreading = false;
	operation_args.block_size = 0;
	operation_args.num_threads = -1;
	bool logging = false;
	bool silence = false;
	char *filename1 = NULL;
	char *filename2 = NULL;
	//parse command line args
	//not using getopt for compatibility
	if(argc < 2){
		fprintf(stderr,"nothing to do\n");
		exit(EXIT_FAILURE);
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
				operation_args.num_threads = strtoimax(argv[i],NULL,10);
				if(operation_args.num_threads == 0){
					if(errno == EINVAL){
						fprintf(stderr,"Conversion error %d occurred after -t option\n",errno);
						exit(EXIT_FAILURE);
					}
					if(errno == ERANGE){
						fprintf(stderr, "Number of threads specified out of range\n");
						exit(EXIT_FAILURE);
					}
				}
			}
			else if(strncmp("-l",argv[i], 2*sizeof(char)) == 0)
				logging = true;
			else if(strncmp("-s",argv[i], 2*sizeof(char)) == 0)
				silence = true;
			else{
				fprintf(stderr, "unkown option: %s\n", argv[i]);
				exit(EXIT_FAILURE);
			}
			break;
		case 4:{
			bool isset = (operation_args.operation != NO_OPERATION);
			if(strncmp("--sm",argv[i], 4 * sizeof(char)) == 0){
				operation_args.operation = SCAL_MUL;
				++i;
				operation_args.scalar = strtold(argv[i],NULL);
				if(operation_args.scalar == 0.0){
					if(errno == ERANGE){
						fprintf(stderr, "Element %s outside of range of specification\n", argv[i]);
						exit(EXIT_FAILURE);
					}
				}
				else if (operation_args.scalar == HUGE_VALF || operation_args.scalar == (-1)*HUGE_VALF){
					if(errno == ERANGE){
						fprintf(stderr, "Element %s outside of range of specification\n", argv[i]);
						exit(EXIT_FAILURE);
					}
				}
			}
			else if(strncmp("--tr",argv[i], 4 * sizeof(char)) == 0)
				operation_args.operation = TRACE;
			else if(strncmp("--ad",argv[i], 4 * sizeof(char)) == 0)
				operation_args.operation = ADD;
			else if(strncmp("--ts",argv[i], 4 * sizeof(char)) == 0)
				operation_args.operation = TRANSPOSE;
			else if(strncmp("--mm",argv[i], 4 * sizeof(char)) == 0)
				operation_args.operation = MAT_MUL;
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
						operation_args.format = COO;
					}
					else if(strncmp("CSR",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = CSR;
					else if(strncmp("CSC",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = CSR;
					else if(strncmp("CSR",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = CSR;
					else if(strncmp("CDS",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = CDS;
					else if(strncmp("JDS",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = JDS;
					else if(strncmp("SKS",argv[i],3 * sizeof(char)) == 0)
						operation_args.format = SKS;
					else{
						fprintf(stderr, "unrecognised format: %s\n",argv[i]);
						exit(EXIT_FAILURE);
					}
					break;
				case 4:
					if(strncmp("BCSR",argv[i],4 * sizeof(char)) == 0){
						operation_args.format = BCSR;
						++i;
						bool isnum = true;
						for(int j = 0; argv[i][j] != '\0'; ++j){
							if(!(argv[i][j] >= '0' && argv[i][j] <= '9')){
								isnum = false;
								break;
							}
						}
						if(!isnum){
							fprintf(stderr, "No block size specified. Using default: 2\n");
							continue;
						}
						operation_args.block_size = strtoimax(argv[i],NULL,10);
						if(operation_args.block_size == 0){
							if(errno == EINVAL){
								fprintf(stderr,"Conversion error %d when specifying block size\n",errno);
								exit(EXIT_FAILURE);
							}
							if(errno == ERANGE){
								fprintf(stderr, "Block size out of integer range\n");
								exit(EXIT_FAILURE);
							}
						}
					}
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
			else{
				fprintf(stderr, "unknown option %s\n", argv[i]);
				exit(EXIT_FAILURE);
			}
			break;
		case 13:
			if(strncmp("--nothreading",argv[i],13 * sizeof(char)) == 0)
				operation_args.nothreading = true;
			else{
				fprintf(stderr, "unknown option %s\n", argv[i]);
				exit(EXIT_FAILURE);
			}
			break;
		default:
			fprintf(stderr,"unknown option: %s\n", argv[i]);
			exit(EXIT_FAILURE);
		}
		++i;
	}
	if(operation_args.num_threads == -1 && !operation_args.nothreading){
		fprintf(stderr,"No number of threads provided\nUsing default num_threads: 2\n");
		operation_args.num_threads = 2;
	}
	if(operation_args.num_threads != -1 && operation_args.nothreading)
		fprintf(stderr, "Warning: number of threads specified while using --nothreading flag\n");
	if(operation_args.nothreading)
		operation_args.num_threads = 1;
	else
		omp_set_num_threads(operation_args.num_threads);
	//check that the right number of files were provided
	switch(operation_args.operation){
	case SCAL_MUL:
		if(numfiles != 1){
			fprintf(stderr, "scalar multiplication requires exactly 1 input matrix\n");
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
		fprintf(stderr, "No operation specified\n");
		exit(EXIT_FAILURE);
	}
	operation_args.file1 = NULL;
	operation_args.file2 = NULL;
	if(numfiles > 0){
		if(!(operation_args.file1 = fopen(filename1,"rb"))){
			fprintf(stderr, "File %s couldn't be opened\n", filename1);
			exit(EXIT_FAILURE);
		}
	}
	if(numfiles > 1){
		if(!(operation_args.file2 = fopen(filename2,"rb"))){
			fprintf(stderr, "File %s couldn't be opened\n", filename2);
			exit(EXIT_FAILURE);
		}
	}
	mat_rv result = execute_operation(operation_args);
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
		fclose(operation_args.file1);
	if(numfiles > 1)
		fclose(operation_args.file2);
	if(logging){
		create_log_file(
			call_time,
			operation_args.operation,
			numfiles,
			filename1,
			filename2,
			operation_args.num_threads,
			result);
	}
	if(!silence)
		print_mat_rv(result);
	exit(EXIT_SUCCESS);
}
