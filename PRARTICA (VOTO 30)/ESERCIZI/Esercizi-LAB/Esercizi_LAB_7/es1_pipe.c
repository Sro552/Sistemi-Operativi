#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define READ 0
#define WRITE 1

//Variabile globale
char buf[50];

int main(int argc,char** argv){
    //Creazione pipe
    int pipe1[2]; int pipe2[2];  
    pipe(pipe1); pipe(pipe2);

    if(!fork()){
        //sistemp le pipe
        close(pipe1[READ]);
        close(pipe2[WRITE]);

        while(1){
            //leggo da console e poi scrivo nella pipe
            printf("P1: "); fflush(stdout);
            read(0,buf,50);
            //fflush(stdin);

            write(pipe1[WRITE],buf,50);

            if(buf[0]=='\n')
                break;

            //leggo dalla pipe e stampo
            read(pipe2[READ],&buf,50);
            printf("-> P1: %s",buf);

            if(buf[0]=='\n')
                break;
        }
        close(pipe1[WRITE]);
        close(pipe2[READ]);

    }else{
        //Replico tutto
        close(pipe1[WRITE]);
        close(pipe2[READ]);

        while(1){
            read(pipe1[READ],&buf,50);
            printf("-> P2: %s",buf); fflush(stdout);

            if(buf[0]=='\n')
                break;

            printf("P2: ");fflush(stdout);
            read(0,buf,50);
            //fflush(stdin);

            write(pipe2[WRITE],buf,50);

            if(buf[0]=='\n')
                break;

        }

        close(pipe1[READ]);
        close(pipe2[WRITE]);
    }

    while(wait(NULL)>0);

    return 0;
}
