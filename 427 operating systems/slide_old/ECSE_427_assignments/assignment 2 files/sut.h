#ifndef __SUT_H__
#define __SUT_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ucontext.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include "queue.h"


/* README

ECSE 427 assignment 2 - simple thread scheduler
author: Ze Yuan Fu - 260963676

-no special instructions or considerations for this assignment

Wishing you a pleasant grading experience~ */


#define STACK_SIZE 1024*1024        // stack size for contexts

typedef void (*sut_task_f)();

typedef struct queue_task {         // task control block (TCB) for threads
    // fields for ready queue
    int id;                         // id of task
    char *stack;                    // pointer to context stack
    sut_task_f *function;           // function executed by task
    ucontext_t threadcontext;       // context of task
    // fields for io queue
    int operation;                  // type of operation requested (1 = open, 2 = close, 3 = read, 4 = write)
    int fd;                         // file descriptor for open(), close(), read() and write()
	char *fname;                    // file name for open()
	char *buf;                      // character buffer for read() and write()
	int size;                       // buffer size for read() and write()
    bool completed;                 // denotes whether I-EXEC has successfully finished the operation
} queue_task;

bool running;                                   // determines if pthreads should run or exit, set to false in exit()
int numthreads, thread_id;                      // number of active threads and thread id counter
pthread_t *compute, *io;                        // compute and i/o p-threads
struct queue ready_queue, waiting_queue;        // ready (compute) and waiting (io) queues
ucontext_t parent;                              // parent context for C-EXEC (to come back to after yield or exit)
queue_task *compute_current, *io_current;       // tasks currently being executed by C-EXEC and I-EXEC

pthread_mutex_t rqlock = PTHREAD_MUTEX_INITIALIZER;         // mutex lock for compute queue
pthread_mutex_t wqlock = PTHREAD_MUTEX_INITIALIZER;         // mutex lock for io queue


// C-EXEC function, starts task at head of ready queue whenever the currently executing task yields or exits
void *C_EXEC() {
    struct queue_entry *last_popped;                        // variable for last popped element from ready_queue
    struct timespec timer;                                  // timespec for nanosleep
    timer.tv_sec = 0;
    timer.tv_nsec = 100000;                                 // 100000 ns = 100 us

    while(true) {
        pthread_mutex_lock(&rqlock);                        // lock while accessing queue
        last_popped = queue_peek_front(&ready_queue);       // check if queue contains anything
        pthread_mutex_unlock(&rqlock);                      // unlock queue access

        if (last_popped) {                                  // if there is at least 1 element in the ready queue

            pthread_mutex_lock(&rqlock);                    // lock while accessing queue
            last_popped = queue_pop_head(&ready_queue);     // get first TCB in queue
            pthread_mutex_unlock(&rqlock);                  // unlock queue access

            compute_current = (queue_task *) last_popped->data;         // set current executing task to popped TCB
            swapcontext(&parent, &(compute_current->threadcontext));    // swap from C-EXEC to the next task's context

        } else if (numthreads == 0 && !running) {           // if no threads are running and shutdown requested, exit thread
            pthread_exit(NULL);
        }

        nanosleep(&timer, NULL);
    }
}

