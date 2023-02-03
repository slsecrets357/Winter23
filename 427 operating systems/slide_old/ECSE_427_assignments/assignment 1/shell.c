#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>


/* README

ECSE 427 assignment 1 - simple shell
author: Ze Yuan Fu - 260963676

Note: Please ensure that commands and operators (|, & and >) are spaced correctly

E.G. 
ls -l | wc -l &    - Good
ls -l > output.txt - Good
ls -l|wc -l&       - Not good
ls -l| wc -l &     - Not good
ls -l> output.txt  - Not good

Thank you for your cooperation and I wish you a pleasant grading experience :D */


// linked list for job list
typedef struct process {
        int process_id;             // pid of job
        char *command;              // command of the job
        struct process *next;       // reference to next element in linked list
} process;


process *head;                      // head node for the job linked list (global variable so it may be accessed by any function without needing to pass it by reference)
int fg_process = -1;                // pid of the current foreground process (-1 means there is no foreground process) (global variable for the same reason as above)
#define max_arg_size 20             // constant for the maximum size of args[]


// handler function for SIGINT, sends SIGKILL to the current foreground process      
void handle_sigint(int signal) {
    printf("SIGINT sent, reaping foreground process with PID %d \n", fg_process);

    if (fg_process > 0) {                   // if there is a foreground process, send SIGTERM to it
        kill(fg_process, SIGTERM);           
        printf("Reaped foreground precess with PID %d \n", fg_process);
        fg_process = -1;
    } else {
    printf("Foreground process not found \n");
    }
}

// handler function for SIGCHLD, iterates through process linked list and removes the node representing the terminated process from the job list
void handle_sigchld(int signal) {
    int pid = waitpid(0, NULL, WNOHANG);
    //printf("handling sigchld with pid: %d \n", pid);  // debug statement

    if (pid > 0) {                                  // get pid of terminated process
        process *previous = head;
        process *current = previous->next;
        while (current != NULL) {
            if (current->process_id == pid) {       // iterate through process linked list until we reach the node with matching pid
                previous->next = current->next;
                free(current);                      // delete node representing terminated process
                if (pid == fg_process) {
                    fg_process = -1;                // if terminated process was in foreground, reset fg_process to -1
                }
                return;
            } else {
                previous = current;
                current = current->next;
            }
        }
    } else {
        fg_process = -1;
    }
}

// helper function that frees linked list and exits
void exit_shell(int i) {
    process *current;

    while (head != NULL) {          // iterate through process linked list and delete node
        current = head; 
        head = head->next;
        free(current);
    }

    kill(0, SIGKILL);               // kill all child processes
    free(head);
    exit(i);
}

int getcmd(char *prompt, char *args[], int *background, int *redirect, int *pipe, char output_file[]) {
    int i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t length, linecap = 0;

    // prompt user for command
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);

    if (length == -1) {             // check for ctrl+D
        exit_shell(-1);
    } else if (length == 0) {       // nothing entered
        return 0;
    }

    // check if background is specified
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else {
        *background = 0;
    }

    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++) {
            if (token[j] <= 32) {
                token[j] = '\0';
            } else if (token[j] == 62 && *redirect == 0) {      // check for '>' and set redirect flag if found
                *redirect = 1;
            } else if (token[j] == 124 && *pipe == 0) {         // check for '|' and set pipe flag if found
                *pipe = 1;
            }
        }

        if (strlen(token) > 0)              // populate args[]
            args[i++] = token;
    }

    if (*redirect == 1) {                   // check for file name and set file name variable
        int iterator = 0;   

        while (*args[iterator] != '>') {
            iterator++;
        }

        args[iterator] = NULL;

        if (args[iterator+1] == NULL) {     // no output file name
            printf("Please enter a file name to redirect output to \n");
            return -1;
        }

        strcpy(output_file, args[iterator+1]);
        args[iterator+1] = NULL;
        i = i - 2;
    } else {
        args[i] = NULL;
    }

    if (*pipe == 1) {                       // check if there are commands on both sides of the pipe symbol
        if (strcmp(args[0], "|") == 0 || strcmp(args[i-1], "|") == 0) {
            printf("Please ensure there are commands on both sides of '|' \n");
            return -1;
        }
    }

    free(line);
    return i;
}

// function to print current working directory
void pwd() {
    //printf("printing working directory \n");      // debug statement
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s \n", cwd);
    } else {
        printf("pwd failed \n");
    }
}

