#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_JOBS 15
#define TIME_QUANTUM 4 // Adjust the time quantum as needed

// Structure to represent a job
struct Job {
    int id;
    int cpuBurst;
    int arrivalTime;
    int remainingTime;
    int ioTime;
    int responseTime;
};

// Function to read jobs from the input file
void readJobsFromFile(struct Job jobs[], int *numJobs, int *ioTime) {
    FILE *file = fopen("jobs.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Read the I/O time from the first line
    fscanf(file, "%d", ioTime);
    *numJobs = 0;

    // Skip the header line
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (sscanf(buffer, "%d %d %d", &jobs[*numJobs].id, &jobs[*numJobs].cpuBurst, &jobs[*numJobs].arrivalTime) == 3) {
            jobs[*numJobs].remainingTime = jobs[*numJobs].cpuBurst;
            jobs[*numJobs].ioTime = *ioTime;
            jobs[*numJobs].responseTime = -1;
            (*numJobs)++;
        }
    }

    fclose(file);
}

int main() {
    struct Job jobs[NUMBER_OF_JOBS];
    int numJobs;
    int ioTime;
    readJobsFromFile(jobs, &numJobs, &ioTime);

    // Initialize variables for metrics
    int currentTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;
    int totalResponseTime = 0;

    // Create a log file
    FILE *logFile = fopen("logFileRR.txt", "w");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(1);
    }

    //fprintf(logFile, "Job No.  Arrival Time  CPU Burst  Turnaround Time  Response Time  Waiting Time\n");

    // Implement Round Robin scheduling
    int remainingJobs = numJobs;
    int currentJob = 0;

    while (remainingJobs > 0) {
        struct Job *job = &jobs[currentJob];

        if(job->responseTime == -1){
            job->responseTime = currentTime + 1;
            totalResponseTime += jobs->responseTime;
        }

        if (job->remainingTime > 0) {
            int timeSlice = (job->remainingTime < TIME_QUANTUM) ? job->remainingTime : TIME_QUANTUM;
            //fprintf(logFile, "%6d %12d %10d ", job->id, job->arrivalTime, timeSlice);

            job->remainingTime -= timeSlice;
            currentTime += timeSlice;

            if (job->remainingTime == 0) {
                // Job has completed
                int turnaroundTime = currentTime - job->arrivalTime;
                int responseTime = turnaroundTime - job->cpuBurst;
                int waitingTime = responseTime - job->cpuBurst;

                totalTurnaroundTime += turnaroundTime;
                //totalResponseTime += responseTime;
                totalWaitingTime += waitingTime;

                //fprintf(logFile, "%15d %14d %13d\n", turnaroundTime, responseTime, waitingTime);
                remainingJobs--;
            } else {
                // The job goes for I/O
                currentTime += job->ioTime;
                //fprintf(logFile, "\n");
            }
        }

        currentJob = (currentJob + 1) % numJobs;
    }

    // Calculate and print averages
    double avgTurnaroundTime = (double)totalTurnaroundTime / numJobs;
    double avgResponseTime = (double)totalResponseTime / numJobs;
    double avgWaitingTime = (double)totalWaitingTime / numJobs;

    fprintf(logFile, "Average Turnaround Time: %.2lf\n", avgTurnaroundTime);
    fprintf(logFile, "Average Response Time: %.2lf\n", avgResponseTime);
    fprintf(logFile, "Average Waiting Time: %.2lf\n", avgWaitingTime);

    fclose(logFile);

    FILE *datFile = fopen("metricsRR.dat", "w");

    fprintf(datFile, "TurnaroundTime: %.2lf\n", avgTurnaroundTime);
    fprintf(datFile, "WaitingTime: %.2lf\n", avgWaitingTime);
    fprintf(datFile, "ResponseTime: %.2lf\n", avgResponseTime);

    //fprintf(datFile, "%.2lf %.2lf %.2lf", avgWaitingTime, avgTurnaroundTime, avgResponseTime);

    fclose(datFile);

    return 0;
}
