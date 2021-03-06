LLVM_INSTALL=../llvm-install

LIBIOMP_PATH = ${LLVM_INSTALL}/lib
LIBIOMP_INCLUDE = ${LLVM_INSTALL}/lib/clang/3.9.0/include

EXE = pthread_create_test_1\
    omphello\
    omp_set_wait_policy_test_1\
    omp_set_wait_policy_test_2\
    omp_get_global__test_1\
    omp_quiesce_test_1\
    omp_set_wait_policy_quiesce_overhead\
    omp_set_wait_policy_pthread\

# Do the following export before run the program
# export LD_LIBRARY_PATH=${LIBIOMP_PATH}:$LD_LIBRARY_PATH

# For Clang/LLVM using interop implementation on Intel OpenMP runtime
CC = clang
CFLAGS = -g -fopenmp -O0 -I${LIBIOMP_INCLUDE} -L${LIBIOMP_PATH} -liomp5 -pthread

# For icc using interop implementation on Intel OpenMP runtime
# CC = icc
# CFLAGS = -fopenmp -I${LIBIOMP_INCLUDE} -L${LIBIOMP_PATH} -liomp5

# For GCC using interop implementation on GOMP runtime
# CC = gcc
# CFLAGS = -fopenmp ...
 
all: ${EXE}

.c:
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

clean:
	rm -rf ${EXE} a.out *.o
