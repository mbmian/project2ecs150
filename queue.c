#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct node {
    void *data;
    struct node *next;
} Node;

struct queue {
    Node *first;
    Node *last;
};

queue_t queue_create(void)
{
    queue_t queue = malloc(sizeof(struct queue));
    if (queue == NULL) {
        return NULL;
    }
    *queue = (struct queue) {
        .first = NULL, .last = NULL
    };
    return queue;
}

int queue_destroy(queue_t queue)
{
    if (queue == NULL || queue_length(queue) > 0) {
        return -1;
    }
    free(queue->first);
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL) {
        return -1;
    }
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        return -1;
    }
    node->data = data;
    node->next = NULL;
    if (queue->first == NULL) {
        queue->first = node;
        queue->last = node;
    } else {
        queue->last->next = node;
        queue->last = node;
    }
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (queue == NULL || data == NULL || (queue_length(queue) == 0)) {
        return -1;
    }

   
    Node *node = queue->first;
    if (node == NULL) {
        return -1;
    }
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
    Node *prev = NULL;
    Node *curr = queue->first;
    while (curr != NULL) {
        if (curr->data == data) {
            break;
        }
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
    if (queue == NULL) {
        return -1;
    }

    if (func == NULL) {
        return -1;
    }
    Node *curr = queue->first;
    while (curr != NULL) {
        Node *next = curr->next;
        if (func != NULL) {
            func(queue, curr->data);
        }
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
    Node *curr = NULL;
    if (queue != NULL) {
        curr = queue->first;
    }
    while (curr != NULL) {
        length = length + 1;
        if (curr != NULL) {
            curr = curr->next;
        }
    }
    return length;
}
