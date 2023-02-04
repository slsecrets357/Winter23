#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"
#include "sut.h"

/* bY kEVIN yU 260 956 214 */

int cexecnum = 1; // change to 2

struct queue readyq, waitq ;
// wait q is for waiting io exec request

threaddesc threadarr[MAX_THREADS*4];
typedef void (*sut_task_f)();
int numthreads, tID;
ucontext_t parent;
//ucontext_t ioparent;
pthread_t *t1;
pthread_t *t2;
pthread_t *t3;

pthread_mutex_t cmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t imutex = PTHREAD_MUTEX_INITIALIZER;

struct iodesc *descript;
// boolean for shutdown
bool turnoff = false;

bool isturnoff() {
    // check boolean and empty queues
    return (turnoff && !queue_peek_front(&waitq) && !queue_peek_front(&readyq));
}

void * C_EXEC() {
    while (true) {
        if (queue_peek_front(&readyq)) {
            // dono't pop because exit does that
            struct queue_entry *ptr = queue_peek_front(&readyq);
            //executes it?
            ucontext_t *context = (ucontext_t *) ptr->data;
            swapcontext(&parent, context);
            
        } else if (isturnoff()) {
            pthread_exit(NULL);
        }
        sleep(0.010);
    }
}

void * I_EXEC() {
    while (true) {
        if (queue_peek_front(&waitq)) {
            // exits immediately 
            struct queue_entry *ptr = queue_pop_head(&waitq);
            //executes it?
            
            struct iodesc *des = (struct iodesc *)ptr->data;
            ucontext_t context = (des->context);

            // 1 is open, 2 is close, 2 is read, 4 is write
            printf("%d\n", des->type);
            if (des->type == 1) {
                //printf("%d\n", des->fd);
                printf("opening\n");
                printf("\n");
                // create file if dne, read+write
                *(des->fd) = open((des->filename), O_RDWR | O_CREAT, 0666);
                //printf("%d\n",*(des->fd));
            } else if (des->type == 2) {
                close(*(des->fd));
            } else if (des->type == 3) {
                read(*(des->fd),&(des->buf), des->size);
            } else if (des->type == 4) {
                printf("writing\n");
                write(*(des->fd), des->buf, des->size);
            }
            
            //pthread_mutex_lock(&imutex);
            free(ptr);
            //pthread_mutex_unlock(&imutex);
            //pthread_mutex_lock(&cmutex);
            queue_insert_tail(&readyq, &context);
            //pthread_mutex_unlock(&cmutex);

        } else if (isturnoff()) {
            pthread_exit("NULL");
        }
        sleep(0.010);
    }
}

void sut_init() {
    /*This call initializes the SU T library.
     Needs to be called before making any other API calls*/	

     // xreating c-exec AND I-EXEC

    tID = 0; // thread id is never decremented
    //filenum = 0;

    readyq = queue_create();
    waitq = queue_create();

    queue_init(&readyq);
    queue_init(&waitq);

    t1 = (pthread_t*)malloc(sizeof(pthread_t));
    t2 = (pthread_t*)malloc(sizeof(pthread_t));

    pthread_create(t1, NULL, C_EXEC, NULL);
    pthread_create(t2, NULL, I_EXEC, NULL);
    // numthreads count the current number of acctive threads
    numthreads = 2;
        /*for the 2nd c-exec*/
    if (cexecnum == 2) {
        numthreads = 3;
        t3 = (pthread_t*)malloc(sizeof(pthread_t));
        pthread_create(t3, NULL, C_EXEC, NULL);
    }
    
    getcontext(&parent);
    //printf("initialized\n");
    
}

bool sut_create(sut_task_f fn) {
    
/*This call creates the task. 
The main body of the task is the function that is passed as the
only argument to this function. 
On success this returns a True (1) else returns False (0).*/
    threaddesc *tdescptr;
	if (numthreads >= MAX_THREADS) // MAX_THREADS == 32
	{
		printf("FATAL: Maximum thread limit reached... creation failed! \n");
		return false;
	} else {
        tdescptr = &(threadarr[tID]);
        getcontext(&(tdescptr->threadcontext));
        tdescptr->threadid = tID;
        tdescptr->threadstack = (char *)malloc(THREAD_STACK_SIZE);
        tdescptr->threadcontext.uc_stack.ss_sp = tdescptr->threadstack;
        tdescptr->threadcontext.uc_stack.ss_size = THREAD_STACK_SIZE;
        // we want to automatically go back to parent
        tdescptr->threadcontext.uc_link = &parent; 
        tdescptr->threadcontext.uc_stack.ss_flags = 0;
        tdescptr->threadfunc = &fn;
        
        makecontext(&(tdescptr->threadcontext), fn, 0);   

        struct queue_entry *node = queue_new_node(&(tdescptr->threadcontext));
        queue_insert_tail(&readyq, node);

        tID++;
        numthreads++;
        //printf("created a thread\n");
        return true;
    }
}

