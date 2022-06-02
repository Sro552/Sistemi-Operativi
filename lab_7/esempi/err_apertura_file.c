#include <stdio.h>
#include <errno.h>
#include <string.h> //errFile.c
extern int errno; // declare external global variable
int main(void){
    FILE * pf;
    pf = fopen ("nonExistingFile.boh", "rb"); //Try to open file
    if (pf == NULL) { //something went wrong!
        fprintf(stderr, "errno = %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr,"Strerror: %s\n", strerror(errno));
    } else {
        fclose (pf);
    }
}