#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"


static Node *step_job_list = NULL;
static int next_job_id = 0;

void enqueueJobSS(char *job_name,
                  int tickets,
                  int burst,
                  int start_time,
                  int deadline)
{
    Job *job = malloc(sizeof(Job));
    job->job_name   = strdup(job_name);
    job->job_id     = next_job_id++;
    job->priority   = tickets;     // priority - liczba kroków / ticketów 
    job->burst      = burst;
    job->start_time = start_time;
    job->deadline   = deadline;

    Node *node = malloc(sizeof(Node));
    node->job  = job;
    node->next = step_job_list;
    step_job_list = node;
}

void runSchedulerSS(void)
{
    // ile zadań
    int total_jobs = 0;
    for (Node *n = step_job_list; n; n = n->next)
        total_jobs++;

    // tablice pomocnicze
    int *initial_burst  = malloc(total_jobs * sizeof(int));
    int *responded = calloc(total_jobs,   sizeof(int));
    int *response_time = malloc(total_jobs * sizeof(int));
    int *step_counter  = calloc(total_jobs,   sizeof(int));

    // oryginalne bursty
    for (Node *n = step_job_list; n; n = n->next) {
        int id = n->job->job_id;
        initial_burst[id] = n->job->burst;
        step_counter[id]  = 0;  // zaczynamy liczenie kroków od 0
    }

    int remaining_jobs = total_jobs;
    int current_time   = 0;
    double sum_wt      = 0;
    double sum_tat     = 0;
    double sum_rt      = 0;

    // 4) symulacja tick po ticku
    while (remaining_jobs > 0) {
        // --- wybierz gotowe zadanie z najniższym step_counter ---
        Node *chosen      = NULL, *prev_chosen = NULL;
        Node *it          = step_job_list, *prev_it = NULL;
        int   found_any   = 0;

        while (it) {
            Job *j = it->job;
            if (j->start_time <= current_time) {
                if (!found_any || step_counter[j->job_id] <= step_counter[chosen->job->job_id]) {
                    chosen      = it;
                    prev_chosen = prev_it;
                    found_any   = 1;
                }
            }
            prev_it = it;
            it      = it->next;
        }

        // jeśli nic gotowe -> przesun zegar do najbliższego arrival
        if (!found_any) {
            int next_arr = -1;
            for (Node *n = step_job_list; n; n = n->next) {
                int at = n->job->start_time;
                if (next_arr < 0 || at < next_arr)
                    next_arr = at;
            }
            current_time = next_arr;
            continue;
        }

        // zapis response time (raz)
        Job *job = chosen->job;
        int  id  = job->job_id;
        if (!responded[id]) {
            response_time[id] = current_time - job->start_time;
            responded[id] = 1;
        }

        //  wykonaj 1 tick i aktuilizuj step counter
        run(job, 1);
        job->burst -= 1;
        step_counter[id] += job->priority;
        current_time += 1;

        // metryki + usunięcie z listy
        if (job->burst == 0) {
            int tat = current_time - job->start_time;
            int wt  = tat - initial_burst[id];
            sum_tat += tat;
            sum_wt  += wt;
            sum_rt  += response_time[id];

            if (prev_chosen)
                prev_chosen->next = chosen->next;
            else
                step_job_list   = chosen->next;

            free(job->job_name);
            free(job);
            free(chosen);
            remaining_jobs--;
        }
    }


    printf("\nStep Scheduling Metrics:\n");
    printf("Average waiting time    = %.2f\n", sum_wt  / total_jobs);
    printf("Average turnaround time = %.2f\n", sum_tat / total_jobs);
    printf("Average response time   = %.2f\n", sum_rt  / total_jobs);

    free(initial_burst);
    free(responded);
    free(response_time);
    free(step_counter);
}
