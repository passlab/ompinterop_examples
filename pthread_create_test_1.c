// execute in parallel
// input the number of thread 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <omp.h>
#include <sys/timeb.h>
#include <unistd.h>

/* read timer in second */
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

int running_to_sleep = 0;
void *running_thread(void *arg){
    printf("Running thread: %d\n", (int)arg);

    while(!running_to_sleep);
    sleep(1000);
    return arg;
}


void *yield_thread(void *arg){
    printf("Yield thread: %d\n", (int)arg);

    sched_yield();
    while(1);
    return arg;
}

void *sleep_thread(void *arg){
    printf("Sleep thread: %d\n", (int)arg);

    sleep(1000);
    return arg;
}

int num_running_threads = 8;
int num_yield_threads = 4;
int num_sleep_threads = 4;

int main(int argc, char * argv[])
{
    int tid; 
    if (argc >= 2){
        num_running_threads = atoi(argv[1]);
    } 
    if (argc >= 3){
        num_yield_threads = atoi(argv[2]);
    } 
    if (argc >= 4){
        num_sleep_threads = atoi(argv[3]);
    } 

    printf("thread running: %d, yield: %d, sleep: %d\n", num_running_threads, num_yield_threads, num_sleep_threads);
    
    int i;
    pthread_t running_threads[num_running_threads];
    for (i=0; i<num_running_threads; i++) {
        pthread_create(&running_threads[i], NULL, running_thread, (void*)i);
    }

    pthread_t yield_threads[num_yield_threads];
    for (i=0; i<num_yield_threads; i++) {
        pthread_create(&yield_threads[i], NULL, yield_thread, (void*)i);
    }
    
    pthread_t sleep_threads[num_sleep_threads];
    for (i=0; i<num_sleep_threads; i++) {
        pthread_create(&sleep_threads[i], NULL, sleep_thread, (void*)i);
    }
  
    printf("Please input an integer value to change running thread to sleep.");
    scanf("%d", &running_to_sleep);
    running_to_sleep = 1;
 
    while(1);
    return 0;
}
