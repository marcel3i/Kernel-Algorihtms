#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

// Define QUANTUM if not already defined (default time slice)
#ifndef QUANTUM
#define QUANTUM 4
#endif

// Global Round Robin queue.
static Node *rr_queue = NULL;
static int global_job_id_rr = 0;
// Array to store original burst times indexed by job_id.
static int originalBurstRR[1000] = {0};

/**
 * enqueueJobRR
 * Appends a new job to the Round Robin queue.
 */
void enqueueJobRR(char *job_name, int priority, int burst, int start_time, int deadline) {
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name = strdup(job_name);
    newJob->priority = priority;
    newJob->burst = burst;
    newJob->start_time = start_time;
    newJob->deadline = deadline; // Not used in RR
    newJob->job_id = global_job_id_rr++;
    
    originalBurstRR[newJob->job_id] = burst;
    
    Node *newNode = malloc(sizeof(Node));
    newNode->job = newJob;
    newNode->next = NULL;
    
    // Append to tail.
    if (rr_queue == NULL) {
        rr_queue = newNode;
    } else {
        Node *temp = rr_queue;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}

/**
 * runSchedulerRR
 * Implements Round Robin scheduling with start time support.
 * It uses a simple linked-list queue where jobs are reinserted until completion.
 */
 void runSchedulerRR() {
    int current_time = 0;
    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    float total_response_time = 0;
    int count = 0;

    // Keep a queue for jobs that need to be scheduled
    Node *queue = rr_queue;

    while (queue != NULL) {
        // Check for the next ready job
        int min_start = -1;
        Node *temp = queue;
        while (temp != NULL) {
            if (min_start == -1 || temp->job->start_time < min_start)
                min_start = temp->job->start_time;
            temp = temp->next;
        }
        if (current_time < min_start)
            current_time = min_start;

        // Dequeue the first job
        Node *currentNode = queue;
        queue = queue->next;

        // Record first execution time as response time
        if ((currentNode->job->job_id & 0x100000) == 0) {
            total_response_time += (current_time - currentNode->job->start_time);
            currentNode->job->job_id |= 0x100000;  // Mark as started
        }

        // Enforce quantum slicing
        int exec_time = (currentNode->job->burst > QUANTUM) ? QUANTUM : currentNode->job->burst;
        run(currentNode->job, exec_time);
        current_time += exec_time;
        currentNode->job->burst -= exec_time;

        // If job is not finished, reinsert at tail
        if (currentNode->job->burst > 0) {
            currentNode->next = NULL;
            if (queue == NULL) {
                queue = currentNode;
            } else {
                Node *iter = queue;
                while (iter->next != NULL)
                    iter = iter->next;
                iter->next = currentNode;
            }
        } else {
            // Job finished
            int turnaround_time = current_time - currentNode->job->start_time;
            int waiting_time = turnaround_time - originalBurstRR[currentNode->job->job_id & 0x0FFFFF];
            total_turnaround_time += turnaround_time;
            total_waiting_time += waiting_time;
            count++;
            free(currentNode->job->job_name);
            free(currentNode->job);
            free(currentNode);
        }
    }

    // Print correct Round Robin scheduling metrics
    printf("\nRound Robin Scheduling Metrics:\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / count);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / count);
    printf("Average response time = %.2f\n", total_response_time / count);
}

