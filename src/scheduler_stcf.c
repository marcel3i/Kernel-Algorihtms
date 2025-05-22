#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "job.h"
#include "cpu.h"
#include "list.h"

static Node *stcf_job_list = NULL;
static int global_job_id_stcf = 0;

void enqueueJobSTCF(char *job_name, int priority, int burst, int start_time, int deadline) {
    Job *newJob = malloc(sizeof(Job));
    newJob->job_name  = strdup(job_name);
    newJob->priority  = priority; // nieuzywane
    newJob->burst     = burst;       // tu przechowujemy "remaining time"
    newJob->start_time= start_time;
    newJob->deadline  = deadline;    // nieuzywane
    newJob->job_id    = global_job_id_stcf++;
    Node *n = malloc(sizeof(Node));
    n->job  = newJob;
    n->next = stcf_job_list;
    stcf_job_list = n;
}

void runSchedulerSTCF() {
    int total_jobs = 0;
    for (Node *t = stcf_job_list; t; t = t->next) total_jobs++;

    // tablice do metryk
    int *initial_burst = malloc(total_jobs * sizeof(int)); // poczatkowy burst przed symulacja
    int *resp_time    = malloc(total_jobs * sizeof(int)); // do średniego resposne time
    int *comp_time    = malloc(total_jobs * sizeof(int)); // czas zakonczenia kazdego zadania
    int *responded    = calloc(total_jobs, sizeof(int)); // flagi czy dla danego job zmierzylismy juz response time

    // zapamiętanie początkowych czasów trwania
    for (Node *t = stcf_job_list; t; t = t->next) {
        initial_burst[t->job->job_id] = t->job->burst;
    }

    // Sumatory czasów (dla wartosci sredniej pod koniec)
    double total_wt  = 0;  // suma Waiting Time
    double total_tat = 0;  // suma Turnaround Time
    double total_rt  = 0;  // suma Response Time

    int jobs_left    = total_jobs;  // ile jeszcze nieukończonych zadań zostało
    int current_time = 0;     

    // Mian loop
    while (jobs_left > 0) {
      
        Node *selected_job      = NULL;  // wskaźnik na wybrane zadanie
        Node *selected_previous = NULL;  // wskaźnik na poprzednik w liście (potrzebny do usunięcia)
        Node *it       = stcf_job_list;  // iterator po całej liście
        Node *it_prev  = NULL;  // poprzednik iteratora
        int found      = 0;     // flaga: czy znaleźliśmy choć jedno ready zadanie

        // Wybór z listy:
        // zadanie które już się pojawiło (start_time <= current_time)
        // spośród nich to z najmniejszym burst (remaining time)
        while (it) {
            if (it->job->start_time <= current_time) {
                // to pierwsze znalezione, albo ma mniejszy remaining niż dotychczasowe selected_job
                if (!found || it->job->burst < selected_job->job->burst) {
                    selected_job       = it;
                    selected_previous  = it_prev;
                    found     = 1;
                }
            }
            it_prev = it;
            it      = it->next;
        }

        // jak nie ma gotowych zadań, przeskoczenie do momentu, gdzie pojawia się nowe zadanie
        if (!found) {
            int min_start_time = -1;  
            for (Node *t = stcf_job_list; t; t = t->next) {
                if (min_start_time < 0 || t->job->start_time < min_start_time)
                    min_start_time = t->job->start_time;
            }
            current_time = min_start_time;
            continue;  // poczatek petli
        }

        // resposne time 
        int id = selected_job->job->job_id; 
        if (!responded[id]) {
            // pierwsze przydzielenie CPU
            resp_time[id] = current_time - selected_job->job->start_time;
            responded[id] = 1;  
        }

        // wykonanie ticku
        run(selected_job->job, 1);           
        selected_job->job->burst -= 1; // remaming time sie zmniejsza
        current_time += 1;          

        // zakonczenie zadania jesli skonczone
        if (selected_job->job->burst == 0) {
            // metryki
            int turnAroundTime = current_time - selected_job->job->start_time;
            int waitingTime  = turnAroundTime - initial_burst[id];

            total_wt  += waitingTime;            
            total_tat += turnAroundTime;
            total_rt  += resp_time[id];

            // Usuwanie wezla
            if (selected_previous)
                selected_previous->next = selected_job->next;
            else
                stcf_job_list = selected_job->next;

            // Czyszczenie pamięci
            free(selected_job->job->job_name);
            free(selected_job->job);
            free(selected_job);

            jobs_left--;  
        }
    }

    printf("\nSTCF Scheduling Metrics:\n");
    printf("Avg waiting time    = %.2f\n", total_wt  / total_jobs); // łączny czas oczekiwania w kolejce na wykonanie
    printf("Avg turnaround time = %.2f\n", total_tat / total_jobs); // czas przetwarzania
    printf("Avg response time   = %.2f\n", total_rt  / total_jobs); // czas reakcji (ile sekund po przyjściu wykonany)

    free(initial_burst);
    free(resp_time);
    free(comp_time);
    free(responded);
}
