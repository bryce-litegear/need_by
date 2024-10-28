

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


void task1(void *context, need_by_status nbs)
{
    printf("task1, val= %i, %s\n", (intptr_t)context, nbs?"Timed Out": "Noraml Call");
}

void task2(void *context, need_by_status nbs)
{
    printf("task2, val= %i, %s\n", (intptr_t)context, nbs?"Timed Out": "Noraml Call");
}


void test_need_by_short_time(void)
{
    printf("%s\n", __func__);
    need_by_entry e1 = build_need_by_task(task1, (void *)1, 10000);
    need_by_entry e2 = build_need_by_task(task2, (void *)2, 500);
    need_by_entry e3 = build_need_by_task(task1, (void *)3, 100);
    need_by_entry e4 = build_need_by_task(task2, (void *)4, 200);
    need_by_entry e5 = build_need_by_task(task1, (void *)5, 600);
    need_by_entry e6 = build_need_by_task(task2, (void *)6, 700);
    need_by_entry e7 = build_need_by_task(task1, (void *)7, 600);
    need_by_entry e8 = build_need_by_task(task2, (void *)8, 700);
    need_by_entry e9 = build_need_by_task(task1, (void *)9, 600);
    need_by_entry ea = build_need_by_task(task2, (void *)10, 700);

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


void test_need_by_long_time(void)
{
    printf("%s\n", __func__);
    need_by_entry e1 = build_need_by_task(task1, (void *)1,  10000);
    need_by_entry e2 = build_need_by_task(task2, (void *)2,  50000);
    need_by_entry e3 = build_need_by_task(task1, (void *)3,  0);
    need_by_entry e4 = build_need_by_task(task2, (void *)4,  20000);
    need_by_entry e5 = build_need_by_task(task1, (void *)5,  60000);
    need_by_entry e6 = build_need_by_task(task2, (void *)6,  70000);
    need_by_entry e7 = build_need_by_task(task1, (void *)7,  60000);
    need_by_entry e8 = build_need_by_task(task2, (void *)8,  70000);
    need_by_entry e9 = build_need_by_task(task1, (void *)9,  60000);
    need_by_entry ea = build_need_by_task(task2, (void *)10, 70000);

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


void task3(void *context, need_by_status nbs)
{
    need_by_entry *ex = (need_by_entry*)context;
    printf("task3, val= %p, %s\n", ex, nbs?"Timed Out": "Noraml Call");
    if(nbs == NB_NORMAL_CALL)
    {
        *ex = build_need_by_task(task3, ex, 1);
        queue_need_by(&the_queues, ex, NB_PRIORITY_5);
    }

}

void test_need_by_complex(void)
{
    printf("%s\n", __func__);
    need_by_entry e1;
    need_by_entry e2;
    need_by_entry e3;
    need_by_entry e4;
    need_by_entry e5;
    need_by_entry e6;
    need_by_entry e7;
    need_by_entry e8;
    need_by_entry e9;
    need_by_entry ea;

    e1 = build_need_by_task(task3, &e1, 20000);
    e2 = build_need_by_task(task3, &e2, 50000);
    e3 = build_need_by_task(task3, &e3, 0);
    e4 = build_need_by_task(task3, &e4, 20000);
    e5 = build_need_by_task(task3, &e5, 60000);
    e6 = build_need_by_task(task3, &e6, 70000);
    e7 = build_need_by_task(task3, &e7, 20000);
    e8 = build_need_by_task(task3, &e8, 70000);
    e9 = build_need_by_task(task3, &e9, 20000);
    ea = build_need_by_task(task3, &ea, 70000);

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

void task4(void *context, need_by_status nbs)
{
    need_by_entry *ex = (need_by_entry*)context;
    printf("task4, val= %p, %s\n", ex, nbs?"Timed Out": "Noraml Call");
    if(nbs == NB_NORMAL_CALL)
    {
        *ex = build_need_by_task(task3, ex, 1);
        queue_need_by(&the_queues, ex, NB_PRIORITY_5);
    }
    else
    {
        *ex = build_need_by_task(task3, ex, 1);
        queue_need_by(&the_queues, ex, NB_PRIORITY_2);
        printf("enter recursion\n")    ;        
        do_need_by_tasks(&the_queues);    
        printf("exit recursion\n")    ;
    }
}


void test_need_by_recurse(void)
{
    printf("%s\n", __func__);
    need_by_entry e1;
    need_by_entry e2;
    need_by_entry e3;
    need_by_entry e4;
    need_by_entry e5;
    need_by_entry e6;
    need_by_entry e7;
    need_by_entry e8;
    need_by_entry e9;
    need_by_entry ea;

    e1 = build_need_by_task(task3, &e1, 20000);
    e2 = build_need_by_task(task4, &e2, 50000);
    e3 = build_need_by_task(task3, &e3, 0);
    e4 = build_need_by_task(task3, &e4, 20000);
    e5 = build_need_by_task(task3, &e5, 60000);
    e6 = build_need_by_task(task3, &e6, 70000);
    e7 = build_need_by_task(task4, &e7, 20000);
    e8 = build_need_by_task(task3, &e8, 70000);
    e9 = build_need_by_task(task4, &e9, 20000);
    ea = build_need_by_task(task3, &ea, 70000);

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

