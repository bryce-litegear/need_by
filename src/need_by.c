#include "need_by.h"
#include <stdbool.h>


// using 64 bit timer to simplify math an ignore roll over issues
STATIC uint64_t time_now = 1; // microsec since power up (~ 584,942 years)
STATIC uint32_t uSec_per_tick = 1000; // default is msec tick rate 1000 uSec per mSec

typedef enum need_by_return
{
    NB_Q_CLEAR,     // queue is empty
    NB_TASK_RAN,    // some task ran, so re cycle
    NB_CONTINUE,    // reached end of stack
    NB_Q_SCANNED,   // The queue was scanned for timeouts
} need_by_return;

// called by some source of time at fixed rate
void need_by_tick(void)
{
    time_now += uSec_per_tick;
}

// for testing
uint64_t get_nb_now(void) 
{
    return time_now;
}

uint64_t future_time(uint64_t uSecs)
{
    return time_now + uSecs;
}

static inline bool is_past_time(uint64_t tval)
{
    return tval && tval <= time_now;
}

static void nb_dequeue(need_by_queue *nbq, need_by_entry *entry)
{
    if(nbq && entry)
    {
        if(nbq->next == entry)
        {
            nbq->next = entry->next;
        }
        else
        {
            need_by_entry *from = nbq->next;
            need_by_entry *next = nbq->next->next;
            // find entry
            while(next && entry != next)
            {
                from = next;
                next = next->next;
            }
            if(next) from->next = entry->next; 
     
        }
        if(nbq->last == entry) nbq->last = NULL;
        entry->need_by_time = 0;  // free the handle
        entry->next = entry;      // free the handle 
    }
}


// try to run top of specified queue
static need_by_return try_to_run(need_by_queue *nbq)
{
    if(nbq->active_task) return NB_CONTINUE; // already task running (coop recursion)
    while(nbq->next) // run first defined task, clearing any non-tasks along the way
    {
        // deque next
        nbq->active_task = nbq->next;
        // grab local copy
        need_by_task task = nbq->active_task->task;
        void *context = nbq->active_task->context;
        // release queue entry
        nb_dequeue(nbq, nbq->active_task);
        // now try to run
        if(task)
        {
            task(context);
            nbq->active_task = NULL; // signal task return
            return NB_TASK_RAN; 
        }
        // if no task, just entry, try again
        nbq->active_task = NULL;
    }
    return NB_Q_CLEAR; // no runnable tasks in the queue
}

static need_by_return run_first_timedout(need_by_queue *nbq)
{
    if(nbq->to_task) return NB_CONTINUE; // already running to task
    if(nbq->next) // items are in the queue
    {
#ifdef TEST
        need_by_tick();
#endif        
        nbq->to_task = nbq->next;
        while(nbq->to_task)
        {
            if(is_past_time(nbq->to_task->need_by_time))
            {
                need_by_task task = nbq->to_task->task;
                void *context = nbq->to_task->context;    
                need_by_entry *temp = nbq->to_task->next;           
                nb_dequeue(nbq, nbq->to_task);
                if(task)
                {
                    task(context);
                    nbq->to_task = NULL;
                    return NB_TASK_RAN;
                }
                // if no task keep walking the queue 
                nbq->to_task = temp;
                continue;
            }
            nbq->to_task = nbq->to_task->next;
        }
    }
    return NB_Q_SCANNED;
}


// find and run the first timed out task, if any
static need_by_return try_need_by(need_by_obj *nb)
{
    for(need_by_priority pri = NB_PRIORITY_1; pri < NB_NO_RUSH; pri++)
    {
        need_by_return ret = run_first_timedout(&nb->queues[pri]);
        if(ret != NB_Q_SCANNED) return ret;
    }
    return NB_Q_SCANNED; // no tasks run because of time outs
}

// try to run one thing from the prioritized part of the queues
static need_by_return try_next_pri(need_by_obj *nb)
{
    if(nb->busy_queue)
    {
        for(need_by_queue *q=&nb->queues[NB_PRIORITY_1]; 
            q < nb->busy_queue; q++)
        {
            while(q->next) // something in this queue
            {
                // try to run and return if it does
                if( try_to_run(q) ) return NB_TASK_RAN;
            }
        }
        return NB_CONTINUE; // can't do any more
    }
    else
    {
        for(nb->busy_queue = &nb->queues[NB_PRIORITY_1]; 
            nb->busy_queue < &nb->queues[NB_NO_RUSH]; 
            nb->busy_queue++ )
        {
            while(nb->busy_queue->next) // something in this queue
            {
                // try to run and return if it does
                if( try_to_run(nb->busy_queue) ) return NB_TASK_RAN;
            }
        }
        nb->busy_queue = NULL;
        return NB_Q_CLEAR; // no tasks run because of time outs    
    }
}

void do_need_by_tasks(need_by_obj *nb)
{
    // first entry, no active priority
    if(NULL == nb->active_queue)
    {
        need_by_return ret = NB_CONTINUE;
        while(ret != NB_Q_CLEAR)
        {
#ifdef TEST
            need_by_tick();
#endif
            nb->active_priority = NB_ASAP;
            nb->active_queue = &nb->queues[nb->active_priority];
            if( NB_TASK_RAN == (ret = try_to_run(nb->active_queue)) ) continue;
            // do any special timeout stuff
            if( NB_TASK_RAN == ( ret = try_need_by(nb) ) ) continue;
            // do prioritized queues
            if( NB_TASK_RAN == ( ret = try_next_pri(nb)) ) continue;               
            nb->active_priority = NB_NO_RUSH;
            nb->active_queue = &nb->queues[nb->active_priority];
            if( NB_TASK_RAN == (ret = try_to_run(nb->active_queue)) ) continue;
            nb->active_queue = NULL;
        }
    }
    else // function was re-entered 
    {
        need_by_return ret = NB_CONTINUE;
        while(ret != NB_Q_CLEAR)
        {
#ifdef TEST
            need_by_tick();
#endif            
            if(NB_CONTINUE == (ret = try_to_run(&nb->queues[NB_ASAP])) ) return; // already running highest queue
            if(ret == NB_TASK_RAN) continue;
            // do any special timeout stuff
            if( NB_CONTINUE == ( ret = try_need_by(nb) ) ) return;
            if(ret == NB_TASK_RAN) continue;
            // do prioritized queues
            if( NB_CONTINUE == ( ret = try_next_pri(nb)) ) return;               
            if(ret == NB_TASK_RAN) continue;
            // We don't do NB_NO_RUSH reentrantly 
            break;
        }
    }
    return;
}

need_by_entry build_need_by_task(
    need_by_task task, 
    void *context, 
    uint64_t limit_uSec
)
{
    return (need_by_entry) {
        .task = task,
        .context = context,
        .need_by_time = limit_uSec ? get_nb_now() + limit_uSec: 0, 
        .next = NULL
    };
}

void queue_need_by(need_by_obj *nbq, need_by_entry *entry, need_by_priority priority)
{
    if(nbq && entry)
    {
        if(priority > NB_NO_RUSH || priority < NB_ASAP) priority = NB_NO_RUSH;
        need_by_queue *q = &nbq->queues[priority];
        if(q->last)
        {
            q->last->next = entry;
            q->last = entry;
        }
        else // q was empty
        {
            q->last = entry;  
            q->next = entry;
        }
    }
}

bool is_need_by_done(need_by_entry *entry)
{
    return entry == entry->next;
}

