#!/bin/bash
num_pthreads=$1
policy=$2
all_ompthreads_options="2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 64 72"
# all_ompthreads_options="2 3 4 5 6 7 8 9 10 11"

for j in 2 4 8
do
echo "-----------------------------------------------------------------------------------------------------------------------------"
echo "${j} pthreads test"
echo "-----------------------------------------------------------------------------------------------------------------------------"
for p in 1 2 3 4
do
#echo "-----------------------------------------------------------------------------------------------------------------------------"
#echo "${j} pthreads test. policy: $p(policy: 1: SPIN_BUSY, 2: SPIN_PAUSE(ACTIVE), 3: SPIN_YIELD, 4: SUSPEND(PASSIVE), 5: TERMINATE)"
#echo "# OpenMP threads per pthread options: ${all_ompthreads_options}"
#echo "Time in ms"
#echo "-----------------------------------------------------------------------------------------------------------------------------"
for i in ${all_ompthreads_options}
do
 #  ./omp_set_wait_policy_pthread ${num_pthreads} ${i} ${policy}
   ./omp_set_wait_policy_pthread ${j} ${i} ${p} 0
   sleep 1
done
echo ""
done
done 

