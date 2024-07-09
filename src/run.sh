#!/bin/bash
#creating jobs for the scheduler
gcc -o generate jobGeneratorWithIO.c
./generate
wait
rm generate

gcc -o scheduler MLFQ.c
./scheduler
wait
rm scheduler

gcc -o scheduler roundrobin.c
./scheduler
wait
rm scheduler

cat log.txt

gnuplot graphNovel.gp

gnuplot graphRR.gp
