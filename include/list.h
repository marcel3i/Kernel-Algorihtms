#ifndef LIST_H
#define LIST_H

#include "job.h"

// Node structure for a linked list of jobs.
typedef struct node {
    Job *job;
    struct node *next;
} Node;

#endif
