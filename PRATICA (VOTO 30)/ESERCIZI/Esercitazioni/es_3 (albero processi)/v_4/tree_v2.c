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
#include <time.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

#define MAX_FIGLI 10
#define MAX 100
#define RD 0
#define WR 1
#define RD 0
#define WR 1

//VARIABILI GLOBALI
char buf[MAX];
int command;
int mypos=0;
int rett;
//Funzioni
//Print error
void perr(char* str){
    printf("%s%s%s\n",RED,str,DF);
}

//Check parametri
int errori_input(int argc,char** argv){
    if(argc!=2){
        perr("NUMERO PARAMETRI ERRATO");
        return 1;
    }
    int tmp=atoi(argv[1]);
    if(tmp<=0 || tmp > MAX_FIGLI){
        perr("NUMERO FIGLI ERRATO");
        return 1;
    }
    return 0;
}

int main(int argc,char** argv){
    if(errori_input(argc,argv)){
        return 1;
    }

    //Variabili
    int n_slave=atoi(argv[1]);
    int pipe1[n_slave][2];
    int pipe2[n_slave][2];
    int pid_padre=getpid();
    int pid_child[n_slave];

    //Apro le pipe e creo i figli
    for(int i=0;i<n_slave;i++){
        pipe(pipe1[i]);
        pipe(pipe2[i]);
        int tmp_pid=fork();
        if(tmp_pid==0){
            close(pipe1[i][WR]);
            close(pipe2[i][RD]);
            mypos=i;
            break;
        }
        else{
            close(pipe1[i][RD]);
            close(pipe2[i][WR]);
            pid_child[i]=tmp_pid;
        }
    }

    //Block padre
    if(getpid()==pid_padre){
        while(1){
            //Prendo il comando
            printf("Next command: "); fflush(stdout); read(0,buf,MAX);

            switch(buf[0]){
                case 'i':
                    command=atoi(buf+1);    //Prelevo il numero di figlio
                    if(command>=0 && command<n_slave){    //Check che sia nel range
                        write(pipe1[command][WR],"i",2);    //Invio msg figlio
                        read(pipe2[command][RD],buf,MAX);   //Risposta figlio
                        printf("Child %d told me: '%s'\n",pid_child[command],buf);     //Stampo risposta
                    }
                    else{
                        perr("WRONG COMMAND");
                    }
                    break;
                case 'r':
                    command=atoi(buf+1);
                    if(command>=0 && command<n_slave){    //Check che sia nel range
                        write(pipe1[command][WR],"r",2);
                        read(pipe2[command][RD],&rett,MAX);
                        printf("Child %d told me: '%d'\n",pid_child[command],rett);     //Stampo risposta
                    }
                    else{
                        perr("WRONG COMMAND");
                    }
                    break;
                case 'q':
                    for(int i=0;i<n_slave;i++){
                        write(pipe1[i][WR],"q",2);
                    }
                    while(wait(NULL)>0);
                    exit(1);
                    break;
            }
        }
    }
    srand(mypos);   // Initialization, should only be called once.
    
    //Block figli
    while(1){
        char tmp[MAX];
        read(pipe1[mypos][RD],buf,MAX);
        switch(buf[0]){
            case 'i':
                printf("%sChild sending pid...%s\n",GREEN,DF);
                sprintf(tmp,"%d",getpid());
                write(pipe2[mypos][WR],tmp,sizeof(tmp));
                break;
            case 'r':
                printf("%sChild computing random%s\n",GREEN,DF);
                rett=rand();
                write(pipe2[mypos][WR],&rett,sizeof(rett));
                break;
            case 'q':
                exit(1);
                break;
        }
    }
}