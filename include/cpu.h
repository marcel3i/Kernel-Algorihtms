#ifndef CPU_H
#define CPU_H

#include "job.h"

// Simulated CPU: run the given job for a specified time slice.
void run(Job *job, int slice);

#endif
