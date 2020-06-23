#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include "scheduler.h"

#include <assert.h>
#include <curses.h>
#include <ucontext.h>
#include <string.h>
#include "util.h"
#include <time.h>
#include <unistd.h>



// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 128

// This is the size of each task's stack memory
#define STACK_SIZE 65536

#define quantum_length 100
// This struct will hold the all the necessary information for each task
typedef struct task_info {
  // This field stores all the state required to switch back to this task
  ucontext_t context;
  
  // This field stores another context. This one is only used when the task
  // is exiting.
  ucontext_t exit_context;
  
  ucontext_t temp_context;
  
  int task_id;
  
  int task_status;
     
  clock_t whenWakeup;
  
  int taskWaitingOn;
  
  bool forUSerInput;
  
  char saveUserInput;
  
  float cpu_request;
  
  float cpu_used;
  
  float finish_time;
  
  int isSleeping;
  int timeelapsed;
  // TODO: Add fields here so you can:
  //   a. Keep track of this task's state.
  //   b. If the task is sleeping, when should it wake up?
  //   c. If the task is waiting for another task, which task is it waiting for?
  //   d. Was the task blocked waiting for user input? Once you successfully
  //      read input, you will need to save it here so it can be returned.
} task_info_t;

int current_task = 0; //< The handle of the currently-executing task
int num_tasks = 1;    //< The number of tasks created so far
task_info_t tasks[MAX_TASKS]; //< Information for every task
int waiting_tasks = 1;

static ucontext_t initial_context;
clock_t time_custom = 0;

/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functiosn in this file.
 */
void scheduler_init() {
  // TODO: Initialize the state of the scheduler
     
     current_task = 0;     
     num_tasks = 1; 
     memset((uint8_t *) tasks, 0, sizeof(tasks));
     time_custom = clock();
    
}


/**
 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */
 void scheduler_main(){
   getcontext(&initial_context);
    for(;;)
    {        
        if( waiting_tasks==1){break;}
        
        for(int i =1;i<num_tasks+1;i++)
        {
            
            if(tasks[i].task_status==1 && tasks[i].isSleeping == 0 && tasks[i].forUSerInput==0)
            {  
            current_task = i;
            int val = swapcontext(&tasks[i].exit_context,&tasks[i].context); 
                if(val==0)
                {
                    //printf("I am here - main - for task : %d\n",current_task);
                    
                    tasks[i].task_status = -1;
                    if(tasks[i].taskWaitingOn==1)
                    {
                        tasks[i].taskWaitingOn=0;
                        waiting_tasks--;
                        
                    }
                    
                }
            tasks[i].cpu_used = tasks[i].cpu_used+quantum_length;                   
            }
            
            if(tasks[i].task_status==-1 && tasks[i].taskWaitingOn==1)
            {
                tasks[i].taskWaitingOn=0;
                waiting_tasks--;
                
            }
            
            if(tasks[i].isSleeping==1)
            {
                
                time_custom = clock();
                if(time_custom >= tasks[i].whenWakeup)
                {
                    
                    tasks[i].task_status = 1;
                    tasks[i].isSleeping = 0;
                    tasks[i].whenWakeup = 0;
                    //num_tasks--;
                    current_task = i;
                    int val_2 = swapcontext(&tasks[i].exit_context,&tasks[i].context);
                    if(val_2==0)
                    {
                        //printf("Disabling task : %d\n",tasks[i].task_id);
                        
                        tasks[i].task_status = -1;
                        if(tasks[i].taskWaitingOn==1)
                       {
                        tasks[i].taskWaitingOn=0;
                        waiting_tasks--;
                        
                       }
                    }
                    // setcontext(&tasks[i].temp_context);
                    
                }
            }
            
            if(tasks[i].forUSerInput==1)
            {
                current_task = i;
                tasks[i].forUSerInput = 0;
                    int val_2 = swapcontext(&tasks[i].exit_context,&tasks[i].context);
                    if(val_2==0)
                    {
                        
                        tasks[i].task_status = -1;
                        if(tasks[i].taskWaitingOn==1)
                    {
                        tasks[i].taskWaitingOn=0;
                        waiting_tasks--;
                        
                    }
                    }
                
            }
        }
        
    }        
        
}
 
void task_exit() {  
    printf("I am here exit task");
  tasks[current_task].task_status = -1;
  num_tasks--;
  //scheduler_main();
}

/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */
void task_create(task_t* handle, task_fn_t fn) {
  // Claim an index for the new task
  int index = num_tasks;
  num_tasks++;
  
  // Set the task handle to this index, since task_t is just an int
  *handle = index;
 
  // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second
  
  // First, duplicate the current context as a starting point
  getcontext(&tasks[index].exit_context);
  
  // Set up a stack for the exit context
  tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;
  
  // Set up a context to run when the task function returns. This should call task_exit.
  makecontext(&tasks[index].exit_context, task_exit, 0);
  
  // Now we start with the task's actual running context
  getcontext(&tasks[index].context);
  
  // Allocate a stack for the new task and add it to the context
  tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].context.uc_stack.ss_size = STACK_SIZE;
  
  // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
  tasks[index].context.uc_link = &tasks[index].exit_context;
  
  tasks[index].task_status = 1;
  tasks[index].cpu_used = 0.0;
  tasks[index].isSleeping = 0;
  tasks[index].forUSerInput = 0;
  tasks[index].task_id = index;
  // And finally, set up the context to execute the task function
  makecontext(&tasks[index].context, fn, 0);
}



/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */
void task_wait(task_t handle) {
  // tasks[handle].task_status = 1;
  tasks[handle].taskWaitingOn = 1;
  waiting_tasks++;
  scheduler_main();
    
}

/**
 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 * 
 * \param ms  The number of milliseconds the task should sleep.
 */
void task_sleep(size_t ms) {
  // TODO: Block this task until the requested time has elapsed.
  // Hint: Record the time the task should wake up instead of the time left for it to sleep. The bookkeeping is easier this way.
  // clock_t start_time = clock();
  // while(clock()<start_time+ms)
  // {
       // 
  // }
  time_custom = clock();
  
  tasks[current_task].whenWakeup =  time_custom + ms*1000;
  //printf("Sleep time = %ld, for task %d\n",tasks[current_task].whenWakeup,current_task);  
  tasks[current_task].isSleeping = 1;
  swapcontext(&tasks[current_task].context,&initial_context); //Duplicate the sleep call state context for resuming from here.
  
  
  //printf("Starting of the program, start_t = %ld\n", time_custom);
  //scheduler_main();
  
  
}

/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */
int task_readchar() {
  // TODO: Block this task until there is input available.
  // To check for input, call getch(). If it returns ERR, no input was available.
  // Otherwise, getch() will returns the character code that was read.
   //getcontext(&tasks[current_task].temp_context); 
  int ch;
  ch = getch();
  while(ch==-1)
  {
    swapcontext(&tasks[current_task].context,&initial_context);
    tasks[current_task].forUSerInput = 1;  
    ch = getch();    
  }
  return ch;
}

