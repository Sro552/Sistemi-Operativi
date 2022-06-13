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

#define MAX_LEVEL 9
#define MAX 100

//Strutture necessarie
typedef struct Msg_packet{
    long mtype;
    char mtext[MAX];
} Msg_packet;

//Variabili globali
int command_level;
int my_level;
int isChild=1;
int myParent;
int figli_livello[MAX_LEVEL][2];

//Funzioni

void creat_children(int command,int queue,int n_tree){
    Msg_packet snd_create;
    //Setto il tipo al livello superiore e il testo a c (= create)
    snd_create.mtype=command-1;
    strcpy(snd_create.mtext,"c");

    for(int i=0;i<figli_livello[command-1][n_tree];i++){
        msgsnd(queue,&snd_create,sizeof(snd_create.mtext),0);
        figli_livello[command][n_tree]++;
    }
    
}

void print_children(int queue,int n_tree){
    Msg_packet snd_print;
    //Setto il tipo al livello superiore e il testo a c (= create)
  

    for(int i=0;i<MAX_LEVEL;i++){
        snd_print.mtype=i;
        strcpy(snd_print.mtext,"p");
        for(int j=0;j<figli_livello[i][n_tree];j++){
            msgsnd(queue,&snd_print,sizeof(snd_print.mtext),0);
        }

    }
}

void kill_children(int queue, int command,int n_tree){
    Msg_packet snd_kill;
    //Setto il tipo al livello superiore e il testo a c (= create)
  

    for(int i=MAX_LEVEL;i>=command;i--){
        snd_kill.mtype=i;
        strcpy(snd_kill.mtext,"k");
        for(int j=0;j<figli_livello[i][n_tree];j++){
            msgsnd(queue,&snd_kill,sizeof(snd_kill.mtext),0);
        }
        figli_livello[i][n_tree]=0;
    }
}

int main(int argc,char** argv){

    char command[MAX];

    key_t queuekey = ftok("/tmp/unique1",1);
    int queue1 = msgget(queuekey, 0777 | IPC_CREAT);
    queuekey = ftok("/tmp/unique1",1);
    int queue2 = msgget(queuekey, 0777 | IPC_CREAT);

    int queue=0;
    int n_tree;


    while(1){
        sleep(1); //Avoid graphic glithces
        printf("\nNext command: ");
        fgets(command,MAX,stdin); fflush(stdin);

        //Decido in che albero sono
        if(command[0]=='1'){
            queue=queue1;
            n_tree=1;
        }
        else if(command[0]=='2'){
            queue=queue2;
            n_tree=2;
        }
        else{
            printf("%sSintassi errata [level_command_depth]\n%s",RED,DF);
            continue;
        }

        switch (command[1]){
            case 'c':
                command_level=atoi(command+2);
                if(command_level==1){
                    figli_livello[1][n_tree]++;
                    printf("Creating child at level 1\n");
                    isChild=fork();
                }
                else if(command_level>1 && command_level<MAX_LEVEL){
                    printf("Creating child at level %d\n",command_level);
                    creat_children(command_level,queue,n_tree);
                }
                else{
                    printf("%s IL NUMERO DI LIVELLO TROPPO GRANDE\n%s",RED,DF);
                }
                break;
            case 'p':
                print_children(queue,n_tree);
                break;
            case 'k':
                command_level=atoi(command+2);
                kill_children(queue,command_level,n_tree);
                break;
            case 'q':
                printf("terminating...\n");
                kill_children(queue1,0,1);
                kill_children(queue2,0,2);
                exit(0);
            default:
                continue;
                break;
        }
        
        if(isChild==0){
            printf("I'm new child at level 1 whith id = %d\n",getpid());
            my_level=1;
            myParent=getppid();
            
            break;
        }
        isChild=1;
    }

    while(1){
        Msg_packet rcv;
        int tmp;
        msgrcv(queue,&rcv,sizeof(rcv.mtext),my_level,0);
        switch(rcv.mtext[0]){
            case 'c':
                tmp=my_level+1;
                isChild=fork();
                
                if(isChild==0){                   
                    my_level=tmp;
                    myParent=getppid();
                    printf("I'm new child at level %d whith id = %d\n",my_level,getpid());
                }
                break;
            case 'p':
                for(int i=1;i<my_level;i++){
                    printf("\t");
                }
                printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,my_level,DF);
                break;
            case 'k':
                exit(0);
                break;

            default: break;
        }
    }
    
        
    
    
}