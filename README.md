# need_by

This module provides a prioritized queue of functions with reentrant capabilities for support of cooperative multi-tasking.

Tasks are functions of the form 

```C
typedef enum need_by_status
{
    NB_NORMAL_CALL,
    NB_TIMED_OUT,
} need_by_status;

void (*nb_task)(void *context, need_by_status status);
```

Before scheduling a task, create a task handle, then en-queue the
handle at the desired priority. Where the ASAP priority will always run before
anything else, and the NO_RUSH priority will only be run after all other priorities.
The other levels are 1-5 with 1 being highest priority. Note that the numbered 
priorities also allow the setting for a maximum delay to ensure that the tasks
will run even if a lower priority is being blocked. Tasks that reach the maximum
delay timeout will be run after the ASAP queue is emptied.

```C
typedef enum need_by_priority
{
    NB_ASAP,        // always run first
    NB_PRIORITY_1,  // highest priority
    NB_PRIORITY_2,
    NB_PRIORITY_3,
    NB_PRIORITY_4,
    NB_PRIORITY_5,  // lowest priority
    NB_NO_RUSH,     // run when idle
    NB_PRIORITIES
} need_by_priority;

uint64_t my_worst_case_delay = 2000; // uSec
need_by_entry my_entry = build_need_by_task(my_task, (void*)my_context, my_worst_case_delay);
queue_need_by( &main_queue_object, &my_entry, NB_PRIORITY_3);
```

In the main background loop, define a need_by_obj, clear it to zeros, then call the
do_need_by_tasks function which will only return after all queued tasks are complete,
including any task queued by the queued tasks themselves.

```C
struct first_task_state
{
    int var1;
} first_task_state;

void my_first_task(void *context, need_by_status status);

need_by_obj main_task_queues;
bool keep_running = true;

void main(void)
{

    // setup section

    uint64_t my_worst_case_delay = 2000; // uSec
    need_by_entry my_entry = build_need_by_task(my_first_task, &first_task_state, my_worst_case_delay);
    queue_need_by( &main_task_queues, &my_entry, NB_PRIORITY_3);


    // main loop
    while(keep_running)  // loop forever
    {
        // do user tasks
        do_need_by_tasks(&main_task_queues);

        // do things outside the task queues

    }

}


```