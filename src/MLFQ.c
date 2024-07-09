#include <stdio.h>
#include <stdlib.h>

#define numJobs 30

// Define process structure
typedef struct {
    int id;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int arrival_time; // Arrival time of the process
    int io_burst;     // I/O burst time
} Process;

// Define queue structure
typedef struct {
    Process** processes;
    int front;
    int rear;
    int capacity;
} Queue;

// Global variables for queues and other shared data
Queue* q1;
Queue* q2;
Queue* q3;
int time_quantum_q1 = 5;
int time_quantum_q2 = 8;
int time_quantum_q3 = 15;
int io_threshold = 10; // Threshold for I/O
int total_response_time = 0;
int total_waiting_time = 0;
int total_turnaround_time = 0;
int total_processes = 0;
int realNumJobs = 0;

// Function to create a new process
Process* createProcess(int id, int burst_time, int arrival_time, int io_burst) {
    Process* process = (Process*)malloc(sizeof(Process));
    process->id = id;
    process->burst_time = burst_time;
    process->remaining_time = burst_time;
    process->waiting_time = 0;
    process->turnaround_time = 0;
    process->response_time = -1;
    process->arrival_time = arrival_time;
    process->io_burst = io_burst;
    return process;
}

// Function to create a new queue
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->processes = (Process**)malloc(sizeof(Process*) * capacity);
    queue->front = queue->rear = -1;
    queue->capacity = capacity;
    return queue;
}

// Function to enqueue a process
void enqueue(Queue* queue, Process* process) {
    if (queue->rear == queue->capacity - 1) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (queue->front == -1) {
        queue->front = 0;
    }
    queue->rear++;
    queue->processes[queue->rear] = process;
}

// Function to dequeue a process
Process* dequeue(Queue* queue) {
    if (queue->front == -1) {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }
    Process* process = queue->processes[queue->front];
    if (queue->front == queue->rear) {
        queue->front = queue->rear = -1;
    } else {
        queue->front++;
    }
    return process;
}

// Function to run processes in the multi-level feedback queue
void runMLFQ(FILE* logFile) {
    int current_time = 0;

    while (1) {
        // Check Q1 (RR5)
        if (q1->front != -1 && current_time >= q1->processes[q1->front]->arrival_time) {
            Process* process = dequeue(q1);
            int quantum = time_quantum_q1;

            if(process->response_time == -1){
                process->response_time = current_time - process->arrival_time; 
            }

            if (process->remaining_time > quantum) {
                printf("Running process %d from Q1 for time quantum %d\n", process->id, quantum);
                process->remaining_time -= quantum;
                current_time += quantum;
                process->waiting_time += quantum;

                // Check for I/O burst
                if (process->waiting_time >= io_threshold && process->io_burst > 0) {
                    printf("Process %d is performing I/O burst for time %d\n", process->id, process->io_burst);
                    //current_time += process->io_burst;
                    process->waiting_time += process->io_burst;
                    process->io_burst = 0; // Reset io burst time time
                }

                enqueue(q2, process); // Move to the end of Q2
            } else {
                printf("Running process %d from Q1 for remaining time %d\n", process->id, process->remaining_time);
                current_time += process->remaining_time;
                process->turnaround_time = current_time - process->arrival_time;
                process->waiting_time += process->remaining_time;

                // Update totals for averages
                total_response_time += process->response_time;
                //total_waiting_time += process->waiting_time;
                total_waiting_time += process->turnaround_time - process->waiting_time;
                total_turnaround_time += process->turnaround_time;
                realNumJobs++;

                // Print process info to the log file
                fprintf(logFile, "[%d | %d | %d | %d | %d]\n", process->id, process->arrival_time,
                        process->turnaround_time - process->burst_time, process->turnaround_time, process->response_time);

                free(process);
                total_processes++;
            }
        } else if (q2->front != -1 && current_time >= q2->processes[q2->front]->arrival_time) { // Check Q2 (RR8)
            Process* process = dequeue(q2);
            int quantum = time_quantum_q2;

            if (process->remaining_time > quantum) {
                printf("Running process %d from Q2 for time quantum %d\n", process->id, quantum);
                process->remaining_time -= quantum;
                current_time += quantum;
                process->waiting_time += quantum;

                // Check for I/O burst
                if (process->waiting_time >= io_threshold && process->io_burst > 0) {
                    printf("Process %d is performing I/O burst for time %d\n", process->id, process->io_burst);
                    //current_time += process->io_burst;
                    process->waiting_time += process->io_burst; // Reset waiting time
                    process->io_burst = 0;
                }

                enqueue(q3, process); // Move to the end of Q3
            } else {
                printf("Running process %d from Q2 for remaining time %d\n", process->id, process->remaining_time);
                current_time += process->remaining_time;
                process->turnaround_time = current_time - process->arrival_time;
                process->waiting_time += process->remaining_time;

                // Update totals for averages
                total_response_time += process->response_time;
                //total_waiting_time += process->waiting_time;
                total_waiting_time += process->turnaround_time - process->waiting_time;
                total_turnaround_time += process->turnaround_time;
                realNumJobs++;

                // Print process info to the log file
                fprintf(logFile, "[%d | %d | %d | %d | %d]\n", process->id, process->arrival_time,
                        process->turnaround_time - process->burst_time, process->turnaround_time, process->response_time);

                free(process);
                total_processes++;
            }
        } else if (q3->front != -1 && current_time >= q3->processes[q3->front]->arrival_time) { // Check Q3 (RR15)
            Process* process = dequeue(q3);
            int quantum = time_quantum_q3;

            // if (process->response_time == -1) {
            //     process->response_time = current_time - process->arrival_time;
            // }

            if (process->remaining_time > quantum) {
                printf("Running process %d from Q3 for time quantum %d\n", process->id, quantum);
                process->remaining_time -= quantum;
                current_time += quantum;
                process->waiting_time += quantum;

                // Check for I/O burst
                if (process->waiting_time >= io_threshold && process->io_burst > 0) {
                    printf("Process %d is performing I/O burst for time %d\n", process->id, process->io_burst);
                    //current_time += process->io_burst;
                    process->waiting_time += process->io_burst; // Reset waiting time
                    process->io_burst = 0;
                }

                enqueue(q3, process); // Move to the end of Q3
            } else {
                printf("Running process %d from Q3 for remaining time %d\n", process->id, process->remaining_time);
                current_time += process->remaining_time;
                process->turnaround_time = current_time - process->arrival_time;
                process->waiting_time += process->remaining_time;
                

                // Update totals for averages
                total_response_time += process->response_time;
                //total_waiting_time += process->waiting_time;
                total_waiting_time += process->turnaround_time - process->waiting_time;
                total_turnaround_time += process->turnaround_time;
                realNumJobs++;

                // Print process info to the log file
                fprintf(logFile, "[%d | %d | %d | %d | %d]\n", process->id, process->arrival_time,
                        process->turnaround_time - process->burst_time, process->turnaround_time, process->response_time);

                free(process);
                total_processes++;
            }
        } else if(q1->front == -1 && q2->front == -1 && q3->front == -1){
            break; // All queues are empty, exit the loop
        } else{
            current_time++;
        }
    }
}

