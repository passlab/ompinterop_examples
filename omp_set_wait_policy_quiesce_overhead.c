// input number of thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <omp.h>
#include <sys/timeb.h>
#include <unistd.h>

/**
 * This function measure the overhead of both set_wait_policy and quiesce since quiesce takes all the arguments
 * required for set_wait_policy plus the TERMINATE policy which shutdown the runtime
 *
 * Important: make sure you use omp_set_num_threads API before parallel to set the number of threads for
 * execution. The use of num_threads clause is not safe due to the way compiler generate the code and the way how
 * the runtime is initialized.
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
    int policy = OMP_SUSPEND_WAIT;
    int policy_input = 3;
    printf("Usage: a.out [<nthreads>] [policy: 1: SPIN_BUSY, 2: SPIN_PAUSE(ACTIVE), 3: SPIN_YIELD, 4: SUSPEND(PASSIVE), 5: TERMINATE], default %d threads, PASSIVE policy\n", nthreads);
	if (argc >= 2) nthreads = (atoi(argv[1]));
    if (argc >= 3) {
        policy_input = atoi(argv[2]); 
        if (policy_input == 1) policy = OMP_SPIN_BUSY_WAIT;
        if (policy_input == 2) policy = OMP_SPIN_PAUSE_WAIT;
        if (policy_input == 3) policy = OMP_SPIN_YIELD_WAIT;
        if (policy_input == 4) policy = OMP_SUSPEND_WAIT;
        if (policy_input == 5) policy = OMP_TERMINATE;
    }
    printf("%d thread, policy: %d\n", nthreads, policy_input);

    int i;
    int NUM_ITERATIONS = 1000;
    double overhead = 0.0;
    int dummy[nthreads][64];
    omp_set_num_threads(nthreads); /* this call init runtime */
		#pragma omp parallel
		{
		}
    double cost_all = read_timer_ms();
    for (i=0; i<NUM_ITERATIONS; i++) {
    	omp_set_num_threads(nthreads); /* this call init runtime */
		#pragma omp parallel
		{
		//	int tid = omp_get_thread_num();
		//	dummy[tid][0] = i;
		}
		
		double temp2  = read_timer_ms();
		if (policy == OMP_TERMINATE) omp_quiesce(policy);
		else omp_set_wait_policy(policy);
		overhead += read_timer_ms() - temp2;
		//usleep(3000);
    }
    cost_all = read_timer_ms() - cost_all;

    omp_set_num_threads(nthreads); /* this call init runtime */
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
		//usleep(3000);
    }
    parallel_overhead = read_timer_ms() - parallel_overhead;
    printf("set_wait_policy/quiesce overhead(us): %f, %f%% of parallel startup cost\n", 1000*overhead/NUM_ITERATIONS, 100*overhead/parallel_overhead);
    printf("parallel startup overhead because of the policy(us): %f\n", 1000*(cost_all - parallel_overhead - overhead)/NUM_ITERATIONS);

	// while(1);
     return 0;
}
