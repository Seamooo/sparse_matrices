GCCPARAMS = -Wall -Werror -pedantic -std=c99 -fopenmp
CFILES = free_mat.c execute_operation.c readline.c read_mat.c scalar_multiply.c trace.c add.c transpose.c mat_mul.c sparse_to_mat.c print_repr.c sort_mat.c timer.c logging.c main.c
HFILES = main.h
GCC_CMD = gcc-7

sparse_matrix.bin:$(CFILES) $(HFILES)
	$(GCC_CMD) $(GCCPARAMS) -o $@ $(CFILES)

debug.bin:$(CFILES) $(HFILES)
	$(GCC_CMD) $(GCCPARAMS) -g -o $@ $(CFILES)

#remember to add actions for creating unittests once finalised

clean:
	rm out.bin
	rm debug.bin
	rm unittests.sh
