#include "main.h"

//returned char * must be freed after use
//function compatible with windows and unix binary ascii text files
char *readline(FILE *file)
{
	int size = MALLOCINIT;
	char *rv;
	if(!(rv = (char*)malloc(size*sizeof(char)))){
		fprintf(stderr,"ran out of virtual memory @readline_win\n");
		exit(EXIT_FAILURE);
	}
	char c;
	int length = 1;
	c = fgetc(file);
	while(c != EOF && c !='\n'){
		if(c == '\r'){
			c = fgetc(file);
			if(c == '\n')
				break;
			else{
				if (length == size){
					size *= 2;
					if(!(rv = (char*)realloc(rv, size*sizeof(char)))){
						fprintf(stderr,"ran out of virtual memory @readline_win\n");
						exit(EXIT_FAILURE);
					}
				}
				rv[length - 1] = '\r';
				length++;
			}
		}
		if (length == size){
			size *= 2;
			if(!(rv = (char*)realloc(rv, size*sizeof(char)))){
				fprintf(stderr,"ran out of virtual memory @readline_win\n");
				exit(EXIT_FAILURE);
			}
		}
		rv[length - 1] = c;
		length++;
		c = fgetc(file);
	}
	rv[length - 1] = '\0';
	return rv;
}
