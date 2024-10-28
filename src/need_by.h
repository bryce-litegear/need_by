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
#include <stdbool.h>

#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

typedef enum need_by_status
{
    NB_NORMAL_CALL,
    NB_TIMED_OUT,
} need_by_status;


// our task are of the form
typedef void (*need_by_task)(void *context, need_by_status nb_status);

// used by the priority queues
typedef struct need_by_entry need_by_entry;
struct need_by_entry
{
    need_by_entry *next;    // next item in queue
    need_by_task task;      // task to run
    uint64_t need_by_time;  // the override time
    void *context;          // the task's context
};

typedef struct need_by_queue
{
    need_by_entry *last;        // insertion point in queue
    need_by_entry *next;        // extraction point in queue
    need_by_entry *active_task; // currently running task
    need_by_entry *to_task;     // elevated task from timeout
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
    need_by_queue *active_queue;
    need_by_queue *busy_queue;
    need_by_priority active_priority;
} need_by_obj;

// 

/**
 * @brief called in background loop or reentrantly in cooperating tasks (with care)
 *  this function runs all the queued tasks in the need_by_obj.
 * 
 * @param nb pointer to object.
 */
void do_need_by_tasks(need_by_obj *nb);

/**
 * @brief build a need_by task object for insertion into one of the priority 
 *      queues. This return a filled in handle not a pointer.
 * 
 * @param task pointer to a need_by task
 * @param context pointer to context info for the task
 * @param future_limit_uSec the time limit before forcing the task to run. or 0 if don't care
 * @return need_by_entry 
 */
need_by_entry build_need_by_task(need_by_task task, void *context, uint64_t future_limit_uSec);

/**
 * @brief add the need_by_entry to one of the prioritized queues in the need_by_obj.
 * 
 * @param nbq the need_by_obj that contains the prioritized queues
 * @param entry pointer to the need_by_entry to add to the queue
 * @param priority the priority of the request where NB_ASAP is highest,
 *                 NB_PRIORITY_1 to NB_PRIORITY_5 or NB_NO_RUSH
 * @details the entry may be tested using the is_need_by_done() function
 *      to determine when the queued task has been started.
 */
void queue_need_by(need_by_obj *nbq, need_by_entry *entry, need_by_priority priority);


/**
 * @brief test if a prior scheduled task has been run (at least started) so the
 *  handle variable is available for reuse. (must be re built before reuse)
 * 
 * @param entry pointer to the handle
 * @return true if the handle has been de-queued
 * @return false if handle is still in a queue
 */
bool is_need_by_done(need_by_entry *entry);

#endif // NEED_BY_H
