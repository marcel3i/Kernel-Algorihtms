#ifndef JOB_H
#define JOB_H

#include <stdlib.h>
#include <string.h>

// Representation of a job in the scheduling system.
typedef struct job {
    char *job_name;    // Renamed from "name"
    int job_id;        // Renamed from "tid"
    int priority;
    int burst;
    int start_time;    // Time when the job becomes available
    int deadline;      // Deadline for job completion (-1 if not applicable)
} Job;

#endif
