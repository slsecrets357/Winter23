#ifndef __SUT_H__
#define __SUT_H__
#include <stdbool.h>

typedef void (*sut_task_f)();

#include <ucontext.h>

#define MAX_THREADS                        32
#define THREAD_STACK_SIZE                  1024*64

typedef struct __threaddesc
{
	int threadid;
	char *threadstack;
	void *threadfunc;
	ucontext_t threadcontext;
} threaddesc;

struct iodesc {
	int *fd;
	char filename[128];
	char *buf;
	int size;
    int type;
    ucontext_t context;
    // 1 = open, 2 = close, 3= read, 4=write
};

extern threaddesc threadarr[MAX_THREADS*4];
extern int numthreads, tID;
extern ucontext_t parent;

void * C_EXEC();
void * I_EXEC();
void sut_init();
bool sut_create(sut_task_f fn);
void sut_yield();
void sut_exit();
int sut_open(char *dest);
void sut_write(int fd, char *buf, int size);
void sut_close(int fd);
char *sut_read(int fd, char *buf, int size);
void sut_shutdown();

#endif
