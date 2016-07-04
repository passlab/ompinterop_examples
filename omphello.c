#include <stdio.h>
#include <unistd.h>
#include <omp.h>
int main(int argc, char * argv[]) {
	int nthreads = sysconf( _SC_NPROCESSORS_ONLN );
    	if (argc >= 2) nthreads = (atoi(argv[1]));
	#pragma omp parallel num_threads(nthreads)
    	{
		printf("Hello from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
	}

        kmp_set_blocktime(100);

	while(1);
	return 0;
}