// I-EXEC function, performs I/O operation requested by task at head of wait queue
void *I_EXEC() {
    struct queue_entry *last_popped;                        // variable for last popped element from waiting_queue
    struct timespec timer;                                  // timespec for nanosleep
    timer.tv_sec = 0;
    timer.tv_nsec = 100000;                                 // 100000 ns = 100 us

    while(true) {
        pthread_mutex_lock(&wqlock);                        // lock while accessing queue
        last_popped = queue_peek_front(&waiting_queue);     // check if queue contains anything
        pthread_mutex_unlock(&wqlock);                      // unlock queue access

        if (last_popped) {                                  // if there is at least 1 element in the waiting queue

            pthread_mutex_lock(&wqlock);                    // lock while accessing queue
            last_popped = queue_pop_head(&waiting_queue);   // get first TCB in queue
            pthread_mutex_unlock(&wqlock);                  // unlock queue access

            io_current = (queue_task *) last_popped->data;         // set current executing task to popped TCB

            switch (io_current->operation) {                // (1 = open, 2 = close, 3 = read, 4 = write)

                // opens file with specified file name with read/write perms in create and append mode
                case 1:
                    printf("opening file with filename %s \n", io_current->fname);
                    io_current->fd = open(io_current->fname, O_RDWR | O_APPEND, 0777);
                    io_current->completed = true;
                    break;

                // closes file with specified file descriptor
                case 2:
                    printf("closing file with fd %d \n", io_current->fd);
                    close(io_current->fd);
                    io_current->completed = true;
                    break;

                // reads the specified number of bytes from file descriptor fd into buffer buf and null-terminates the buffer
                case 3:
                    printf("reading %d bytes from file with fd %d \n", io_current->size, io_current->fd);
                    io_current->operation = read(io_current->fd, io_current->buf, (io_current->size)-1);        // load io_current->operation with return value of read() to pass to sut_read()
                    io_current->buf[(io_current->size)-1] = '\0';
                    io_current->completed = true;
                    break;

                // writes the specified number of bytes to file descriptor fd from buffer buf
                case 4:
                    printf("writing '%s' (%d bytes) to file with fd %d \n", io_current->buf, io_current->size, io_current->fd);
                    write(io_current->fd, io_current->buf, io_current->size);
                    io_current->completed = true;
                    break;
                
                // invalid operation code
                default:
                    printf("invalid operation code, resuming execution (where did this come from?) \n");
                    io_current->completed = false;
                    break;
            }

            if (io_current->completed) {                    // put TCB back into ready queue
                struct queue_entry *node = queue_new_node(io_current);
                pthread_mutex_lock(&rqlock);
                queue_insert_tail(&ready_queue, node);
                pthread_mutex_unlock(&rqlock);
            }

        } else if (numthreads == 0 && !running) {           // if no threads are running and shutdown requested, exit thread
            pthread_exit(NULL);
        }

        nanosleep(&timer, NULL);
    }
}

// initializer function for the thread scheduler library, initializes variables and threads
void sut_init() {
    // initialize global variables and allocate memory for pthreads
    numthreads = 0;
    thread_id = 1;
    running = true;
    compute = (pthread_t *)malloc(sizeof(pthread_t));
    io = (pthread_t *)malloc(sizeof(pthread_t)); 

    // create and initialize ready and waiting queues
    ready_queue = queue_create();
    waiting_queue = queue_create();
    queue_init(&ready_queue);
    queue_init(&waiting_queue);

    // initialize p-threads and parent context
    getcontext(&parent);
    pthread_create(compute, NULL, C_EXEC, NULL);
    pthread_create(io, NULL, I_EXEC, NULL);
    printf("thread scheduler library initialized \n");
}

// creates a new task control block struct with the given function and inserts it at the end of the ready queue
bool sut_create(sut_task_f fn) {
    //printf("adding new task \n");                       // debug statement

    // create new task object and set struct properties
    queue_task *new_task = (queue_task *) malloc(sizeof(queue_task));
    int error = getcontext(&(new_task->threadcontext));
    new_task->id = thread_id++;
    new_task->function = &fn;
    new_task->stack = (char *) malloc(STACK_SIZE);
    new_task->threadcontext.uc_stack.ss_sp = new_task->stack;
    new_task->threadcontext.uc_stack.ss_size = STACK_SIZE;
    new_task->threadcontext.uc_link = &parent;
    new_task->threadcontext.uc_stack.ss_flags = 0;

    makecontext(&(new_task->threadcontext), fn, 0);     // make context for task with given function

    // insert task at the end of the ready queue
    struct queue_entry *node = queue_new_node(new_task);
    pthread_mutex_lock(&rqlock);                        // lock while accessing queue
    queue_insert_tail(&ready_queue, node);
    pthread_mutex_unlock(&rqlock);                      // unlock queue access

    // check if task was successfully added to queue, return true if so and false if not
    if (error != -1) {
        numthreads++;
        //printf("new task added, number of threads: %d \n", numthreads);         // debug statement
        return true;
    } else {
        return false;
    }
}

