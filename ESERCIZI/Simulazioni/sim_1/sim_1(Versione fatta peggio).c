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
#include <pthread.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define YELLOW "\x1b[33m"

#define MAX_FIGLI 5
#define MAX 100
#define RD 0
#define WR 1
#define TRUE 1
#define FALSE 0

//VARIABILI GLOBALI
int pipe1[MAX_FIGLI][2];   //Per pipe anonime
char buf[MAX];  //Buffer input da tastiera
pthread_t t_id;
char msg_saved[MAX];
int pid_padre;
int tot_figli=0;
int mypos;
int pid_children[MAX_FIGLI];
int openedFile;
char messaggio[MAX];
char tmp[MAX];
int segnale;
char mesg[MAX];

//FUNZIONI
//Print error
void perr(char* str){
    printf("%s%s%s\n",RED,str,DF);
}

void *new_child(void *param){
    
    
    openedFile=open("tmp/log.txt",O_CREAT|O_RDWR|O_APPEND,S_IRUSR|S_IWUSR);
    strcpy(messaggio,RED);
    strcat(messaggio,"[THREAD] I'm a new thread generated by signal ");
    sprintf(tmp,"%d",segnale);
    strcat(messaggio,tmp);
    strcat(messaggio,"\n[THREAD] Sending message to children \n");
    write(openedFile,messaggio,strlen(messaggio));
    close(openedFile);
    for(int i=0;i<tot_figli;i++){
        
        
        sprintf(tmp,"%s%d%s%d","To child ",i," PID ",pid_children[i]);
        strcat(tmp,": ");
        strcat(tmp,msg_saved);
        write(pipe1[i][WR],tmp,sizeof(tmp));
    }
    tot_figli=0;
    openedFile=open("tmp/log.txt",O_CREAT|O_RDWR|O_APPEND,S_IRUSR|S_IWUSR);
    strcpy(messaggio,RED);
    strcat(messaggio,"[THREAD] Children reset\n");
    strcat(messaggio,"[THREAD] Termination thread \n");
    write(openedFile,messaggio,strlen(messaggio));
    close(openedFile);  
    return (void *)3;
}

void handler(int signo){
    segnale = signo;
    pthread_create(&t_id,NULL,new_child,NULL);
}

int main(int argc,char** argv){
    //Stampo il PID del Processo principale
    mkdir("tmp",0755);
    //creat("tmp/log.txt",0755);
    printf("%s[MAIN] my id is %d%s\n",GREEN,getpid(),DF);
    pid_padre=getpid();

    //set pipe
    // for(int i=0;i<MAX_FIGLI;i++){
    //     pipe(pipe1[i]);
    // }

    signal(SIGUSR1,handler);
    signal(SIGUSR2,handler);

    while(1){
        //fflush(stdout);
        //leggo input
        fgets(buf,MAX,stdin); fflush(stdin);

        if(atoi(buf)>0){    //Se è un numero
            if(tot_figli<MAX_FIGLI){
                pipe(pipe1[tot_figli]);
                int isChild=fork();
                if(isChild==0){
                    mypos=tot_figli;
                    printf("%s[CHD] I'm a new childwith id %d, and I'm waiting message from father%s\n",YELLOW,getpid(),DF);
                    break;
                }
                else{
                    close(pipe1[tot_figli][RD]);
                }
                pid_children[tot_figli]=isChild;
                tot_figli++;
            }
            else{
                perr("[MAIN] Error troppi figli");
            }
        }
        else{
            //strcpy(msg_saved,"");  
            strcpy(msg_saved,buf); 
            //msg_saved[strlen(buf)]='\0';  

            
            printf("%s[MAIN] Msg saved %s%s\n",GREEN,msg_saved,DF);  
        }
        sleep(1);
    }
    //Blocco i figli
    

    close(pipe1[mypos][WR]);
    while(1){
        fflush(stdout);
        fflush(stdin);
        read(pipe1[mypos][RD],&mesg,MAX);
        printf("sono qua\n");
        printf("%s%s%s\n",YELLOW,mesg,DF);
        exit(1);
    }
}