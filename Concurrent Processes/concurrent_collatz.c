#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{

    pid_t childPid;
    pid_t childPid2;
    int cmdLineInput = atoi(argv[1]);
    int mainProcessId = getpid();

    if(mainProcessId == getpid()){
    childPid = fork();
    }

    if(mainProcessId == getpid()){
        childPid2 = fork();
    }

    if (childPid == 0){
        
        char buffer[26];
        int b = cmdLineInput;
        printf("Child1 init n = %d \n", b);

        while (b != 1) {
            if (b % 2 == 1){
                b = (3 * b) + 1;
            }

            else if(b % 2 == 0){
                b = (b / 2);
            }
            sprintf(buffer, "Child1 n = %d ", b);
            printf("%s \n", buffer);
        }
    }

        wait(NULL);
        printf("DONE. \n");
        printf("Main process id is %d \n", mainProcessId);

}