// saves context of current task into its task control block, adds it to the tail of the ready queue and prompts C-EXEC to start the next task in queue
void sut_yield() {
    //printf("yielding... \n");                       // debug statement

    // save current task's context into TCB
    getcontext(&(compute_current->threadcontext));
    struct queue_entry *node = queue_new_node(compute_current);

    // insert TCB at the end of the queue
    pthread_mutex_lock(&rqlock);                     // lock while accessing queue
    queue_insert_tail(&ready_queue, node);
    pthread_mutex_unlock(&rqlock);                   // unlock queue access

    swapcontext(&(compute_current->threadcontext), &parent);    // swap to C-EXEC to start execution of task at head of ready queue
}

// terminates current task execution and prompts C-EXEC to start the next task in queue
void sut_exit() {
    //printf("exit called \n");                   // debug statement

    // get current context in temporary variable for swapcontext()
    ucontext_t temp_context;
    getcontext(&temp_context);

    //free(compute_current->stack);               // free memory allocated to task stack (segfaults for some reason)
    free(compute_current);                      // free memory allocated to TCB

    numthreads--;
    //printf("exit finished, %d threads remaining \n", numthreads);       // debug statement
    swapcontext(&temp_context, &parent);        // swap to C-EXEC to start execution of task at head of ready queue
}

// loads current task control block with open() call parameters, inserts it to tail of I/O queue and prompts C-EXEC to start the next task in queue
int sut_open(char *fname) {
    queue_task *io_task = compute_current;

    // save current task's context into TCB and load io task information
    getcontext(&(io_task->threadcontext));
    io_task->operation = 1;                             // (1 = open, 2 = close, 3 = read, 4 = write)
    io_task->fname = fname;
    io_task->completed = false;
    struct queue_entry *node = queue_new_node(io_task);

    // insert TCB at the end of the queue
    pthread_mutex_lock(&wqlock);                        // lock while accessing queue
    queue_insert_tail(&waiting_queue, node);
    pthread_mutex_unlock(&wqlock);                      // unlock queue access

    swapcontext(&(io_task->threadcontext), &parent);    // swap to C-EXEC to start execution of task at head of ready queue

    struct timespec timer;
    timer.tv_sec = 0;
    timer.tv_nsec = 50000;

    while(!io_task->completed) {                        // while operation is not completed, sleep for 50 us to wait for completion
        nanosleep(&timer, NULL);
    }

    return io_task->fd;                                 // return value of fd (file descriptor if success, -1 if error)
}

// loads current task control block with close() call parameters, inserts it to tail of I/O queue and prompts C-EXEC to start the next task in queue
void sut_close(int fd) {
    queue_task *io_task = compute_current;

    // save current task's context into TCB and load io task information
    getcontext(&(io_task->threadcontext));
    io_task->operation = 2;                             // (1 = open, 2 = close, 3 = read, 4 = write)
    io_task->fd = fd;
    io_task->completed = false;
    struct queue_entry *node = queue_new_node(io_task);

    // insert TCB at the end of the queue
    pthread_mutex_lock(&wqlock);                        // lock while accessing queue
    queue_insert_tail(&waiting_queue, node);
    pthread_mutex_unlock(&wqlock);                      // unlock queue access

    swapcontext(&(io_task->threadcontext), &parent);    // swap to C-EXEC to start execution of task at head of ready queue
}

// loads current task control block with read() call parameters, inserts it to tail of I/O queue and prompts C-EXEC to start the next task in queue
char *sut_read(int fd, char *buf, int size) {
    queue_task *io_task = compute_current;

    // save current task's context into TCB and load io task information
    getcontext(&(io_task->threadcontext));
    io_task->operation = 3;                             // (1 = open, 2 = close, 3 = read, 4 = write)
    io_task->fd = fd;
    io_task->buf = buf;
    io_task->size = size;
    io_task->completed = false;
    struct queue_entry *node = queue_new_node(io_task);

    // insert TCB at the end of the queue
    pthread_mutex_lock(&wqlock);                        // lock while accessing queue
    queue_insert_tail(&waiting_queue, node);
    pthread_mutex_unlock(&wqlock);                      // unlock queue access

    swapcontext(&(io_task->threadcontext), &parent);    // swap to C-EXEC to start execution of task at head of ready queue

    struct timespec timer;
    timer.tv_sec = 0;
    timer.tv_nsec = 50000;

    while(!io_task->completed) {                        // while operation is not completed, sleep for 50 us to wait for completion
        nanosleep(&timer, NULL);
    }

    if (io_task->operation == -1) {                     // if read() executed successfully, return "success", else return NULL
        return NULL;
    } else {
        return "success";
    }
}