int main() {
    // Initialize queues
    q1 = createQueue(numJobs);
    q2 = createQueue(numJobs);
    q3 = createQueue(numJobs);

    FILE *filePointer;
    filePointer = fopen("jobs.txt", "r");
    int itr = 0;
    char line[100]; int ioBurst;

    while (fgets(line, sizeof(line), filePointer)) {
        if (itr == 0) {
            // Read I/O burst time from the first line
            ioBurst = atoi(line);
            itr++;
        } else if (itr == 1) {
            // Skip the header line
            itr++;
        } else {
            // Read job attributes
            int cpu_burst, job, arrival_time;
            sscanf(line, "%d %d %d", &job, &cpu_burst, &arrival_time);
            Process *process = createProcess(job, cpu_burst, arrival_time, ioBurst);
            enqueue(q1, process);
        }
    }

    fclose(filePointer);

    // Open a log file for writing
    FILE* logFile = fopen("log.txt", "w");
    if (logFile == NULL) {
        printf("Failed to open the log file.\n");
        return 1;
    }

    //opening log file
    fprintf(logFile, "[Job No. | Arrival Time | Waiting Time | Turn Around Time | Response Time]\n");

    runMLFQ(logFile);

    fprintf(logFile, "Average Turn Around Time: %.2lf\n", (double)total_turnaround_time/realNumJobs);
    fprintf(logFile, "Average Waiting Time: %.2lf\n", (double)total_waiting_time/realNumJobs);
    fprintf(logFile, "Average Response Time: %.2lf\n", (double)total_response_time/realNumJobs);
    
    // Close the log file
    fclose(logFile);

    FILE *datFile = fopen("metricsMLFQ.dat", "w");

    fprintf(datFile, "TurnAroundTime %.2lf\n", (double)total_turnaround_time/realNumJobs);
    fprintf(datFile, "WaitingTime %.2lf\n", (double)total_waiting_time/realNumJobs);
    fprintf(datFile, "ResponseTime %.2lf\n", (double)total_response_time/realNumJobs);
    

    //fprintf(datFile, "%.2lf %.2lf %.2lf", (double)total_waiting_time/realNumJobs, (double)total_turnaround_time/realNumJobs, (double)total_response_time/realNumJobs);

    fclose(datFile);

    // Cleanup
    free(q1->processes);
    free(q1);
    free(q2->processes);
    free(q2);
    free(q3->processes);
    free(q3);

    return 0;
}

