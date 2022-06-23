#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define YELLOW "\x1b[33m"

#define MAX 32
#define TRUE 1
#define FALSE 0
#define RD 0
#define WR 1

//STRUTTURE UTILI
typedef struct msg{
    long mtype;
    char mtext[100];
}msg;

//VARIABILI GLOBALI
char name[MAX];
char action[MAX];
char value[MAX];
int pid;
key_t queueKey;
int queueId;
//char messaggio[MAX];


//FUNZIONI
//Print in stderr
void perr(char* str){
    fprintf(stderr,"%s%s%s\n",RED,str,DF);
}

//Check parametri input
int errori_parametri(int argc,char** argv){
    if(argc<4 || argc> 5){
        printf("%d",argc);
        perr("Errore Parametri in ingresso");
        return 1;
    }
    return 0;
}

void manda_segnale(){
    if(errno==0){
        kill(pid,SIGUSR1);
    }
    else{
        kill(pid,SIGUSR2);
    }
}

void new(){
    creat(name,0755);
    queueKey=ftok(name,1);
    queueId = msgget(queueKey, 0777|IPC_EXCL);
    printf("%s%d  %d\n",GREEN,queueId,errno);
    if (queueId != -1){ // Se la queue esiste già
        printf("ciao%d\n",queueId);
    }
    else{
        queueId = msgget(queueKey, 0777 | IPC_CREAT); // ...creo la queue
    }
    manda_segnale();
}

void get(){
    queueKey=ftok(name,1);
    queueId = msgget(queueKey, 0777);
    if (queueId != -1){ // Se la queue esiste già
        queueId = msgget(queueKey, 0777 | IPC_CREAT); // ...creo la queue
    }
    msg msg_rcv;
    msgrcv(queueId,&msg_rcv,sizeof(msg_rcv.mtext),1,0);
    manda_segnale();
    printf("%s\n",msg_rcv.mtext);
}

void del(){
    queueKey=ftok(name,1);
    queueId = msgget(queueKey, 0777);
    if (queueId != -1){ // Se la queue non esiste già
        msgctl(queueId, IPC_RMID, NULL); // La cancello...
    }
    manda_segnale();
}

void put(){
    creat(name,0755);
    queueKey=ftok(name,1);
    queueId = msgget(queueKey, 0777);
    if (queueId != -1){ // Se la queue esiste già
        queueId = msgget(queueKey, 0777 | IPC_CREAT); // ...creo la queue
    }
    //Invio
    msg messaggio;
    messaggio.mtype=1;
    strcpy(messaggio.mtext,value);
    msgsnd(queueId,&messaggio,sizeof(messaggio.mtext),0);
    manda_segnale();
}

void emp(){
    queueKey=ftok(name,1);
    queueId = msgget(queueKey, 0777);
    if (queueId != -1){ // Se la queue non esiste già
        msgctl(queueId, IPC_RMID, NULL); // La cancello...
    }
    manda_segnale();
    
}

int main(int argc,char** argv){
    if(errori_parametri(argc,argv)){
        return 1;
    }

    //Prendo i valori
    if(argc==4){
        strcpy(name,argv[1]);
        strcpy(action,argv[2]);
        pid=atoi(argv[3]);
    }
    else if(argc==5){
        strcpy(name,argv[1]);
        strcpy(action,argv[2]);
        strcpy(value,argv[3]);
        pid=atoi(argv[4]);
    }

    //ESEGUO ACTION
    //printf("%s\n%s\n",action,argv[2]);
    if(strcmp(action,"new")==0){
        new();
        
    }
    else if(strcmp(action,"put")==0){
        put();
    }
    else if(strcmp(action,"get")==0){
        get();
    }
    else if(strcmp(action,"del")==0){
        del();
    }
    else if(strcmp(action,"emp")==0){
        emp();
    }
    else{
        perr("Errore in action");
        kill(pid,SIGUSR2);
    }
    exit(1);
}