// loads current task control block with write() call parameters, inserts it to tail of I/O queue and prompts C-EXEC to start the next task in queue
void sut_write(int fd, char *buf, int size) {
    queue_task *io_task = compute_current;

    // save current task's context into TCB and load io task information
    getcontext(&(io_task->threadcontext));
    io_task->operation = 4;                             // (1 = open, 2 = close, 3 = read, 4 = write)
    io_task->fd = fd;
    io_task->buf = buf;
    io_task->size = size;
    io_task->completed = false;
    struct queue_entry *node = queue_new_node(io_task);

    // insert TCB at the end of the queue
    pthread_mutex_lock(&wqlock);                        // lock while accessing queue
    queue_insert_tail(&waiting_queue, node);
    pthread_mutex_unlock(&wqlock);                      // unlock queue access

    swapcontext(&(io_task->threadcontext), &parent);    // swap to C-EXEC to start execution of task at head of ready queue
}

// waits for all tasks in both queues to terminate, then shuts down C-EXEC and I-EXEC threads
void sut_shutdown() {
    //printf("shutting down \n");         // debug statement
    running = false;                    // set running flag to false to signal to C-EXEC and I-EXEC that a shutdown was requested
    pthread_join(*compute, NULL);       // wait for pthreads to terminate
    pthread_join(*io, NULL);
    //printf("threads joined \n");        // debug statement
    free(compute);                      // deallocate memory for threads
    free(io);
    printf("finished shutting down thread scheduler library \n");
}

#endif


/* Thank you for reading through my assignment! Please enjoy this ascii art for your time :D

⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣤⣤⣤⣤⣤⣶⣦⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡿⠛⠉⠙⠛⠛⠛⠛⠻⢿⣿⣷⣤⡀⠀⠀⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⠋⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⠈⢻⣿⣿⡄⠀⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⣸⣿⡏⠀⠀⠀⣠⣶⣾⣿⣿⣿⠿⠿⠿⢿⣿⣿⣿⣄⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⣿⣿⠁⠀⠀⢰⣿⣿⣯⠁⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣷⡄⠀ 
⠀⠀⣀⣤⣴⣶⣶⣿⡟⠀⠀⠀⢸⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣷⠀ 
⠀⢰⣿⡟⠋⠉⣹⣿⡇⠀⠀⠀⠘⣿⣿⣿⣿⣷⣦⣤⣤⣤⣶⣶⣶⣶⣿⣿⣿⠀ 
⠀⢸⣿⡇⠀⠀⣿⣿⡇⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⠀ 
⠀⣸⣿⡇⠀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠉⠻⠿⣿⣿⣿⣿⡿⠿⠿⠛⢻⣿⡇⠀⠀ 
⠀⣿⣿⠁⠀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣧⠀⠀ 
⠀⣿⣿⠀⠀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀ 
⠀⣿⣿⠀⠀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀ 
⠀⢿⣿⡆⠀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⡇⠀⠀ 
⠀⠸⣿⣧⡀⠀⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⠃⠀⠀ 
⠀⠀⠛⢿⣿⣿⣿⣿⣇⠀⠀⠀⠀⠀⣰⣿⣿⣷⣶⣶⣶⠶⠀ ⣿⣿⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⣿⣿⠀⠀⠀⠀⠀⣿⣿⡇⠀⣽⣿⡏⠁⠀⠀⢸⣿⡇⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⣿⣿⠀⠀⠀⠀⠀⣿⣿⡇⠀⢹⣿⡆⠀⠀⠀⣸⣿⠇⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⢿⣿⣦⣄⣀⣠⣴⣿⣿⠁⠀⠈⠻⣿⣿⣿⣿⡿⠏⠀⠀⠀⠀ 
⠀⠀⠀⠀⠀⠀⠀⠈⠛⠻⠿⠿⠿⠿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀

*/