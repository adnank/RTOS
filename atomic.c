/*
 * atomic.c
 *
 *  Created on: 2010-11-28
 *      Author: Joel J
 */


#include "rtx.h"



int change;

void atomic(change)
{
    static sigset_t x;
    static sigset_t y;

    if (change == ON)
    {
        //Need to create an atomic count variable to keep track of if a process has atomic(on)twice.
        //if (atomic_count == 0){
        sigemptyset (&x);
        sigaddset(&x, SIGINT);
        sigaddset(&x, SIGALRM);
        //sigaddset(&x, SIGUSR1);
        //sigaddset(&x, SIGUSR2);
        sigprocmask(SIG_BLOCK, &x, NULL/*&y*/);

    }
    else
    {

        sigprocmask(SIG_UNBLOCK, &x, NULL/*&y*/);

    }
}
