#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "private.h"
#include "uthread.h"
/*self notes: SIGVTALRM used to implement preemptive scheduling for your user-level threads.2-*/
/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
//to store the original signal action and timer configuration when preempt_stop is called
static struct sigaction initial_sa;
static struct itimerval initial_timer;
static bool preemption_enabled = true;

#define HZ 100
void timer_handler(int sig)
        {
            (void) sig; // Suppress unused parameter warning
            uthread_yield();
        }
// help taken from mhttps://stackoverflow.com/questions/50195942/sigprocmask-with-sig-block-doesnt-block-sigalrm
//slide 41
void preempt_disable(void)
{
	/* TODO Phase 4 */
	if (preemption_enabled==true)
    {
        preemption_enabled = false;

        // if preemption disabled Block SIGVTALRM
        sigset_t sig_mask;
		//https://www.gnu.org/software/libc/manual/html_node/Process-Signal-Mask.html
        sigemptyset(&sig_mask);
        sigaddset(&sig_mask, SIGVTALRM);
        sigprocmask(SIG_BLOCK, &sig_mask, NULL);
    }
}
//SLIDE 41 OF SYSTEM CALLS
void preempt_enable(void)
{
	/* TODO Phase 4 */
	 if (!preemption_enabled)
    {
        preemption_enabled = true;

        // Unblock SIGVTALRM when preemption is enabled
        sigset_t sig_mask;
        sigemptyset(&sig_mask);
        sigaddset(&sig_mask, SIGVTALRM);
        sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
    }
}
//https://stackoverflow.com/questions/48064940/understanding-struct-itimerval-field-tv-usec
void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
	/*self notee:SIGVTALRM, which is timer_interrupt_handler(int sig), is executed. 
	This handler calls uthread_yield() that helps to switch current running thread with next thread in ready queue.*/
   if (preempt)
    {
        // Signal handler implementation
        

        // Set up the signal handler for SIGVTALRM
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));//ensure SA struct in clean state
        sa.sa_handler = timer_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGVTALRM, &sa, &initial_sa) == -1) // Save the old signal action
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        // Configure the timer to fire at 100 Hz
        //https://www.ibm.com/docs/en/i/7.2?topic=ssw_ibm_i_72/apis/setitime.html
        //https://stackoverflow.com/questions/48064940/understanding-struct-itimerval-field-tv-usec
        struct itimerval timer_config;
        timer_config.it_value.tv_sec = 0;
        timer_config.it_value.tv_usec = 10000; // 10000 microseconds = 1/100 second
        timer_config.it_interval.tv_sec = 0;
        timer_config.it_interval.tv_usec = 10000;
        if (setitimer(ITIMER_VIRTUAL, &timer_config, &initial_timer) == -1) // Save the old timer configuration
        {
            perror("setitimer");
            exit(EXIT_FAILURE);
        }
    }
}



void preempt_stop(void)
{
	/* TODO Phase 4 */
	// Restore the previous signal action for SIGVTALRM
    if (sigaction(SIGVTALRM, &initial_sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Restore the previous timer configuration
    if (setitimer(ITIMER_VIRTUAL, &initial_timer, NULL) == -1)
    {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }
}


