#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
 // cprintf("shutdown signal sent\n");
  outw( 0xB004, 0x0  | 0x2000 );

  return 0;
}

int sys_fork_winner(void)
{
  int n;
  if(!argint(0,&n))
    {
	if(n ==1)
	  myGlobal = 1;
	  //cprintf("\nsetting myGlobal to 1\n");
	else
	 myGlobal = 0;
	 //cprintf("\nsetting myGlobal to 0\n"); 
    }
//  get_random_bytes(&i, sizeof(int));
//  cprintf("\n%d\n",1);
  return 0;
}


int sys_set_sched(void)
{
 int m;
 if(!argint(0,&m))
  {
	//cprintf("\n\ninside method value of m: %d",m );
	if(m)
	  policy = 1;  //use stride scheduling
  }
	return 0;
}

int
sys_tickets_owned(void)
{
  return proc->tickets;
}

int sys_transfer_tickets(void)
{
 int pid, tickets;
 argint(0, &pid);
 argint(1, &tickets);
 return transfer_tickets(pid, tickets, proc);
}

extern int sched_trace_enabled;
int sys_enable_sched_trace(void)
{
  if (argint(0, &sched_trace_enabled) < 0)
  {
    cprintf("enable_sched_trace() failed!\n");
  }

  return 0;
}
