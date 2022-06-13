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

#define MAX 100
#define MAX_LEVEL 10

//VARIABILI GLOBALI
char buf[MAX];  //Buffer per input da tastiera
int isChild=-1;  //Var per sapere se è un figlio 
int myParent;   //Var per salvare il padre
int mylevel;    //Var per ricordare il livello del processo
int command;   //Var per prendere input da tastiera
key_t queueKey; //Key della queue
int queueId;    //Id della queue
int nChild_level[MAX_LEVEL];

//STRUTTURE NECESSARIE
//msg per le queue
typedef struct msg{
    long mtype;
    char mtext[MAX];
}msg;

//FUNZIONI
//Print error in RED
void perr(char* str){
    printf("%s%s%s\n",RED,str,DF);
}

//Create new child
void create_child(){ 
    msg msg_create;
    msg_create.mtype=command-1;
    strcpy(msg_create.mtext,"c");
    printf("Creating grandchild at level %d\n",command);
    for(int i=0;i<nChild_level[command-1];i++){
        msgsnd(queueId,&msg_create,sizeof(msg_create.mtext),0);
        nChild_level[command]++;
    }
    
}

//Kill child
void kill_child(){
    msg msg_kill;
    for(int i=MAX_LEVEL;i>=command;i--){
        msg_kill.mtype=i;
        strcpy(msg_kill.mtext,"k");
        for(int j=0;j<nChild_level[i];j++){
            msgsnd(queueId,&msg_kill,sizeof(msg_kill.mtext),0);
        }     
        nChild_level[i]=0;
    } 
}

//Print all tree
void print_tree(){
    msg msg_print;
    for(int i=1;i<MAX_LEVEL;i++){
        msg_print.mtype=i;
        strcpy(msg_print.mtext,"p");
        for(int j=0;j<nChild_level[i];j++){
            msgsnd(queueId,&msg_print,sizeof(msg_print.mtext),0);
        }     
    } 
}


int main(int argc,char** argv){
    //Reset array
    for(int i=0;i<MAX_LEVEL;i++){
        nChild_level[i]=0;
    }

    //Creo la queue
    queueKey=ftok("/tmp/unique",1);
    queueId=msgget(queueKey,0777 | IPC_CREAT);

    //Blocco il processo principale
    while(1){
        //Prendo l'input da tastiera
        printf("Next command: "); fflush(stdout); read(0,buf,MAX);

        switch(buf[0]){
            case 'c':   //Create
                //Prelevo il numero del livello dal comando
                command=atoi(buf+1);
                //Se figlio di primo livello lo creo subito
                if(command==1){ //Se è primo livello creo subito il figlio
                    nChild_level[1]++;
                    printf("Creating child at level 1\n");
                    isChild=fork();
                    if(isChild==0){
                        mylevel=1;  //Salvo livello
                        myParent=getppid(); //Save PID padre
                        printf("I'm new child at level 1 with id = %d\n",getpid());
                    }
                }
                else if(command>1 && command<MAX_LEVEL){    //Create
                    create_child();
                }
                else{   //Errore
                    perr("Errore input");
                }
                break;
            case 'k': //Kill level
                //Prelevo il numero del livello dal comando
                command=atoi(buf+1);
                if(command>=1 && command<MAX_LEVEL){    //KILL
                    kill_child();
                }
                else{   //Errore
                    perr("Errore input");
                }
                break;
            case 'p':
                print_tree();   //Stampo l'intero albero
                break;
            case 'q':
                command=1;  //Kill tutti i figli
                kill_child();
                while(wait(NULL)>0);
                exit(1);    //Esco
                break;
            default:
                break;
        }
        //Faccio uscire il figlio
        if(isChild==0){
            isChild=-1;
            break;
        }

        sleep(1); //Wait all process
    }
    //Blocco il figli
    while(1){
        //Ricevo i messaggi
        msg msg_rcv;
        int tmp;
        msgrcv(queueId,&msg_rcv,sizeof(msg_rcv.mtext),mylevel,0);

        //Scan child
        switch(msg_rcv.mtext[0]){
            case 'c':   //Create
                tmp=mylevel+1;
                isChild=fork();
                if(isChild==0){
                        mylevel=tmp;
                        myParent=getppid(); //Save PID padre
                        printf("I'm new child at level %d with id = %d\n",mylevel,getpid());
                }
                break;
            case 'p':   //Print
                for(int i=1;i<mylevel;i++){
                    printf("\t");
                }
                printf("%s[ID %d - Parent: %d] death %d%s\n",GREEN,getpid(),myParent,mylevel,DF);
                break;
            case 'k':   //Kill
                exit(1);
                break;
        }
    }
}