#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

#ifndef QUANTUM
#define QUANTUM 10
#endif

// Global array of queues—one for each priority level (1 to MAX_PRIORITY).
// Here we assume priorities 1..10.
static Node *priority_rr_queues[11] = {0};  
static int global_job_id_priority_rr = 0;
// Array to store original burst times.
static int originalBurstPRR[1000] = {0};

/**
 * enqueueJobPriorityRR
 * Inserts a new job into the queue corresponding to its priority.
 */
void enqueueJobPriorityRR(char *job_name, int priority, int burst, int start_time, int deadline) {
    if (priority < 1 || priority > 10) {
        fprintf(stderr, "Priority out of range: %d\n", priority);
        return;
    }
    
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name = strdup(job_name);
    newJob->priority = priority;
    newJob->burst = burst;
    newJob->start_time = start_time;
    newJob->deadline = deadline; // Not used here.
    newJob->job_id = global_job_id_priority_rr++;
    
    originalBurstPRR[newJob->job_id] = burst;
    
    Node *newNode = malloc(sizeof(Node));
    newNode->job = newJob;
    newNode->next = NULL;
    
    // Append to the tail of the corresponding priority queue.
    if (priority_rr_queues[priority] == NULL) {
        priority_rr_queues[priority] = newNode;
    } else {
        Node *temp = priority_rr_queues[priority];
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}

/**
 * runSchedulerPriorityRR
 * Implements a Priority Round Robin scheduler.
 * Jobs are scheduled from the highest priority (10) to the lowest (1) using RR.
 * Each queue is processed until empty.
 */
void runSchedulerPriorityRR() {
    int current_time = 0;
    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    float total_response_time = 0;
    int count = 0;
    
    // Continue until every priority queue is empty.
    int jobs_remaining = 1;
    while (jobs_remaining) {
        jobs_remaining = 0;
        // Process priorities from highest (10) down to lowest (1).
        for (int p = 10; p >= 1; p--) {
            while (priority_rr_queues[p] != NULL) {
                jobs_remaining = 1;
                // Check for ready jobs in this priority queue.
                int min_start = -1;
                Node *temp = priority_rr_queues[p];
                while (temp != NULL) {
                    if (min_start == -1 || temp->job->start_time < min_start)
                        min_start = temp->job->start_time;
                    temp = temp->next;
                }
                if (current_time < min_start)
                    current_time = min_start;
                
                // Dequeue the first job.
                Node *currentNode = priority_rr_queues[p];
                priority_rr_queues[p] = priority_rr_queues[p]->next;
                
                // Record response time on first execution.
                if ((currentNode->job->job_id & 0x100000) == 0) {
                    total_response_time += (current_time - currentNode->job->start_time);
                    currentNode->job->job_id |= 0x100000;
                }
                
                int exec_time = (currentNode->job->burst > QUANTUM) ? QUANTUM : currentNode->job->burst;
                run(currentNode->job, exec_time);
                current_time += exec_time;
                currentNode->job->burst -= exec_time;
                
                if (currentNode->job->burst > 0) {
                    // Reinsert at tail of the same priority queue.
                    currentNode->next = NULL;
                    if (priority_rr_queues[p] == NULL) {
                        priority_rr_queues[p] = currentNode;
                    } else {
                        Node *iter = priority_rr_queues[p];
                        while (iter->next != NULL)
                            iter = iter->next;
                        iter->next = currentNode;
                    }
                } else {
                    // Job completed.
                    int turnaround_time = current_time - currentNode->job->start_time;
                    int waiting_time = turnaround_time - originalBurstPRR[currentNode->job->job_id & 0x0FFFFF];
                    total_turnaround_time += turnaround_time;
                    total_waiting_time += waiting_time;
                    count++;
                    free(currentNode->job->job_name);
                    free(currentNode->job);
                    free(currentNode);
                }
            }
        }
    }
    
    printf("\nPriority Round Robin Scheduling Metrics:\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / count);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / count);
    printf("Average response time = %.2f\n", total_response_time / count);
}
