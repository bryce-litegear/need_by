#include "need_by.h"
#include <stdbool.h>

uint32_t time_now;

void need_by_tick(void)
{
    time_now++;
}



static uint32_t get_now(void)
{
    return time_now;
}

uint32_t future_time(uint32_t uSecs)
{
    return get_now() + uSecs;
}

static bool is_time(uint32_t tval)
{
    return tval - get_now() >= 0x80000000;
}

// try to run top of specified queue
static bool try_to_run(need_by_queue *nbq)
{
    while(nbq->next)
    {
        need_by_entry *active = nbq->next;
        nbq->next = nbq->next->next;    // pop from queue
        need_by_task task = active->task;
        void *context = active->context;
        active->next = NULL;    // release the entry
        if(task)    // if task run it
        {
            task(context);
            return true;
        }
        // if no task, just entry, try again
    }
    return false;
}

// find and run the first timed out task, if any
static bool try_need_by(need_by_obj *nb)
{
    for(need_by_priority pri = NB_PRIORITY_1; pri < NB_NO_RUSH; pri ++)
    {
        while(nb->queues[pri].next)
        {
            if(is_time(nb->queues[pri].next->need_by_time))
            {
                if(try_to_run(&nb->queues[pri])) return true;
            }
            else
            {
                break;
            }
        }
    }
    return false; // no tasks run because of time outs
}

// try to run one thing from the prioritized part of the queues
static bool try_next_pri(need_by_obj *nb)
{
    for(need_by_priority pri = NB_PRIORITY_1; pri < NB_NO_RUSH; pri ++)
    {
        while(nb->queues[pri].next) // something in this queue
        {
            // try to run and return if it does
            if( try_to_run(&nb->queues[pri]) ) return true;
        }
    }
    return false; // no tasks run because of time outs    
}

void do_need_by_tasks(need_by_obj *nb)
{
    bool ran_task = true;
    while(ran_task)
    {
        ran_task = false;

        if( (ran_task = try_to_run(&nb->queues[NB_ASAP])) ) continue;
        // no ASAP, look for timed_out
        if( (ran_task = try_need_by(nb)) ) continue;

        if( (ran_task = try_next_pri(nb)) ) continue;               

        if( (ran_task = try_to_run(&nb->queues[NB_NO_RUSH])) ) break;
    }
}

int queue_need_by(need_by_task task, void *context, uint32_t limit_uSec, int8_t priority)
{
    
}