void sut_yield() {
    /* In SUT a running task can yield execution before 
    the completion of the function by issuing
    this yield call. 
    Another task that is present in the ready queue will start running and the task
    running the yield will get enqueued at the back of the ready queue. */
    //printf("start yield\n");
    ucontext_t oldcontext;
    getcontext(&oldcontext);

    struct queue_entry *ptr = queue_new_node(&oldcontext);
    
    //pthread_mutex_lock(&cmutex);
    queue_insert_tail(&readyq, ptr);
    //pthread_mutex_unlock(&cmutex);

    //pthread_mutex_lock(&cmutex);
    queue_pop_head(&readyq);
    //pthread_mutex_unlock(&cmutex);
    swapcontext(&oldcontext, &parent);
    //printf("yielded\n");
    
}

void sut_exit() {
/* You call this function terminate the task execution. 
This should not terminate the whole
program unless there is only one task. 
That is, if there are multiple SUT tasks running in
the system, exiting a task using this call is just going to terminate that task.*/
    numthreads--;
    //pthread_mutex_lock(&cmutex);
    queue_pop_head(&readyq);
    //pthread_mutex_unlock(&cmutex);
    //printf("exiting\n");
    sleep(2);
    return;
}

int sut_open(char *fname) {
/*If there is no such file, you will get a negative return value.*/
    descript = (struct iodesc*)malloc(sizeof(struct iodesc));
    
    ucontext_t oldcontext;
    getcontext(&oldcontext);
    int fd;
    //printf("opening\n");
    (descript->fd) = &fd; // modyfying descriptfd will also modify fd
    strcpy(descript->filename, fname);
    descript->type = 1;

    descript->context = oldcontext;

    struct queue_entry *node = queue_new_node((descript));
    //thread_mutex_lock(&imutex)
    
    queue_insert_tail(&waitq, node);
    //pthread_mutex_unlock(&imutex);
    
    queue_pop_head(&readyq);
    // swaps to c-exec
    swapcontext(&oldcontext, &parent);
    sleep(0.010);
    //printf("%d\n", fd);
    return (fd);
}

void sut_write(int fd, char *buf, int size) {
    /* We write the bytes in buf to the disk file that is already open. We don’t consider write
errors in this call. */
    descript = (struct iodesc*)malloc(sizeof(struct iodesc));

    (descript->fd) = &fd;
    descript->buf = buf;
    descript->size = size;
    descript->type = 4;

    
    getcontext(&(descript->context));


    struct queue_entry *node = queue_new_node(descript);
    //pthread_mutex_lock(&imutex);
    queue_insert_tail(&waitq, node);
    //pthread_mutex_unlock(&imutex);
    //pthread_mutex_lock(&cmutex);
    free(queue_pop_head(&readyq));
    //pthread_mutex_unlock(&cmutex);
    
    swapcontext(&(descript->context), &parent);
}

void sut_close(int fd) {
    descript = (struct iodesc*)malloc(sizeof(struct iodesc));
    ucontext_t oldcontext;
    getcontext(&oldcontext);

    descript->fd = &fd;

    // descript->buf=malloc(1024);
    descript->type = 2;

    descript->context = oldcontext;

    struct queue_entry *node = queue_new_node((descript));
    //pthread_mutex_lock(&imutex);
    queue_insert_tail(&waitq, node);
    //pthread_mutex_unlock(&imutex);
    //pthread_mutex_lock(&cmutex);
    queue_pop_head(&readyq);
    //pthread_mutex_unlock(&cmutex);
    
    swapcontext(&oldcontext, &parent);
    /* This function closes the file that is pointed by the file descriptor. */

}

char *sut_read(int fd, char *buf, int size) {
    /*The size tells the function the max number of bytes
that could be copied into the buffer. If the read operation is a success, it returns a non NULL
value. On error it returns NULL. */
    descript = (struct iodesc*)malloc(sizeof(struct iodesc));
    ucontext_t oldcontext;
    getcontext(&oldcontext);
    descript->fd = &fd;
    strcpy(descript->buf,buf);
    descript->type = 3;
    descript->size = size;
    descript->context = oldcontext;

    struct queue_entry *node = queue_new_node((descript));
    pthread_mutex_lock(&imutex);
    queue_insert_tail(&waitq, node);
    pthread_mutex_unlock(&imutex);
    pthread_mutex_lock(&cmutex);
    queue_pop_head(&readyq);
    pthread_mutex_unlock(&cmutex);

    swapcontext(&oldcontext, &parent);
    return buf;
}
void sut_shutdown() {
    
    turnoff = true;
    
    for (int i = 0; i<tID; i++) {
        // freeing mem
        threaddesc *tdes;
        tdes = &(threadarr[i]);
        free(tdes->threadstack);
    }
    
    sleep(0.1); // make sure they get the message
    pthread_join(*t1, NULL); // t is the thread we want to "join"
    pthread_join(*t2, NULL); 
    printf("shutting down\n");
    // //pthread_join(*t3, NULL); 

}

