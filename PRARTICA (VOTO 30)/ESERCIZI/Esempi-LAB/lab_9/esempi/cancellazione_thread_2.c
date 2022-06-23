#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> //thCancel2.c
#include <fcntl.h>
int tmp = 0;
void * my_fun(void * param){
pthread_setcanceltype(*(int *)param,NULL); // Change type
for (long unsigned i = 0; i < (0x9FFF0000); i++); //just wait
tmp++;
open("prova.txt",O_RDONLY); //Cancellation point!
}
int main(int argc, char ** argv){ //call program with ‘async’ or ‘defer’
    pthread_t t_id1; int arg;
    if(strcmp(argv[1],"async")) 
        arg = PTHREAD_CANCEL_ASYNCHRONOUS;
    else if(strcmp(argv[1],"defer")) 
        arg = PTHREAD_CANCEL_DEFERRED;
    pthread_create(&t_id1, NULL, my_fun, (void *)&arg); sleep(1); //Create
    pthread_cancel(t_id1); sleep(5); //Cancel
    printf("Tmp %d\n",tmp);
}