// function to iterate through the process linked list and print jobs and pids
void jobs() {
    printf("Listing background jobs \n");
    process *current = head;
    current = current->next;
    int counter = 1;

    if (current != NULL) {
        while (current != NULL) {
            printf("Job number: %d          Job command: %s          Job PID: %i \n", counter, current->command, current->process_id);
            current = current->next;
            counter++;
        }
    printf("Use the job number as parameter for the fg command to bring it to the foreground");
    } else {
        printf("No jobs found \n");
    }
}

// function to add a process node to the background process linked list
void add_process_to_list(int pid, char *command) {
    process *new_process = (process*)malloc(sizeof(process));       // create process node and populate data fields
    new_process->process_id = pid;
    new_process->command = command;
    new_process->next = NULL;
    process *current = head;

    while (current->next != NULL) {     // iterate through process linked list until the end
        current = current->next;
    }

    current->next = new_process;        // add new node to tail of list
}

int main(void) {
    char *args[max_arg_size];
    char output_file[64];
    int bg, rd, piping, pid, pid2;

    // bind signal hanglers
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        printf("could not bind SIGINT to signal handler \n");
        exit_shell(1);
    } 
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
        printf("could not bind SIGTSTP to signal handler \n");
        exit_shell(1);
    }
    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        printf("could not bind SIGCHLD to signal handler \n");
        exit_shell(1);
    }

    head = (process*)malloc(sizeof(process));       // allocate memory for dummy head node
    head->process_id = -1;                          // no foreground process at program start
    head->next = NULL;

    while(1) {
        // reset parameter variables and args[]
        bg = 0;
        rd = 0;
        piping = 0;
        pid = 0;
        pid2 = 0;
        memset(args, 0, max_arg_size);

        // get user command
        int cnt = getcmd("\n>> ", args, &bg, &rd, &piping, output_file);
      
        // go to next loop if command empty or error
        if (cnt <= 0) {
            continue;
        }

        //printf("command: %s, bg: %d, rd: %d, pipe: %d \n", args[0], bg, rd, piping);   // debug statement

        // if command involves pipes, set up pipe, split command and fork
        if (piping == 1) {
            int pp[2];

            if (pipe(pp) == -1) {
                printf("pipe creation failed \n");
                continue;
            }            

            //split command into two
            char *command2[max_arg_size];
            int counter1 = 0;
            int counter2 = 0;

            while (strcmp(args[counter1], "|") != 0) {
                counter1++;
            }

            args[counter1++] = NULL;

            while (args[counter1] != NULL) {
                command2[counter2++] = args[counter1];
                args[counter1++] = NULL;
            }

            args[counter1] = NULL;
            command2[counter2] = NULL;

            pid = fork();

            if (pid < 0) {                              // error
                perror("forking 1st child failed");
                continue;
            } else if (pid == 0) {                      // child code
                //printf("in first child code \n");     // debug statement
                if (dup2(pp[1], 1) == -1) {
                    perror("error dup2ing write end for first command");
                    continue;
                }
                close(pp[0]);
                close(pp[1]);

                if (execvp(args[0], args) < 0) {        // execute first command of pipe
                    perror("first child exec failed");
                    continue;
                } 
            } else {                                    // second part of pipe
                pid2 = fork();

                if (pid2 < 0) {                         // error
                    perror("forking 2nd child failed");
                    continue;
                } else if (pid2 == 0) {                 // child code
                    //printf("in second child code \n");    // debug statement
                    if (dup2(pp[0], 0) == -1) {
                        perror("error dup2ing read end for second command");
                        continue;
                    }
                    close(pp[0]);
                    close(pp[1]);

                    //printf("second child executing %s \n", command2[0]);  // debug statement
                    if (execvp(command2[0], command2) < 0) {    // execute second command of pipe
                        perror("exec2 failed");
                        continue;
                    }

                } else {                                // parent code
                    close(pp[0]);       // close file descriptors to give child processes input
                    close(pp[1]);
                }
            }
        } else {
            // echo built-in command
            if (strcmp(args[0], "echo") == 0) {
                if (args[1] != NULL) {
                    int counter = 1;
                    while(args[counter] != NULL) {
                        printf("%s ", args[counter]);       // print all arguments of echo command
                        counter++;
                    }
                } else {
                    printf("echo is enabled \n");
                }
            }

            // cd built-in command, calls pwd() if no argument was passed
            else if (strcmp(args[0], "cd") == 0) {
                if (args[1] != 0) {
                    //printf("changing directory to %s \n", args[1]);       // debug statement
                    if (chdir(args[1]) != 0) {                                  // change directory to the one specified in argument
                        printf("could not change directory to %s \n", args[1]);
                    }
                } else {
                    pwd();
                }
            }

            // pwd built-in command
            else if (strcmp(args[0], "pwd") == 0) {
                pwd();
            }

            // exit built-in command
            else if (strcmp(args[0], "exit") == 0) {
                printf("terminating all child processes and exiting \n");
                exit_shell(0);
            }

            // fg built-in command, calls jobs() if no argument was passed
            else if (strcmp(args[0], "fg") == 0) {
                if (args[1] != 0) {
                    printf("calling job with pid %s to foreground \n", args[1]);
                    int index = atoi(args[1]); 

                    if (index <= 0) {
                        printf("please enter a valid index \n");
                        continue;
                    }

                    process *previous = head;
                    process *current;
                    int counter = 0;
                    current = head->next;

                    while (current != NULL) {
                        counter++;
                        current = current->next;
                    }

                    if (index <= counter) {
                        for (int i = 1; i < index; i++) {           // go to node indicated by index
                            previous = previous->next;
                        }
                        current = previous->next;
                        previous->next = previous->next->next;      // remove node from background process list
                        fg_process = current->process_id;
                        free(current);
                        printf("brought process with pid %d to foreground \n", fg_process);
                        waitpid(fg_process, NULL, 0);               // wait for background process
                        continue;
                    } else {            
                    printf("invalid index entered, please try again \n");
                    }
                } else {
                    jobs();
                }
            }

            // jobs built-in command
            else if (strcmp(args[0], "jobs") == 0) {
                jobs();
            }

            // running non built-in command
            else {
                pid = fork();

                if (pid < 0) {                  //error
                    perror("fork failed");
                    continue;
                } else if (pid == 0) {          //child code
                    //printf("child executing %s \n", args[0]);   //debug statement

                    // if redirect requested, create file and add to fdt
                    if (rd == 1) {
                        int file;
                        if ((file = creat(output_file, 0777)) < 0) {
                            perror("error creating output file");
                            continue;
                        }
                        dup2(file, 1); 
                        close(file);
                    }

                    setpgid(0, 0);          // set process group id to smth new so ctrl+c does not get sent to child process

                    if (execvp(args[0], args) < 0) {        // execute command
                        perror("exec failed");
                        continue;
                    }  
                } else {                        //parent code
                    if (bg == 0){               // process running in foreground
                        //printf("parent waiting for %s \n", args[0]);      // debug statement
                        fg_process = pid;       // set foreground process variable to pid of new process
                        waitpid(pid, NULL, 0);
                    } else {                    // process running in background
                        add_process_to_list(pid, args[0]);      // add node to linked list representing new process
                        //printf("parent continuing with execution, child has pid %d \n", pid);  //debug statement
                    }
                }      
            }
        }
    }
}

