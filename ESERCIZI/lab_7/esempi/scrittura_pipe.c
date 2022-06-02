#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
extern int errno; //writePipe.c
void handler(int signo){
    printf("SIGPIPE received\n"); perror("Error"); exit(errno);
}
int main(void){
    signal(SIGPIPE,handler);
    int fd[2]; char buf[50];
    int esito = pipe(fd); //Create unnamed pipe
    close(fd[0]); //Close read side
    printf("Attempting write\n");
    write(fd[1],"writing",8);
    printf("I've written something\n");
}