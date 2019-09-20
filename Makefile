GCCPARAMS = -Wall -Werror -pedantic -std=c99 -fopenmp
CFILES = free_mat.c execute_operation.c readline.c read_mat.c scalar_multiply.c trace.c add.c transpose.c mat_mul.c sparse_to_mat.c print_repr.c sort_mat.c timer.c logging.c main.c
HFILES = main.h
GCC_CMD = gcc-7 #change this to command supporting openmp
ERROR_HANDLER = \
	case "$$?" in \
		1) \
			echo $(GCC_CMD) does not support openmp, please install a version of gcc or clang that contains fopenmp and change GCC_CMD in Makefile; \
	esac;

sparse_matrix.bin:$(CFILES) $(HFILES)
	@$(GCC_CMD) $(GCCPARAMS) -o $@ $(CFILES); $(ERROR_HANDLER)

debug.bin:$(CFILES) $(HFILES)
	@$(GCC_CMD) $(GCCPARAMS) -g -o $@ $(CFILES); $(ERROR_HANDLER)

#remember to add actions for creating unittests once finalised

clean:
	rm out.bin
	rm debug.bin
	rm unittests.sh

out.bin:
	gcc-7 -o out.bin
