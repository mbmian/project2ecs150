#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void iterator_inc(struct queue *q, void *data);


#define TEST_ASSERT(assert) do { \
    printf("ASSERT: " #assert " ... "); \
    if (assert) { \
        printf("PASS\n"); \
    } else { \
        printf("FAIL\n"); \
        exit(1); \
    } \
} while (0)

/* Create */
void test_create(void) {
    fprintf(stderr, "*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
    int data = 3, *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_simple ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void **) &ptr);
    TEST_ASSERT(ptr == &data);
}

/* Enqueue/Dequeue multiple */
void test_queue_multiple(void) {
    int data[] = {1, 2, 3, 4, 5};
    int *ptr;
    queue_t q;
    size_t i;

    fprintf(stderr, "*** TEST queue_multiple ***\n");

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        queue_enqueue(q, &data[i]);
    }

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        queue_dequeue(q, (void **) &ptr);
        TEST_ASSERT(ptr == &data[i]);
    }
}

/* Test queue_delete */
void test_queue_delete(void) {
    int data[] = {1, 2, 3, 4, 5};
    queue_t q;
    size_t i;

    fprintf(stderr, "*** TEST queue_delete ***\n");

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        queue_enqueue(q, &data[i]);
    }

    TEST_ASSERT(queue_delete(q, &data[2]) == 0); // Delete the value '3'
    TEST_ASSERT(queue_length(q) == 4);
}

/* Test queue_iterate */
void test_iterator(void) {
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}


void iterator_inc(queue_t q, void *data) {
    int *a = (int *)data;
    (*a)++;
    printf("%d ", *a);

    // Delete the item with the value '42'
    if (*a == 43) {
        queue_delete(q, a);
    }
}



void test_queue_length(void) {
    fprintf(stderr, "*** TEST queue_length ***\n");

    int data[] = {1, 2, 3, 4, 5};
    queue_t q;
    size_t i;

    q = queue_create();

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        queue_enqueue(q, &data[i]);
        int expected_length = (int)(i + 1);
        int actual_length = queue_length(q);
        printf("Enqueue: Expected length: %d, Actual length: %d\n", expected_length, actual_length);
        TEST_ASSERT(actual_length == expected_length);
    }

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        void *dequeued_data;
        queue_dequeue(q, &dequeued_data);
        int expected_length = (int)(sizeof(data) / sizeof(data[0]) - i - 1);
        int actual_length = queue_length(q);
        printf("Dequeue: Expected length: %d, Actual length: %d\n", expected_length, actual_length);
        TEST_ASSERT(actual_length == expected_length);
    }
}



int main(void) {
    test_create();
    test_queue_simple();
    test_queue_multiple();
    test_queue_delete();
    test_queue_length();
    test_iterator();
   

    return 0;
}
