#!/bin/bash
PID=`ps -o pid= -p $1`

if [ ! "${PID}" ]
then
   echo "process $1 does not exist!"
   exit
fi 

pwd=$PWD

proc_dir=/proc/$1/task

cd ${proc_dir}
seq=0
for th in `find . -maxdepth 1  -type d \( ! -name . \)`
do
   echo ${seq} ${th}: `cat ${th}/status  | grep State`

   ((seq++))
done

cd ${pwd}
