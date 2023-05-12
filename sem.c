#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"



typedef struct semaphore {
	/*TODO Phase 3 */
    int count;  //semaphore internal count
    queue_t semaphore_queue;  //waiting threads
}semaphore;

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	// Allocate memory for the semaphore
    struct semaphore *init_semaphore = malloc(sizeof(struct semaphore));
    if (init_semaphore == NULL) 
    {
        return NULL;
    }
    else 
    {
        // Initialize semaphore count
        init_semaphore->count = count;

        // Set up an empty waiting queue
        init_semaphore->semaphore_queue = queue_create();
        if (init_semaphore->semaphore_queue == NULL) {
            free(init_semaphore);
            return NULL;
    }
    }

    return init_semaphore;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL || (queue_length(sem->semaphore_queue) > 0))
	{
         return -1;
       }
	else
	{
		// Free waiting queue
    	queue_destroy(sem->semaphore_queue);
    	// Free semaphore 
   		free(sem);
    	return 0;
	}
}

int sem_down(sem_t sem)
{
    /*TODO Phase 3*/
    /*if the semaphore is NULL */
    if (sem == NULL)
    {
        return -1;
    }

    /* avoid race condition by disable preemption */
    preempt_disable();

    /* decrement the count and proceed if the semaphore count is more than 0 */
    if (sem->count > 0)
    {
        sem->count--;
    }
    else
    {
        /* else add thread to the waiting queue and block it*/
        /* Enable preemption */
        preempt_enable();
        queue_enqueue(sem->semaphore_queue, uthread_current());
        uthread_block();
        preempt_disable();
    }

    

    return 0;
}


int sem_up(sem_t sem)
{
    /*TODO Phase 3*/
    /* Check if the semaphore is NULL */
    if (sem == NULL)
    {
        return -1;
    }
    else 
    {
            /* avoid race condition by disable preemption */
        preempt_disable();

        /* Increment the semaphore count */
        sem->count++;

        /* If threads in the waiting queue, unblock the oldest */
        if (queue_length(sem->semaphore_queue) > 0)
        {
            /* Dequeue the oldest thread from the waiting queue */
            struct uthread_tcb *first_thread;
            queue_dequeue(sem->semaphore_queue, (void **)&first_thread);

            /* unblock the dequeued thread */
            preempt_enable();
            uthread_unblock(first_thread);
            preempt_disable();
        }
    }


    return 0;
}


