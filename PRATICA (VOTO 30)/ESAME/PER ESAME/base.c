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
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define YELLOW "\x1b[33m"

#define MAX 100
#define TRUE 1
#define FALSE 0
#define RD 0
#define WR 1

//STRUTTURE UTILI

//VARIABILI GLOBALI

//FUNZIONI
//Print in stderr
void perr(char* str){
    fprintf(stderr,"%s%s%s\n",RED,str,DF);
}

//Check parametri input
int errori_parametri(int argc,char** argv){
    return 0;
}


int main(int argc,char** argv){
    if(errori_parametri(argc,argv)){
        return 1;
    }
}