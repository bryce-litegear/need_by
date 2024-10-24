

#include "unity.h"

#include "need_by.h"
#include <string.h>

need_by_obj the_queues;

void setUp(void)
{
    memset(&the_queues, 0, sizeof the_queues);
}

void tearDown(void)
{
}

void test_need_by_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement need_by");
}


void task1(void *context)
{
    TEST_ASSERT_EQUAL_PTR(task1, context);
}

void task2(void *context)
{
    TEST_ASSERT_EQUAL_PTR(task2, context);
}


void test_need_by(void)
{

    need_by_entry e1 = build_need_by_task(task1, task1, 10000);
    need_by_entry e2 = build_need_by_task(task2, task2, 500);
    need_by_entry e3 = build_need_by_task(task1, task1, 100);
    need_by_entry e4 = build_need_by_task(task2, task2, 200);
    need_by_entry e5 = build_need_by_task(task1, task1, 600);
    need_by_entry e6 = build_need_by_task(task2, task2, 700);
    need_by_entry e7 = build_need_by_task(task1, task1, 600);
    need_by_entry e8 = build_need_by_task(task2, task2, 700);
    need_by_entry e9 = build_need_by_task(task1, task1, 600);
    need_by_entry ea = build_need_by_task(task2, task2, 700);

    queue_need_by(&the_queues, &e1, NB_PRIORITY_2);
    queue_need_by(&the_queues, &e2, NB_NO_RUSH);
    queue_need_by(&the_queues, &e3, NB_PRIORITY_1);
    queue_need_by(&the_queues, &e4, NB_ASAP);
    queue_need_by(&the_queues, &e5, NB_PRIORITY_5);
    queue_need_by(&the_queues, &e6, NB_PRIORITY_3);
    queue_need_by(&the_queues, &e7, NB_PRIORITY_3);
    queue_need_by(&the_queues, &e8, NB_PRIORITY_3);
    queue_need_by(&the_queues, &e9, NB_PRIORITY_3);
    queue_need_by(&the_queues, &ea, NB_PRIORITY_3);

    do_need_by_tasks(&the_queues);

    TEST_ASSERT_TRUE(is_need_by_done(&e1));
    TEST_ASSERT_TRUE(is_need_by_done(&e2));
    TEST_ASSERT_TRUE(is_need_by_done(&e3));
    TEST_ASSERT_TRUE(is_need_by_done(&e4));
    TEST_ASSERT_TRUE(is_need_by_done(&e5));
    TEST_ASSERT_TRUE(is_need_by_done(&e6));
    TEST_ASSERT_TRUE(is_need_by_done(&e7));
    TEST_ASSERT_TRUE(is_need_by_done(&e8));
    TEST_ASSERT_TRUE(is_need_by_done(&e9));
    TEST_ASSERT_TRUE(is_need_by_done(&ea));

}
