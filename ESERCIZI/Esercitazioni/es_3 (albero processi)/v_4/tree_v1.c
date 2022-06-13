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

//Funzioni
void controlli_pipe1(int argc,char** argv){
    
    if(argc!=2){
        printf("%sNumero argomenti errato!!%s\n",RED,DF);
        exit(0);
    }
    int tmp=atoi(argv[1]);
    if(tmp<=0 || tmp>=MAX_FIGLI){
        printf("%sTroppi figli!!%s\n",RED,DF);
        exit(0);
    }
}

void signalIntHandler(int signo){
    //Issue termination signal to all children
    kill(0,SIGTERM);

    //Wait children termination
    while(wait(NULL)>0);
    printf("Terminating");

    //Terminate
    exit(0);
}

int main(int argc,char** argv){
    int isChild;
    int pidPadre=getpid();
    char command[MAX];
    int figlio_dest;
    int mypos=0;
    char mesg[MAX];
    int rett=0;
    int children[MAX_FIGLI];

    controlli_pipe1(argc, argv);
    int n_figli=atoi(argv[1]);

    // pipe1 pipes used by the children to talk with the parent
    int pipe1[n_figli][2];
    
    //pipe2 pipes used by the parent to talk with the childrens
    int pipe2[n_figli][2];

    printf("Creating %d process\n",n_figli);
    //Cycle over the number of procs
    for(int i = 0;i<n_figli; i++){
        //Create pipes
        pipe(pipe1[i]);
        pipe(pipe2[i]);
        isChild = fork();
        if(isChild != 0){ // Ancestor
            children[i]=isChild;
            close(pipe1[i][WR]); //Close writing end of pipe1 pipe
            close(pipe2[i][RD]); //Close read end of pipe2 pipe
        }else{ //Child
            mypos=i;
            close(pipe2[i][WR]); //Close writing end of pipe2 pipe (child's pipe1)
            close(pipe1[i][RD]);  //Close reading end of pipe1 pipe (child's pipe2)
            break; //Break from the for loop if child. Child will have unique 'current' value
            
        }
    }

    if(getpid()==pidPadre){//only ancestor
        signal(SIGINT,signalIntHandler); //Override handler to kill all children
        while(1){
            printf("\nNext command: ");
            fgets(command,MAX,stdin);fflush(stdin);
            
            switch (command[0]){
                case 'r':
                    figlio_dest=atoi(command+1);
                    if(figlio_dest>=0 && figlio_dest<n_figli){
                        write(pipe2[figlio_dest][WR],"r",2);
                        read(pipe1[figlio_dest][RD],&rett,50);
                        printf("Child %d told me: '%d'\n",children[figlio_dest],rett);
                    }
                    else{
                        printf("%sNumero figlio richiesto errato%s",RED,DF);
                    }
                    break;
                case 'i':
                    figlio_dest=atoi(command+1);
                    if(figlio_dest>=0 && figlio_dest<n_figli){
                        write(pipe2[figlio_dest][WR],"i",2);
                        read(pipe1[figlio_dest][RD],&rett,50);
                        printf("Child %d told me: '%d'\n",children[figlio_dest],rett);
                    }
                    else{
                        printf("%sNumero figlio richiesto errato%s",RED,DF);
                    }
                    break;
                case 'q':
                    // Terminate all children and itself
                    signalIntHandler(0);
                    break;
                default:
                    break;
            }
        }
    
    }
    else{
        srand(mypos);   // Initialization, should only be called once.
        while(1){
            read(pipe2[mypos][RD],&mesg,50);
            
            switch(mesg[0]){
                case 'r':
                    printf("%sChild computing random%s\n",GREEN,DF);
                    rett=rand();
                    write(pipe1[mypos][WR],&rett,sizeof(rett));
                break;
                case 'i':
                    printf("%sChild sending own pid%s\n",GREEN,DF);
                    rett=getpid();
                    write(pipe1[mypos][WR],&rett,sizeof(rett));
                break;
            }
        }
    }

}