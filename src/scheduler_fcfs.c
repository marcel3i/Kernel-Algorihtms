#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

// Global linked list for FCFS scheduler.
static Node *fcfs_job_list = NULL;
static int global_job_id_fcfs = 0;

/**
 * enqueueJobFCFS
 * Appends a new job (with start_time and deadline) to the FCFS queue.
 */
void enqueueJobFCFS(char *job_name, int priority, int burst, int start_time, int deadline) {
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name = strdup(job_name);
    newJob->priority = priority;
    newJob->burst = burst;
    newJob->start_time = start_time;
    newJob->deadline = deadline; // Not used in FCFS
    newJob->job_id = global_job_id_fcfs++;
    
    Node *newNode = malloc(sizeof(Node));
    newNode->job = newJob;
    newNode->next = NULL;
    
    // Append new node to tail.
    if (fcfs_job_list == NULL) {
        fcfs_job_list = newNode;
    } else {
        Node *temp = fcfs_job_list;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}

/**
 * runSchedulerFCFS
 * Implements First-Come First-Served scheduling with start time support.
 */
void runSchedulerFCFS() {
    int current_time = 0;
    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    float total_response_time = 0;
    int count = 0;
    
    Node *temp;
    while (fcfs_job_list != NULL) {
        // Dequeue the first job.
        temp = fcfs_job_list;
        fcfs_job_list = fcfs_job_list->next;
        
        // Wait if the job is not yet ready.
        if (current_time < temp->job->start_time)
            current_time = temp->job->start_time;
        
        int waiting_time = current_time - temp->job->start_time;
        total_waiting_time += waiting_time;
        total_response_time += waiting_time;
        
        run(temp->job, temp->job->burst);
        current_time += temp->job->burst;
        total_turnaround_time += (current_time - temp->job->start_time);
        count++;
        
        free(temp->job->job_name);
        free(temp->job);
        free(temp);
    }
    
    printf("\nFCFS Scheduling Metrics:\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / count);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / count);
    printf("Average response time = %.2f\n", total_response_time / count);
}