/* Thank you for reading through my assignment! Please enjoy this ascii art for your time :D

⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⣤⣤⣀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⠟⠉⠉⠉⠉⠉⠉⠉⠙⠻⢶⣄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠙⣷⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⡟⠀⣠⣶⠛⠛⠛⠛⠛⠛⠳⣦⡀⠀⠘⣿⡄⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠁⠀⢹⣿⣦⣀⣀⣀⣀⣀⣠⣼⡇⠀⠀⠸⣷⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡏⠀⠀⠀⠉⠛⠿⠿⠿⠿⠛⠋⠁⠀⠀⠀⠀ ⣿
⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⢻⡇⠀
⠀⠀⠀⠀⠀⠀⠀⠀⣸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⢸⡇⠀
⠀⠀⠀⠀⠀⠀⠀⠀⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⢸⣧⠀
⠀⠀⠀⠀⠀⠀⠀⢸⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀   ⣿⠀
⠀⠀⠀⠀⠀⠀⠀⣾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⣿⠀
⠀⠀⠀⠀⠀⠀⠀⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⣿⠀
⠀⠀⠀⠀⠀⠀⢰⣿⠀⠀⠀⠀⣠⡶⠶⠿⠿⠿⠿⢷⣦⠀⠀⠀⠀ ⠀⠀⣿⠀
⠀⠀⣀⣀⣀⠀⣸⡇⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀ ⠀⠀⣿⠀
⣠⡿⠛⠛⠛⠛⠻⠀⠀⠀⠀⠀⢸⣇⠀⠀⠀⠀⠀⠀⣿⠇⠀⠀⠀ ⠀ ⣿⠀
⢻⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡟⠀⠀    ⣤⣴⣿⠀⠀⠀⠀⠀ ⣿⠀
⠈⠙⢷⣶⣦⣤⣤⣤⣴⣶⣾⠿⠛     ⣶⡟⠉⠀⠀⠀⠀⠀⠀ ⡟⠀
⠀                     ⣿⣆⡀  ⠀⢀⣠⣴⡾⠃⠀
⠀                      ⠈⠛⠻⠿⠟⠋ 

Have a nice day!

*/