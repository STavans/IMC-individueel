#ifndef MP3_Q
#define MP3_Q

#include <stdio.h>
#include <stdlib.h>

// A single item in a queue (implemented with linkedList principle)
typedef struct queue
{
        char *data;
        struct queue *next;
} Queue;

/*
Adds an item to the end of the queue

-Parameters: A pointer to the head of the queue, the data for the new item
*/
void queue_enqueue(Queue**, char*);

/*
Removes the first item (and from the memory) from the queue and returns it data

-Returns: The data of the first item in the queue
-Paramters: A pointer to the head of the queue
*/
char* queue_front(Queue**);

/*
Deletes the whole queue from the memory and sets the head to NULL

-Parameters: A pointer to the head of the queue
*/
void queue_freeQueue(Queue**);

#endif