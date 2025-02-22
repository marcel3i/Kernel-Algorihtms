#include <stdlib.h>
#include "list.h"

// Insert a job at the beginning of the list.
void insertNode(Node **head, Job *job) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->job = job;
    newNode->next = *head;
    *head = newNode;
}

// Free the entire list (and the job resources).
void freeList(Node *head) {
    while (head) {
        Node *temp = head;
        head = head->next;
        free(temp->job->job_name);
        free(temp->job);
        free(temp);
    }
}
