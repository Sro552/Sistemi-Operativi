#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define READ 0
#define WRITE 1

int main(int argc,char** argv){
    int pipe1[2]; int pipe2[2]; char buf[50];
    pipe(pipe1); pipe(pipe2);
    int p2 = !fork();

    if(p2){

        close(pipe1[READ]);
        close(pipe2[WRITE]);

        while(1){
            printf("P1: ");
            fgets(buf,50,stdin);
            fflush(stdin);

            write(pipe1[WRITE],buf,50);

            if(buf[0]=='\n')
                break;

            read(pipe2[READ],&buf,50);
            printf("-> P1: %s",buf);

            if(buf[0]=='\n')
                break;
        }
        close(pipe1[WRITE]);
        close(pipe2[READ]);

    }else{
        close(pipe1[WRITE]);
        close(pipe2[READ]);

        while(1){
            read(pipe1[READ],&buf,50);
            printf("-> P2: %s",buf);

            if(buf[0]=='\n')
                break;

            printf("P2: ");
            fgets(buf,50,stdin);
            fflush(stdin);

            write(pipe2[WRITE],buf,50);

            if(buf[0]=='\n')
                break;

        }

        close(pipe1[READ]);
        close(pipe2[WRITE]);
    }
}
