#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3_queue.h"

char* queue_front(Queue **q)
{
        if (*q == NULL)
                return NULL;
        
        // Store the head and data of the head of the queue
        Queue *first = *q;
        char *data = first->data;

        // Set the head of the queue to the second in the queue
        (*q) = (*q)->next;
        // Remove the head of the queue from the memory
        free(first);

        return data;
}

void queue_enqueue(Queue **q , char *data)
{
        // Create a new node for the queue
        Queue *newNode = (Queue*) malloc(sizeof(Queue));
        Queue *last = *q;

        // Initialise the new node
        newNode->data = data;
        newNode->next = NULL;

        // If the queue is empty, set the new node as the head
        if (*q == NULL)
        {
                *q = newNode;
                return;
        }

        // Iterate through the list until the last node in the queue
        while (last->next != NULL)
                last = last->next;
        
        // Set the next for the last node as the new node
        last->next = newNode;
}

void queue_freeQueue(Queue **q)
{
        if (*q == NULL)
                return;

        // Store the head of the queue
        Queue *current = *q;
        Queue *next;
        while (current != NULL)
        {
                // Remove every node from the queue from the memory
                next = current->next;
                free(current);
                current = next;
        }

        q = NULL;
}