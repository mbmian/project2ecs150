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
    queue_t q;
    size_t i;

    fprintf(stderr, "*** TEST queue_multiple ***\n");

    q = queue_create();
    TEST_ASSERT(q != NULL);

    size_t data_length = sizeof(data) / sizeof(data[0]);
    for (i = 0; i < data_length; i++) {
        queue_enqueue(q, &data[i]);
    }

    int *ptr = NULL;
    for (i = 0; i < data_length; i++) {
        int dequeue_result = queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(dequeue_result == 0);
        TEST_ASSERT(ptr != NULL);
        TEST_ASSERT(*ptr == data[i]);
    }
}

/* Test queue_delete */
void test_queue_delete(void) {
    int data[] = {1, 2, 3, 4, 5};
    queue_t q;
    size_t i;

    fprintf(stderr, "*** TEST queue_delete ***\n");

    q = queue_create();
    TEST_ASSERT(q != NULL);

    size_t data_length = sizeof(data) / sizeof(data[0]);
    for (i = 0; i < data_length; i++) {
        queue_enqueue(q, &data[i]);
    }

    int set_length = 4;
    int *delete_pointer = &data[2];
    TEST_ASSERT(queue_delete(q, delete_pointer) == 0); // Delete the value '3'
    TEST_ASSERT(queue_length(q) == set_length);
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
    TEST_ASSERT(q != NULL);

    size_t data_length = sizeof(data) / sizeof(data[0]);
    for (i = 0; i < data_length; i++) {
        int queue_item = data[i];
        int enqueue_result = queue_enqueue(q, &queue_item);
        TEST_ASSERT(enqueue_result == 0);
        int expected_length = (int)(i + 1), actual_length = queue_length(q);
        printf("Enqueueing:\n");
        printf("  Expected length: %d\n", expected_length);
        printf("  Actual length: %d\n", actual_length);
        TEST_ASSERT(actual_length == expected_length);
    }

    for (i = 0; i < data_length; i++) {
        void *dequeued_data;
        queue_dequeue(q, &dequeued_data);
        int expected_length = (int)(data_length - i - 1), actual_length = queue_length(q);
        printf("Dequeueing:\n");
        printf("  Expected length: %d\n", expected_length);
        printf("  Actual length: %d\n", actual_length);
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
