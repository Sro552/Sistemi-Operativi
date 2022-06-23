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

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

#define MAX_LEVEL 9 //Numero massimo di livelli
#define MAX 100

//Strutture necessarie
typedef struct Msg_packet{
    long mtype;
    char mtext[MAX];
} Msg_packet;

//Variabili globali

int command_level;
int my_level;
int isChild;    //Bool è figlio
int myParent;   //Salvo il PID padre
int figli_livello[2][MAX_LEVEL];   //Numero figli per livello
char command[MAX];  //Command da tastiera
int mytree;     //Salvo in che albero è il processo
int pos=0;      //Decito in che albero sono

//Funzioni

void creat_children(int command,int queue){
    Msg_packet snd_create;
    //Setto il tipo al livello superiore e il testo a c (= create)
    snd_create.mtype=command-1;
    strcpy(snd_create.mtext,"c");

    //Invio messaggio di create 
    for(int i=0;i<figli_livello[pos][command-1];i++){
        msgsnd(queue,&snd_create,sizeof(snd_create.mtext),0);
        figli_livello[pos][command]++;
    }
    
}

void print_children(int queue){
    Msg_packet snd_print;
    //Setto il tipo al livello superiore e il testo a c (= create)
  
    //Mando messaggio di stampa
    for(int i=0;i<MAX_LEVEL;i++){
        snd_print.mtype=i;
        strcpy(snd_print.mtext,"p");
        for(int j=0;j<figli_livello[pos][i];j++){
            msgsnd(queue,&snd_print,sizeof(snd_print.mtext),0);
        }

    }
}

void kill_children(int queue, int command){
    Msg_packet snd_kill;
    //Setto il tipo al livello superiore e il testo a c (= create)
  
    //Mando messaggio di kill
    for(int i=MAX_LEVEL;i>=command;i--){
        snd_kill.mtype=i;
        strcpy(snd_kill.mtext,"k");
        for(int j=0;j<figli_livello[pos][i];j++){
            msgsnd(queue,&snd_kill,sizeof(snd_kill.mtext),0);
        }
        figli_livello[pos][i]=0;
    }
}

int main(int argc,char** argv){
    //Reset array
    for(int i=0;i<MAX_LEVEL;i++){
        figli_livello[0][i]=0;
    }
    for(int i=0;i<MAX_LEVEL;i++){
        figli_livello[1][i]=0;
    }

    //Creo le 2 queue
    key_t queuekey1 = ftok("/tmp/unique",1);
    key_t queuekey2 = ftok("/tmp/unique",2);
    int queue[2];
    queue[0] = msgget(queuekey1, 0777 | IPC_CREAT);
    queue[1] = msgget(queuekey2, 0777 | IPC_CREAT);
    


    while(1){
        sleep(1); //Avoid graphic glithces
        //Prendo il nuovo comando
        printf("\nNext command: "); fgets(command,MAX,stdin); fflush(stdin);

        //Decido in che albero sono
        if(command[0]=='1'){
            pos=0;
        }
        else if(command[0]=='2'){
            pos=1;
        }
        else if(command[0]=='q'){
            printf("terminating...\n");
            pos=0;
            kill_children(queue[0],1);
            pos=1;
            kill_children(queue[1],1);
            exit(0);
        }
        else{
            continue;
        }

        switch (command[1]){
            case 'c':   //Create
                command_level=atoi(command+2);
                if(command_level==1){   //Se livello 1 creo subito
                    figli_livello[pos][1]++;
                    printf("Creating child at level 1\n");
                    isChild=fork();
                    
                }
                else if(command_level>1 && command_level<MAX_LEVEL){
                    printf("Creating child at level %d\n",command_level);
                    creat_children(command_level,queue[pos]);
                }
                else{
                    printf("%s IL NUMERO DI LIVELLO ERRATO\n%s",RED,DF);
                }
                break;
            case 'p':
                print_children(queue[pos]);
                break;
            case 'k':
                command_level=atoi(command+2);
                kill_children(queue[pos],command_level);
                break;
            default:
                break;
        }
        
        if(isChild==0){
            printf("I'm new child at level 1 whith id = %d\n",getpid());
            my_level=1;
            myParent=getppid();
            mytree=pos;
            break;
        }
    }

    while(1){
        //Ricevo messaggio
        Msg_packet rcv;
        int tmp;
        int tmp2;
        msgrcv(queue[mytree],&rcv,sizeof(rcv.mtext),my_level,0);


        switch(rcv.mtext[0]){
            case 'c':   //Create
                tmp=my_level+1;
                tmp2=mytree;
                isChild=fork();
                
                if(isChild==0){                   
                    my_level=tmp;
                    mytree=tmp2;
                    myParent=getppid();
                    printf("I'm new child at level %d whith id = %d\n",my_level,getpid());
                }
                break;
            case 'p':   //Print
                for(int i=1;i<my_level;i++){
                    printf("\t");
                }
                printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,my_level,DF);
                break;
            case 'k':   //Kill
                exit(0);
                break;

            default: break;
        }
    }
        
        
    
    
}