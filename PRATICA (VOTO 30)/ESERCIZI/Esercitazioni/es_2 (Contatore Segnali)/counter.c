#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_PROCESS 20
#define PIPE_NAME "/tmp/fifo"  //Name of the slave pipes
#define TRUE 1
#define FALSE 0

//STRUTTURE UTILI
typedef struct Process{
    int pid;
    int usr1;
    int usr2;
}Process;

//VARIABILI GLOBALI
struct sigaction sa;  //Sigaction
Process pp[MAX_PROCESS]; //Array dei mittenti
int tot_process=0;  //numero di processi che hano inviato un segnale finora
int fine=FALSE;  //Bool per terminare


//FUNZIONI
//Trova processo che ha inviato segnale
int find_process(int sender){
    int trovato=FALSE;  //bool trovato
    int pos;    //Variabile che conterra la posizione da ritornare
    //Scorro il mio array di processi
    for(int i=0;i<tot_process && !trovato;i++){
        if(pp[i].pid==sender){
            pos=i;
            trovato=TRUE;
        }
    }
    //se non trovato lo aggiungo manualmente
    if(!trovato){
        pos=tot_process;
        tot_process++;
        pp[pos].pid=sender;
    }

    //ritorno la posizione
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
    if (getpgrp() == tcgetpgrp(STDOUT_FILENO)) fg=TRUE;
    return fg;
}

//Handler che conterà i segnali (sium è inutile)
void handler(int signo, siginfo_t *info,void *sium){
    int sender = (int)info->si_pid; //Prendo il PID di chi invia il segnale
    int pos=find_process(sender);

    //Controlli dei segnali
    if(signo == SIGINT || signo == SIGTERM){
        fine=TRUE;
    }
    else if(signo==SIGUSR1){
        pp[pos].usr1++;
    }
    else if(signo==SIGUSR2){
        pp[pos].usr2++;
    }

    //Controllo se è in background o in foregraund
    if(foreground()){
        printf("%d send signal %d\n",sender,signo);
    }
}

//Stampo tutti i mittenti e i segnali che hanno inviato
void stampa_finale(){
    for(int i=0;i<tot_process;i++){
        printf("PID:%d  SIGUSR1:%d  SIGUSR2:%d\n",pp[i].pid,pp[i].usr1,pp[i].usr2);
    }
}

//Setto sigaction
void set_action(){
    sa.sa_flags = SA_SIGINFO;  // Ricevo infoemazioni da chi manda il segnale tipo il PID
    sa.sa_sigaction = handler; //Uso sa_sigaction perchè posso usare info
    sigaction(SIGUSR1,&sa,NULL);    //Blocco i segnali
    sigaction(SIGUSR2,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
}


int main(int argc,char** argv){
    printf("%sMy PID--> %d%s\n",GREEN,getpid(),DF); //Stampo il mio pid
    set_action();   //Setto i segnali
    
    //Blocco il processo e guardo i segnali che arrivano
    while(1){
        if(fine){
            stampa_finale();
            exit(1);
        }
        else{
            sleep(1);
        }
    }
}