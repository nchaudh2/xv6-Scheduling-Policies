#include "types.h"
#include "user.h"

#define SOLUTION

#ifdef SOLUTION
#define SCHEDULER_DEFAULT  0
#define SCHEDULER_STRIDE 1

#define LOOP_CNT 0x02000000
#define MAX_CHILD_COUNT 6

struct {
    int pid;
} pa[MAX_CHILD_COUNT];

int pipe_fd[MAX_CHILD_COUNT][2];

void do_child(void)
{
    int cnt = 0;
    int tmp = 0;
    int n = 2 * LOOP_CNT;
    
	//printf(1, "I, %d , have %d tickets\n", getpid(), tickets_owned());

    while(cnt < n)
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
    int n = 2 * LOOP_CNT;

	//printf(1, "I, %d , have %d tickets\n", getpid(), tickets_owned());

    while(cnt < n)
    {
        tmp += cnt;
        cnt ++;
    }
}

void do_parent_partial(void)
{
    int cnt = 0;
    int tmp = 0;
    int n = 1.2 * LOOP_CNT;
    
	//printf(1, "I,%d , have %d tickets\n", getpid(), tickets_owned());

    while(cnt < n)
    {
        tmp += cnt;
        cnt ++;
    }
}



// Create "cnt" child processes and block all of them by having them read from empty pipes
void create_child_process(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0;
    int c = 0;

    for (i = 0; i < child_cnt; i++)
    {
        if (pipe(pipe_fd[i]) < 0)
        {
            printf(1, "pipe() failed!\n");
            exit();
        }

        pa[i].pid = fork();

        if (pa[i].pid < 0)
        {
            printf(1, "fork() failed!\n");
            exit();
        }
        else if (pa[i].pid == 0) // child
        {
            close(pipe_fd[i][1]); // close the write end on the child side
            read(pipe_fd[i][0], &c, 1); // reading any char will unblock it
            do_child();  
        }
        else // parent
        {
            close(pipe_fd[i][0]); // close the read end on the parent side
        }
    }        
}

void let_go_child_processes(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    for (i = 0; i < child_cnt; i++)
    {   
       write(pipe_fd[i][1], "A", 1); // start child-i
       close(pipe_fd[i][1]); // close the write end on the parent side
    }    
}


void wait_on_child_processes(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    for (i = 0; i < child_cnt; i++)
    {   
       if (wait() < 0)
       {
            printf(1, "wait() on child-%d failed!\n", i);
       }
    }            

}


int
main(int argc, char *argv[])
{
    int scheduler = 0;
    int child_cnt = 0;
    int n1 = 0, n2 = 0;
    
    // ---------------- test case 0 -------------------
    child_cnt = 3;
    printf(1, "===== Test case 0: default (RR) scheduler, %d processes =====\n", child_cnt);
    scheduler = SCHEDULER_DEFAULT;
    set_sched(scheduler);
    create_child_process(child_cnt);
    let_go_child_processes(child_cnt);
    enable_sched_trace(1);
    do_parent();	
    wait_on_child_processes(child_cnt);
    enable_sched_trace(0);
    printf(1, "\n\n");
	
    // ---------------- test case 1 -------------------
    child_cnt = 2;

    printf(1, "===== Stride scheduling test case 1 =====\n");
    printf(1, ">>> %d child processes, no transfer.\n", child_cnt);
    scheduler = SCHEDULER_STRIDE;
    set_sched(scheduler);

    create_child_process(child_cnt);
    let_go_child_processes(child_cnt);

    enable_sched_trace(1);

    do_parent();
    wait_on_child_processes(child_cnt);

    enable_sched_trace(0);

    printf(1, "\n\n");


    // ---------------- test case 2 -------------------
    child_cnt = 1;

    printf(1, "===== Stride scheduling test case 2 =====\n");
    printf(1, ">>> %d child processes, parent tranfers 1/2 of its tickets at the begining.\n", child_cnt);
    scheduler = SCHEDULER_STRIDE;
    set_sched(scheduler);
    create_child_process(child_cnt);

    printf(1, "Parent: tickets_owned() returned [%d]\n", tickets_owned());
    printf(1, "Parent: tranfer out -2 tickets, transfer_tickets() returned [%d]\n", 
            transfer_tickets(pa[0].pid, -2));
    printf(1, "Parent: tranfer out all tickets, transfer_tickets() returned [%d]\n", 
            transfer_tickets(pa[0].pid, tickets_owned()));
    printf(1, "Parent: tranfer out 1 ticket to pid 1000, transfer_tickets() returned [%d]\n", 
            transfer_tickets(1000, 1));            

    n1 = tickets_owned()/2;            
	 n2 = transfer_tickets(pa[0].pid, n1); // Here parent should have 25 tickets, child has 75 ==> ceiling(100/25)=4, ceiling(100/25)=1 ==> output should like: PCCCCPCCCC...
    printf(1, "Parent: tranfer out %d tickets, remaining tickets [%d]\n", n1, n2);

    
    
    let_go_child_processes(child_cnt);

    enable_sched_trace(1);

    do_parent();
    wait_on_child_processes(child_cnt);

    enable_sched_trace(0);

    printf(1, "\n\n");      
    

    
    // ---------------------- end -----------------------------
    exit(); // main process terminates
}
#else

#define LOOP_CNT 0x01000000

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
    int pid = 0;

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

    example_test_code();

    /* ---------------- end: add your test code ------------------- */

    enable_sched_trace(0);
    
    exit();
}


#endif
