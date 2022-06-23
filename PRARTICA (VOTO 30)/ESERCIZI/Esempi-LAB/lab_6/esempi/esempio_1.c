#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>//gsignal.c
void handler(int signo){
    printf("[%d,%d] sig%d received\n",getpid(),getpgid(0),signo);
    sleep(1); exit(0);
}
int main(void){
    signal(SIGUSR1,handler);
    signal(SIGUSR2,handler);
    int ancestor = getpid(); int group1 = fork(); int group2;
    if(getpid()!=ancestor ){ // First child
        setpgid(0,getpid()); // Become group leader
        fork();fork(); //Generated 3 children in new group
    }
    else{
        group2 = fork();
        if(getpid()!=ancestor){ // Second child
            setpgid(0,getpid()); // Become group leader
            fork();fork();
        }
    } //Generated 3 children in new group
    if(getpid()==ancestor){
        printf("[%d]Ancestor and I'll send signals\n",getpid());
        sleep(1);
        kill(-group2,SIGUSR2); //Send SIGUSR2 to group2
        kill(-group1,SIGUSR1); //Send SIGUSR1 to group1
    }else{
        printf("[%d,%d]chld waiting signal\n", getpid(),getpgid(0));
        while(1);
    }
    while(wait(NULL)>0);
    printf("All children terminated\n");
    
}