// input number of thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <omp.h>
#include <sys/timeb.h>
#include <unistd.h>

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

/**
 * TODO: how to make sure that an empty parallel do not get optimized out by the compiler
 */
int main(int argc, char * argv[]) {
	int nthreads = 4;
    int policy = omp_thread_state_SLEEP;
    int policy_input = 3;
    printf("Usage: a.out [<nthreads>] [policy: 1: SPIN(ACTIVE), 2: YIELD, 3: SLEEP(PASSIVE), 4: KILL], default %d threads, PASSIVE policy\n", nthreads);
	if (argc >= 2) nthreads = (atoi(argv[1]));
    if (argc >= 3) {
        policy_input = atoi(argv[2]); 
        if (policy_input == 1) policy = omp_thread_state_SPIN;
        if (policy_input == 2) policy = omp_thread_state_YIELD;
        if (policy_input == 3) policy = omp_thread_state_SLEEP;
        if (policy_input == 4) policy = omp_thread_state_KILL;
    }
    printf("%d thread, policy: %d\n", nthreads, policy_input);

    int i;
    int NUM_ITERATIONS = 1000;
    double overhead = 0.0;
    int dummy[nthreads][64];
    double cost_all = read_timer_ms();
    for (i=0; i<NUM_ITERATIONS; i++) {
    		omp_set_num_threads(nthreads); /* this call init runtime */
		#pragma omp parallel
		{
		//	int tid = omp_get_thread_num();
		//	dummy[tid][0] = i;
		}
		
		double temp2  = read_timer_ms();
		omp_quiesce(policy);
		overhead += read_timer_ms() - temp2;
		usleep(3000);
    }
    cost_all = read_timer_ms() - cost_all;

    omp_set_wait_policy(OMP_ACTIVE_WAIT);
 	// this is for not quiesce
    double parallel_overhead = read_timer_ms();
    for (i=0; i<NUM_ITERATIONS; i++) {
    		omp_set_num_threads(nthreads); /* this call init runtime */
		#pragma omp parallel
		{
		//	int tid = omp_get_thread_num();
		//	dummy[tid][0] = i;
		}
		usleep(3000);
    }
    parallel_overhead = read_timer_ms() - parallel_overhead;
    printf("set_wait_policy/quiesce overhead(us)      : %f\n", 1000*overhead/NUM_ITERATIONS);
    printf("parallel startup overhead because of the policy(us): %f\n", 1000*(cost_all - parallel_overhead - overhead)/NUM_ITERATIONS);

	// while(1);
     return 0;
}
