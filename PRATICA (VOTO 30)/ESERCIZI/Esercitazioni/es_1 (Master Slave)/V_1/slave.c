#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX 100
#define PIPE_NAME "/tmp/fifo"  //Name of the slave pipes

//Variabili Globali
char dest[MAX];
char msg[MAX];
char tmp[MAX];


int main(int argc,char** argv){
    printf("Creato slave: %s\n",argv[1]); //Print for check
    char *pipe_name = "/tmp/fifo";      //Base pipe name

    char pipe_dest[MAX]="";
    int fd=0; char fifoname[15];
    char input[MAX];

    //Creazione stringa per pipe
    strcpy(fifoname,"");
    strcat(fifoname,pipe_name);
    strcat(fifoname,argv[1]);
    printf("%s\n",fifoname);
    
    
    while(1){ 
        //Creo la pipe e la apro
        mkfifo(fifoname,S_IRUSR|S_IWUSR);
        fd = open(fifoname,O_RDONLY);
        read(fd,input,MAX);     //Aspetto un messaggio da uno slave o da un master
        printf("%s\n",input);
        close(fd);  

        //Se Master
        if(input[0]=='M')
        {
            sscanf(input,"%s %s %s",tmp,dest,msg); //Divido il messaggio
            
            printf("[%s]Sent '%s' to %s\n",argv[1],msg,dest);
            //Creo stringa pipe destinatario
            strcpy(pipe_dest,pipe_name);
            strcat(pipe_dest,dest);

            //Creo la pipe e invio il messaggio al destinatario
            mkfifo(pipe_dest,S_IRUSR|S_IWUSR);
            fd = open( pipe_dest, O_WRONLY);
            write(fd,msg,strlen(msg)+1);
            close(fd);
        }
        else{//Se slave
             printf("[%s]Ricevuto '%s'\n\n",argv[1],input);      
        }
            
        sleep(1);   //Wait all process
    }
    
}