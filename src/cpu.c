#include <stdio.h>
#include "cpu.h"

void run(Job *job, int slice) {
    printf("Running job: [%s] (ID: %d) [Priority: %d, Burst: %d] for %d time units.\n",
           job->job_name, job->job_id, job->priority, job->burst, slice);
}
