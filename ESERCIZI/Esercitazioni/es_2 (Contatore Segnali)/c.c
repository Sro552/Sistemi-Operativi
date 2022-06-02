#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define TRUE 1
#define FALSE 0

#define MAX 100

//Strutture per i dati
typedef struct Counter {
   int pid;
   int usr1;
   int usr2;
} Counter;

//Variabili globali
struct sigaction sa;
Counter counter[MAX];
int pos_cur=0;  //prima posizione libera per counter
int fine=FALSE;

//Funzioni appoggio

int find_sender(int pid){
    int trovato = FALSE;
    int pos=-1;
    for(int i=0;i<pos_cur && trovato==FALSE;i++){
        if(counter[i].pid==pid){
            trovato=TRUE;
            pos=i;
        }
    }
    if(!trovato){
        counter[pos_cur].pid=pid;
        pos=pos_cur;
        pos_cur++;
    }

    return pos;
}


// check if application run in foreground
int foreground() {
    // tcgetpgrp(fd):
    // process group ID of foreground process group for terminal of “fd”
    // (should be the controlling terminal of the calling process)
    // getpgrp():
    // returns process group ID of the calling process
    int fg=FALSE;
    if (getpgrp() == tcgetpgrp(STDOUT_FILENO)) 
        fg=TRUE;
    return fg;
}



void handler(int signo, siginfo_t *info){
    int sender;
    sender = (int)info->si_pid; //Prendo il PID di chi invia il segnale

    int pos = find_sender(sender);

    if(signo==SIGTERM || signo==SIGINT){
        fine=TRUE;
    }
    else{
        if(signo==SIGUSR1){
            counter[pos].usr1++;
        }
        else if(signo==SIGUSR2){
            counter[pos].usr2++;
        }
    }

    if(foreground()){
        printf("%d - %d/%d",counter[pos].pid,counter[pos].usr1,counter[pos].usr2);
    }
}


void set_handler(){
    sa.sa_flags = SA_SIGINFO;           // Ricevo infoemazioni da chi manda il segnale tipo il PID
    sa.sa_sigaction = &handler;
    sigaction(SIGUSR1,&sa,NULL);
    sigaction(SIGUSR2,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
}




int main(int argc,char** argv){
    set_handler();

    while(!fine){
        sleep(1);
    }
    for(int i=0;i<pos_cur;i++){
        printf("%d - %d/%d",counter[i].pid,counter[i].usr1,counter[i].usr2);
    }
}