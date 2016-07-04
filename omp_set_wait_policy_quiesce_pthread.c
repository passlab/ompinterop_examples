// 2-process
// set wait policy, test2(0):passive / test2(1):active
// uncomment omp_quiesce() and omp_begin2() to check the time for quiesce policy

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include <sys/timeb.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void *omp_parallel_foo(void *ptr);

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
/* 1s = 1,000 ms = 1,000,000 us */
void busy_waiting(double usecond) {
    clock_t ticks1, ticks2;

    ticks1 = clock();
    ticks2 = ticks1;
    int second = usecond/1000000.0;
    while(((double)(ticks2-ticks1))/CLOCKS_PER_SEC < second)
        ticks2 = clock();

    // printf("It took %ld ticks to wait one second.\n",ticks2-ticks1);
    // printf("This value should be the same as CLOCKS_PER_SEC which is %d.\n",CLOCKS_PER_SEC);
}
double read_timer_ms() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time * 1000.0 + (double) tm.millitm;
}

int togo = 0;
int policy = omp_thread_state_SLEEP;
int num_ompthreads = 4;

int main(int argc, char * argv[])
{
    int num_pthreads = 2;
    int policy_input = 3;
    printf("Usage: a.out [<num_pthreads>] [num_ompthreads] [policy: 1: SPIN(ACTIVE), 2: YIELD, 3: SLEEP(PASSIVE), 4: KILL], default %d threads, PASSIVE policy\n", num_ompthreads);
    if (argc >= 2) num_pthreads = (atoi(argv[1]));
    if (argc >= 3) num_ompthreads = (atoi(argv[2]));
    if (argc >= 4) {
        policy_input = atoi(argv[3]);
        if (policy_input == 1) policy = omp_thread_state_SPIN;
        if (policy_input == 2) policy = omp_thread_state_YIELD;
        if (policy_input == 3) policy = omp_thread_state_SLEEP;
        if (policy_input == 4) policy = omp_thread_state_KILL;
    }
    printf("%d pthreads each creates %d OpenMP thread. Test policy: %d\n", num_pthreads, num_ompthreads, policy_input);
    pthread_t pthreads[num_pthreads];
    pthread_attr_t pthread_attr;

    pthread_attr_init( &pthread_attr );
    pthread_attr_setdetachstate( & pthread_attr, PTHREAD_CREATE_JOINABLE );
    pthread_attr_setscope(&pthread_attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setschedpolicy(&pthread_attr, SCHED_FIFO); /* or SCHED_RR */
    /*
    int fifo_max_prio, fifo_min_prio;
    struct sched_param fifo_param;
    pthread_attr_init(&pthread_attr);
    pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&pthread_attr, SCHED_FIFO);
    fifo_max_prio = sched_get_priority_max(SCHED_FIFO);
    fifo_min_prio = sched_get_priority_min(SCHED_FIFO);
    fifo_mid_prio = (fifo_min_prio + fifo_max_prio)/2;
    fifo_param.sched_priority = fifo_mid_prio;
    pthread_attr_setschedparam(&pthread_attr, &fifo_param);
    */
    pthread_attr_setinheritsched(&pthread_attr, PTHREAD_INHERIT_SCHED);

    int i;
    for (i=0; i<num_pthreads; i++) {
        pthread_create(&pthreads[i], &pthread_attr, omp_parallel_foo, (void *) i);
    }

    double elapsed = read_timer_ms();
    togo = 1; /* fire them at once */

    for (i=0; i<num_pthreads; i++) {
        pthread_join(pthreads[i], NULL);
    }

    elapsed = read_timer_ms() - elapsed;
    printf("Elapsed(ms): %f\n", elapsed);

    return 0;
}

#define NUM_ITERATIONS 1000
void *omp_parallel_foo(void *ptr )
{
    int user_thread_id = (int) ptr;
    int i;
    while (!togo);
    for (i=0; i<NUM_ITERATIONS; i++) {
        /* busy wait for one second */
        busy_waiting(user_thread_id*3000);
        omp_set_num_threads(num_ompthreads);
#pragma omp parallel
        {
            /* busy waiting, the whole parallel region should takes 1s */
            busy_waiting(3000);
        }
        omp_quiesce(policy); /* or active */
    //    usleep(3000); /* usleep so the policy applied */
    }
    return NULL;
}
