#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

// Global linked list for EDF scheduler.
static Node *edf_job_list = NULL;
static int global_job_id_edf = 0;

/**
 * enqueueJobEDF
 * Adds a new job to the EDF scheduler list. In EDF scheduling, each job must
 * have a valid deadline (set to a non-negative value).
 */
void enqueueJobEDF(char *job_name, int priority, int burst, int start_time, int deadline) {
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name = strdup(job_name);
    newJob->priority = priority;
    newJob->burst = burst;
    newJob->start_time = start_time;
    newJob->deadline = deadline; // Deadline must be provided for EDF
    newJob->job_id = global_job_id_edf++;
    
    // Insert at the beginning (unsorted list)
    Node *newNode = malloc(sizeof(Node));
    newNode->job = newJob;
    newNode->next = edf_job_list;
    edf_job_list = newNode;
}

/**
 * runSchedulerEDF
 * Implements a non-preemptive Earliest Deadline First (EDF) scheduler. The scheduler
 * selects the ready job (current_time >= start_time) with the earliest deadline.
 * If no job is ready, current_time is advanced to the next job’s start time.
 */
void runSchedulerEDF() {
    int current_time = 0;
    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    float total_response_time = 0;
    int count = 0;
    
    // Continue until all jobs have been processed.
    while (edf_job_list != NULL) {
        // Find the ready job with the earliest deadline.
        Node *selected = NULL;
        Node *selected_prev = NULL;
        Node *iterator = edf_job_list;
        Node *iterator_prev = NULL;
        int found = 0;
        
        while (iterator != NULL) {
            if (iterator->job->start_time <= current_time) {
                if (!found || iterator->job->deadline < selected->job->deadline) {
                    selected = iterator;
                    selected_prev = iterator_prev;
                    found = 1;
                }
            }
            iterator_prev = iterator;
            iterator = iterator->next;
        }
        
        // If no job is ready, advance time to the earliest start time.
        if (!found) {
            int min_start = -1;
            Node *temp = edf_job_list;
            while (temp != NULL) {
                if (min_start == -1 || temp->job->start_time < min_start) {
                    min_start = temp->job->start_time;
                }
                temp = temp->next;
            }
            current_time = min_start;
            continue;
        }
        
        // Remove the selected job from the list.
        if (selected_prev == NULL) {
            edf_job_list = selected->next;
        } else {
            selected_prev->next = selected->next;
        }
        
        // Compute waiting and response times.
        int waiting_time = current_time - selected->job->start_time;
        if (waiting_time < 0)
            waiting_time = 0;
        total_waiting_time += waiting_time;
        total_response_time += waiting_time;
        
        // Run the job to completion.
        run(selected->job, selected->job->burst);
        current_time += selected->job->burst;
        total_turnaround_time += (waiting_time + selected->job->burst);
        count++;
        
        // Free the selected job.
        free(selected->job->job_name);
        free(selected->job);
        free(selected);
    }
    
    printf("\nEDF Scheduling Metrics:\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / count);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / count);
    printf("Average response time = %.2f\n", total_response_time / count);
}
