/**
 * @file need_by.h
 * @author Bryce Deary (bryce.deary@litegear.com)
 * @brief function runner queue
 * @date 2024-10-23
 * 
 * @copyright Copyright LightGear (2024)
 * 
 */
#ifndef NEED_BY_H
#define NEED_BY_H
#include <stdint.h>

// our task are of the form
typedef void (*need_by_task)(void *context);

// for setting time limit
uint32_t future_time(uint32_t uSecs);

// used by the priority queues
typedef struct need_by_entry need_by_entry;
struct need_by_entry
{
    need_by_entry *next;    // next item in queue
    need_by_task task;      // task to run
    void *context;          // the task's context
    uint32_t need_by_time;  // the override time
};

typedef struct need_by_queue
{
    need_by_entry *last;
    need_by_entry *next;
} need_by_queue;


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

typedef struct need_by_obj
{
    need_by_queue queues[NB_PRIORITIES];
    need_by_queue *active;
} need_by_obj;


void do_need_by_tasks(need_by_obj *nb);

int queue_need_by(need_by_task task, void *context, uint32_t limit_uSec, int8_t priority);



#endif // NEED_BY_H
