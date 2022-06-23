#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>//gsignal.c

int main(void){
    
    int ancestor = getpid();
    int group1;
    int group2;

    group1 = !fork();

    if(getpid() != ancestor){
        setpgid(0,getpid());
        fork();
        fork();
        sleep(2);
        return 0;
    }
    else{
        group2 = fork();
        if(getpid() != ancestor){
            setpgid(0,getpid());
            fork();fork();
            sleep(4);
            return 0;
        }
    }
    sleep(1);
    while(waitpid(-group1,NULL,0)>0);
    printf("GRUPPO 1 FINITO\n");
    while(waitpid(-group2,NULL,0)>0);
    printf("gruppo 2 finito\n");
}