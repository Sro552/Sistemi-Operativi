#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h> //errSig.c
extern int errno; // declare external global variable
int main(void){
    int sys = kill(3443,SIGUSR1); //Send signal to non existing proc
    if (sys == -1) { //something went wrong!
        fprintf(stderr, "errno = %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr,"Strerror: %s\n", strerror(errno));
    } else {
        printf("Signal sent\n");
    }
}