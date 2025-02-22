#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

// Global list for non-preemptive Priority scheduling.
static Node *priority_list = NULL;
static int global_job_id_priority = 0;

/**
 * enqueueJobPriority
 * Inserts a new job into the priority scheduling list.
 * (The list is unsorted; selection is done at runtime.)
 */
void enqueueJobPriority(char *job_name, int priority, int burst, int start_time, int deadline) {
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name = strdup(job_name);
    newJob->priority = priority;
    newJob->burst = burst;
    newJob->start_time = start_time;
    newJob->deadline = deadline; // Not used here.
    newJob->job_id = global_job_id_priority++;
    
    Node *newNode = malloc(sizeof(Node));
    newNode->job = newJob;
    newNode->next = priority_list;
    priority_list = newNode;
}

/**
 * runSchedulerPriority
 * Implements a non-preemptive Priority scheduler with start time support.
 * At each step, it scans for the ready job with the highest priority.
 */
void runSchedulerPriority() {
    int current_time = 0;
    float total_waiting_time = 0;
    float total_turnaround_time = 0;
    float total_response_time = 0;
    int count = 0;
    
    while (priority_list != NULL) {
        // Find the ready job with the highest priority.
        Node *selected = NULL;
        Node *selected_prev = NULL;
        Node *iter = priority_list;
        Node *iter_prev = NULL;
        int found = 0;
        
        while (iter != NULL) {
            if (iter->job->start_time <= current_time) {
                if (!found || iter->job->priority > selected->job->priority) {
                    selected = iter;
                    selected_prev = iter_prev;
                    found = 1;
                }
            }
            iter_prev = iter;
            iter = iter->next;
        }
        
        if (!found) {
            // No job ready—advance time to the earliest start time.
            int min_start = -1;
            Node *temp = priority_list;
            while (temp != NULL) {
                if (min_start == -1 || temp->job->start_time < min_start)
                    min_start = temp->job->start_time;
                temp = temp->next;
            }
            current_time = min_start;
            continue;
        }
        
        // Remove the selected node.
        if (selected_prev == NULL)
            priority_list = selected->next;
        else
            selected_prev->next = selected->next;
        
        int waiting_time = current_time - selected->job->start_time;
        if (waiting_time < 0)
            waiting_time = 0;
        total_waiting_time += waiting_time;
        total_response_time += waiting_time;
        
        run(selected->job, selected->job->burst);
        current_time += selected->job->burst;
        total_turnaround_time += (current_time - selected->job->start_time);
        count++;
        
        free(selected->job->job_name);
        free(selected->job);
        free(selected);
    }
    
    printf("\nPriority Scheduling Metrics:\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / count);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / count);
    printf("Average response time = %.2f\n", total_response_time / count);
}
