#include "types.h"
#include "user.h"

#define LOOP_CNT 0x02000000

int pid = 0;

void do_child(void)
{
    int cnt = 0;
    int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    exit();
}


void do_parent(void)
{

  int cnt = 0;
    int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
}

void example_test_code()
{
    pid = fork();
    if (pid < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid == 0) // child
    {
        do_child();
    }
    else // parent
    {
	   do_parent();
       if (wait() < 0)
       {
            printf(1, "wait() failed!\n");
       }
    }
	
	printf(1, "\n");
}

int
main(int argc, char *argv[])
{
    enable_sched_trace(1);

    /* ---------------- start: add your test code ------------------- */

        set_sched(1);
        example_test_code();
    /* ---------------- end: add your test code ------------------- */

    enable_sched_trace(0);
    
    exit();
}

