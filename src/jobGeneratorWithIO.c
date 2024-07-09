/*
    This file generates Jobs with I/O bursts
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//change this macro to change number of jobs generated
#define numberOfJobs 15
 
//a utility function for randomly generating jobs
void randomlyGenerateJobs(){
    //seeding random function
    srand(time(NULL));

    //opening file to write jobs to
    FILE *filePointer;
    filePointer = fopen("jobs.txt", "w");

    //printing I/O time in the file(jobs.txt)
    int ioTime = (rand()%10) + 1;
    fprintf(filePointer, "%d\n", ioTime);

    fprintf(filePointer, "%s   %s   %s\n", "Job No.", "CPU Burst", "Time of Arrival");

    //writing to jobs.txt
    int time = 0;
    for(int job=1; job<=numberOfJobs; job++){
        //randomly creating CPU Burst for a job
        int cpuBurst = (rand()%20) + 11;
        //int cpuBurst = (rand()%5) + 1;

        int lagArrival = (rand()%4) + 1;
        if(job == 1) lagArrival = 0;
        time += lagArrival;

        //writing it to the file
        fprintf(filePointer, "%d         %d          %d %s\n", job, cpuBurst, time, "ms");
    }

    //closing the file
    fclose(filePointer);
}

int main(){

    randomlyGenerateJobs();

    return 0;
}
