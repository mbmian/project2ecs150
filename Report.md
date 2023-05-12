##Project Description
In this project we devloped a a program that helps implements a user-level thread library using C.
We implemented a number of functions that make this possible such create, destroy,run and yield control to threads. With the help of timer interupts preemption was implemented while threads are scheduled using a round-robin algorithm.

##High-level Design Choices
I made two data structures in queue.c, the reason being is that one was needed to track the queue while the other was needed to track the data in the queue. In the preemption phase is used `time_interupt_handler` that would just yield to the next thread once it had run for a specific amount of time (0.01s). Next With the help of SIGVTALRM signal we were able to interrupt threads and switch to the next thread in the ready queue. Linked list of blocked threads helped implement semaphores that were used for synchronization and the hardest part was to make sure. these semaphores semaphore has a counter and a list of blocked threads. THe hard part was making sure race condition was prevented therefore we had to use `preempt_enable()` and `preempt_disable()` that were used when these semaphores were accessed. The final part involves the main task of this code which was threading which turned out to be the most challenging for us. The hardest part was tracking the states, threades, and memory allocation. Therefore, we used an infinite loop which had the job of only calling `uthread_yield` and `uthread_yield` would then take of the current thread and context switching between the next and current thread. This made the code easier to understand and implement.

##Relevant Parts of Implementation
`uthread.c`
This part of the project implements the user-level threading library. The uthread_create function helps the in creating new threads and takes care of initializing them such as memory allocation and stack parts of the thread. The uhtread_run function helps in running the desired number of threads and calls the uthread_yield function. Now the uthread_yield function yields helps in transfering form the current thread to the next thread and helping in placing the current thread into the ready queue. Finnaly the uthread_exit function is called when the current thread needs to be terminated and removed from the ready queue.

One of the challenging parts of implementing this part was the `uthread_yield` function. The uthread_yield function had a sole function of context switiching when the next thread needed to be executed. However, this was made really challanging when our code would not give the desired output for the `uthread_hello` tester. We had to go through the code and understand exactly what was happening at each stage. Therefore, we decided to call `uthread_yield` from `uthread_run` in a while loop which would break as soon as the ready_queue was empty. We also had to make sure `uthread_ctx_switch` was being passed the right elements. Once we fixed these issue our `uthread_yield` functioned as it should.

`preempt.c`
The preempt.c file implements preemption using a timer interrupt. We start with the `preempt_start` function that generates a timer interrupt at a frequency of 100 Hz. The signal handler calls the uthread_yield() function when there is a timer interupt so the next thread in ready queue could execute.

One of the challenging parts of the implementation was disabling, enabling preemption and also dealing with signals. Therefore, we had to go through the lecture slides multiple times to modify the code present there. In general, sigprocmask function was used to block and unblock the SIGVTALRM signal, the SIGVTALRM signal gets blocked When preemption is disabled, and vise versa for when it is enabled.

`The sem.c`
One of the key issues that we learned in this course was race conditions and this part tackles axactly that. Semaphores, which are used for thread synchronization. First we need to create a queue for the semaphores and count the number of resources that are available and The sem_create function does exactly that. Now in order to de-allocate memory the sem_destroy function is called the sempahore is destryoed and any memory attached to it is freed. Now if there are no resources available the count would be zero and the sem_down function is called to decrements the count of a semaphore and blocks the calling thread. While the sem_up function increments the count of a semaphore and goes into the queue so the first thread in the queue in unblocked.

One of the challenging parts of the implementation was to prevent a race condition in the case that two threads try to access the same semaphore. To tackle this problem we used the preempt_disable and preempt_enable functions defined in preempt.c to disable and enable preemption.

`The queue.c`
This file helps define a simple queue data structure that would be used to store threads or semaphores on other parts of the project. The queue_create function takes care of first setting up a queue and initializing its head and tail pointers. The queue_destroy function helps in de-allocating memory for the queue and its nodes. The queue_enqueue function helps in incrementing a queue by adding a new node to the end of a queue. The queue_dequeue function helps in rmeoving the node from the queue.

The most challanging part for us was understanding the need for 2 data structures. Initially we thought the code could not have more functions the already given to us, but after claerification and discussing the issue with our professor we realized how 2 data structure would help track the whole queue and the othe would track the data in the queue.

##Testing

`queue.c`
In order to test this part we had to modify the given tester file. Initially queue and assert are created and made sure they are not NULL. Then the test series involves Enqueuing and dequeuing a single element, Enqueuing multiple elements, dequeuing them, and making sure they return the values that are expected. Then elements are enqueued, and then delete a specific value, this helps make sure the queue_length function works. Then the same concept is applied but with multiiple elements. Hence, our tester makes sure all functions of `queue.c` is working and if they are returning the expected values even if the testing is scaled.

`uthread.c`
For this part we mostly utilized `uthread_hello.c` and `uthread_yield.c` that were provided. In short these programs tested the functionality of one thread and then 3 threads to ensure if user-level multi-threading was implemented properly.

`sem.c`
For this part we mostly utilized `sem_simple.c`, `sem_prime.c`, `sem_count.c` and `sem_buffer.c` that were provided. Each one of these files ensured the correct implementation of semaphores in our code. `sem_simple` uses semaphores over 3 threads, `sem_count` uses multiple threads to count, `sem_buffer` uses a great number of write operations to the buffer and made sure there was no race conditions.

`preempt.c`
For this part we did not have enough time to complete the testing part. However, the idea was to have a while loop and have multiple threads running then use preempt to make sure they are yielding to the CPU.

##Sources
We tried our best to record as many resources that we used for this project, there we some websites that were used for understanding concepts which are included below but some may be missing due to the length and complexity of this project.

1-mhttps://stackoverflow.com/questions/50195942/sigprocmask-with-sig-block-doesnt-block-sigalrm for preempt.c
2-Slide 42 of system calls for preempt.c
3-//https://www.gnu.org/software/libc/manual/html_node/Process-Signal-Mask.html for preempt.c
4-https://www.geeksforgeeks.org/introduction-and-array-implementation-of-queue/# for queue.c
5-slides of process scheudeling
6-https://www.youtube.com/watch?v=FcIubL92gaI for queue.c
7-https://stackoverflow.com/questions/36094115/c-low-level-semaphore-implementation for sem.c
8-https://greenteapress.com/thinkos/html/thinkos013.html for sem.c
9-slide 21 onwards on synchronization for sem.c
10-https://www.schaertl.me/posts/a-bare-bones-user-level-thread-library/ for uthread.c
11-https://www.geeksforgeeks.org/implement-threads-in-user-space/ for uthread.c
12-https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-pthread-yield-release-processor-other-threads
13-https://www.gnu.org/software/libc/manual/2.36/html_mono/libc.html#Blocking-Signals
14https://www.gnu.org/software/libc/manual/2.36/html_mono/libc.html#Signal-Actions
15-https://www.gnu.org/software/libc/manual/2.36/html_mono/libc.html#Setting-an-Alarm
16-https://www.gnu.org/software/libc/manual/2.36/html_mono/libc.html#System-V-contexts
17-http://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html
18-https://stackoverflow.com/questions/48064940/understanding-struct-itimerval-field-tv-usec for preempt.c
19-The professor and TA's were asked for guidance several times.
