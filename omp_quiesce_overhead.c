// input number of thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <omp.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "omp_interop.h"

/**Important: make sure you use num_threads clause in parallel direction and set it to the 
 * number of hardware cores, not the number of cores Linux gives or the default from OpenMP
 * 
 * cat /proc/cpuinfo and check the processor id, core id and CPU model number so you can look up fron internet
 * Lennon is Xeon CPU E5-2683 v3 @ 2.00GHz, it has two CPU for total 28 cores, but support upto 56 threads
 * Paul is Xeon CPU E5-2695 v2 @ 2.40GHz, it has two CPU for total 24 cores, support upto 48 threads
 * Fornax Intel® Xeon® E5-2699 v3 2.3GHz, it has two CPU for total 36 cores, support upto 72 threads.
 *
 * Use -O0 optimization
 */

 double read_timer_ms() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time * 1000.0 + (double) tm.millitm;
}

int dummy = 0;
/**
 * TODO: how to make sure that an empty parallel do not get optimized out by the compiler
 */
int main(int argc, char * argv[]) {
	int nthreads = 4;
    int policy = omp_thread_state_PASSIVE;
    printf("Usage: a.out [<nthreads>] [policy: 1: SPIN(ACTIVE), 2: YIELD, 3: SLEEP(PASSIVE), 4: KILL], default %d threads, PASSIVE policy\n", nthreads);
	if (argc >= 2) nthreads = (atoi(argv[1]));
    if (argc >= 3) {
        policy = atoi(argv[2]);
        if (policy == 1) policy = omp_thread_state_SPIN;
        if (policy == 2) policy = omp_thread_state_YIELD;
        if (policy == 3) policy = omp_thread_state_SLEEP;
        if (policy == 4) policy = omp_thread_state_KILL;
    }

    int i;
    int NUM_ITERATIONS = 1000;
    double overhead = 0.0;

    double overhead_parallel = read_timer_ms();
    for (i=0; i<NUM_ITERATIONS; i++) {
		#pragma omp parallel num_threads(nthreads)
		{
			//int tid = omp_get_thread_num();
		}
		
		double temp2  = read_timer_ms();
		omp_quiesce(policy);
		overhead += read_timer_ms() - temp2;
    }
    overhead_parallel = read_timer_ms() - overhead_parallel;

    omp_set_wait_policy(OMP_ACTIVE_WAIT);
 	// this is for not quiesce
    double parallel_overhead = read_timer_ms();
    for (i=0; i<NUM_ITERATIONS; i++) {
		#pragma omp parallel num_threads(nthreads)
		{
    		//int tid = omp_get_thread_num();
		}
    }
    parallel_overhead = read_timer_ms() - parallel_overhead;
    printf("%d thread, policy: %d\n", nthreads, policy);
    printf("set_wait_policy/quiesce overhead      : %f\n", overhead/NUM_ITERATIONS);
    printf("parallel startup overhead because of the policy: %f\n", (overhead_parallel - parallel_overhead - overhead)/NUM_ITERATIONS);

	// while(1);
     return 0;
}
