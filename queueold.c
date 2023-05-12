#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
    void *data;
    struct node *next;
};

struct queue {
    struct node *first;
    struct node *last;
};

queue_t queue_create(void)
{
    queue_t queue = malloc(sizeof(struct queue));
    if (queue == NULL) {
        return NULL;
    }
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

int queue_destroy(queue_t queue)
{
    if (queue == NULL || queue->first != NULL) {
        return -1;
    }
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL) {
        return -1;
    }
    struct node *node = malloc(sizeof(struct node));
    if (node == NULL) {
        return -1;
    }
    node->data = data;
    node->next = NULL;
    if (queue->last == NULL) {
        queue->first = node;
    } else {
        queue->last->next = node;
    }
    queue->last = node;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (queue == NULL || data == NULL || queue->first == NULL) {
        return -1;
    }
    struct node *node = queue->first;
    *data = node->data;
    queue->first = node->next;
    if (queue->first == NULL) {
        queue->last = NULL;
    }
    free(node);
    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL) {
        return -1;
    }
    struct node *prev = NULL;
    struct node *curr = queue->first;
    while (curr != NULL && curr->data != data) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        return -1;
    }
    if (prev == NULL) {
        queue->first = curr->next;
    } else {
        prev->next = curr->next;
    }
    if (queue->last == curr) {
        queue->last = prev;
    }
    free(curr);
    return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
    if (queue == NULL || func == NULL) {
        return -1;
    }
    struct node *curr = queue->first;
    while (curr != NULL) {
        struct node *next = curr->next;
        func(queue, curr->data);
        curr = next;
    }
    return 0;
}

int queue_length(queue_t queue)
{
    if (queue == NULL) {
        return -1;
    }
    int length = 0;
    struct node *curr = queue->first;
    while (curr != NULL) {
        length++;
        curr = curr->next;
    }
    return length;
}
