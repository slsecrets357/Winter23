#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
// #include <sys/wait.h>
#include <sys/types.h>

int main(void)
{
    /* code */
    int status;
    for(int i=0; i<3; i++){
        printf("%d",i);
        pid_t fork_ret;
        if(fork_ret=fork()){

        }
    }
    return 0;
}
