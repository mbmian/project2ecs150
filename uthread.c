#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "preempt.c"
#include <string.h>
#include "private.h"
#include "uthread.h"
#include "queue.h"

//define what different states are equal to so we could use in code later
#define RUNNING_Thread 0
#define READY_Thread 1
#define EXITED_Thread -1
#define BLOCKED_Thread -2



typedef struct uthread_tcb {
    uthread_ctx_t context;       /* thread context to be stored here */
    int state;        /* thread state to be stored here */
    void *stack;                 /* start of the stack pointer*/
    uthread_func_t function;     /* thread will run this function*/
    void *arg;                   /* the function would be passed this argument */
} uthread_tcb_type;

// queue_t ready_queue;
uthread_tcb_type *current_thread = NULL;
uthread_tcb_type *idle_thread = NULL;  // global idle thread
queue_t ready_queue;

void create_ready_queue() {
    ready_queue = queue_create();
    if (ready_queue == NULL) {
        fprintf(stderr, "Error: Unable to start ready queue\n");
        exit(EXIT_FAILURE);
    }
}



struct uthread_tcb *uthread_current(void)
{
    /* TODO Phase 2/3 */
    return current_thread;
}


void uthread_yield(void)
{
    uthread_tcb_type *prev_thread = current_thread;

    if (current_thread != NULL) {
        if (current_thread->state != EXITED_Thread)
        {
            current_thread->state = READY_Thread;
            queue_enqueue(ready_queue, current_thread);
        }
    }

    uthread_tcb_type *next_thread;
    queue_dequeue(ready_queue, (void **)&next_thread);

    if (next_thread != NULL) {
        next_thread->state = RUNNING_Thread;
    } else {
        fprintf(stderr, "Error: Failed to get the next thread from the ready queue.\n");
        exit(EXIT_FAILURE);
    }

    current_thread = next_thread;
    if (prev_thread != NULL)
    {
        uthread_ctx_switch(&prev_thread->context, &next_thread->context);
    }
}

void destroy_stack_and_yield() {
    uthread_ctx_destroy_stack(current_thread->stack);
    
    if (queue_length(ready_queue) > 0) {
        uthread_yield();
    } else {
        fprintf(stderr, "Error: No more threads available for execution.\n");
        exit(0);
    }
}

void uthread_exit(void)
{
    uthread_tcb_type *current_thread = uthread_current();
    // void *prevStack = current_thread->stack;

    current_thread->state = EXITED_Thread;

   destroy_stack_and_yield();
}


int uthread_create(uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
    preempt_disable();
    // malloc to new thread
    uthread_tcb_type *new_thread = malloc(sizeof(struct uthread_tcb));
    if (new_thread==NULL) {
        preempt_enable();
        return -1;
    }

    // set up/allocate new threads stack
    new_thread->stack = uthread_ctx_alloc_stack();
    if (new_thread->stack == NULL) {
        free(new_thread);
        return -1;
    }


    // Initialize the new thread's execution context
    int init_result = uthread_ctx_init(&(new_thread->context), new_thread->stack, func, arg);
    if (init_result == -1) {//context is thread stack,state,context
        uthread_ctx_destroy_stack(new_thread->stack);
        free(new_thread);
        return -1;
    }

    // new thread is now ready
    new_thread->state = READY_Thread;

    // Add the new thread to the ready queue or other appropriate data structure
    queue_enqueue(ready_queue, new_thread);
    preempt_enable(); // Re-enable preemption after manipulating shared data structures


    return 0;
}

bool check_and_yield() {
    bool continue_loop = true;
    while (continue_loop) {
        preempt_disable();
        if (queue_length(ready_queue) > 0) {
            uthread_yield();
        } else {
            continue_loop = false;
        }
        preempt_enable();
    }
    return true;
}


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    create_ready_queue();

    // Initialize preemption
    preempt_start(preempt);

    // Allocating space for the idle thread
    uthread_tcb_type *idle_thread = malloc(sizeof(uthread_tcb_type));
    if (idle_thread == NULL) {
        return -1;
    }

    // Initiate idle_thread's stack and context
    void* idle_stack = uthread_ctx_alloc_stack();
    if (idle_stack == NULL) {
        free(idle_thread);
        return -1;
    }

    idle_thread->stack = idle_stack;


    // Initialize the idle thread's context but leave its function as NULL
    int init_result = uthread_ctx_init(&(idle_thread->context), idle_thread->stack, NULL, NULL);
    if (init_result == -1) {
        uthread_ctx_destroy_stack(idle_thread->stack);
        free(idle_thread);
        return -1;
    }


    idle_thread->state = RUNNING_Thread;  // Set idle thread state as running
    idle_thread->function = NULL;         // Idle thread has no function
    idle_thread->arg = NULL;              // No arguments as well

    // Now, create the initial thread
    preempt_disable();
    preempt_enable();

    if (uthread_create(func, arg) == -1) {
        return -1;
    }


    current_thread=idle_thread;
    // Execute an infinite loop

    bool loop_result = check_and_yield();


    // Stop preemption
    preempt_stop();
    // Destroy the ready queue
    queue_destroy(ready_queue);
    // If there are no more threads which are ready to run, stop the idle loop
    return loop_result ? 0 : -1;
}



void uthread_block(void)
{
    /* TODO Phase 3 */
    uthread_tcb_type *current_thread = uthread_current();
    current_thread->state = BLOCKED_Thread;
    uthread_yield(); // yield to another thread
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    /* TODO Phase 3 */
    uthread->state = READY_Thread;
    queue_enqueue(ready_queue, uthread);
    uthread_yield(); // yield the processor immediately
}



