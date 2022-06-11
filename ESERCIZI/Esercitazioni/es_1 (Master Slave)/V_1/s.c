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

char dest[MAX];
char msg[MAX];
char tmp[MAX];


int main(int argc,char** argv){
    printf("Creato slave: %s\n",argv[1]);
    char *pipe_name = "/tmp/fifo";
    char pipe_dest[MAX]="";
    int fd=0; char fifoname[15];
    strcpy(fifoname,"");
    strcat(fifoname,pipe_name);
    strcat(fifoname,argv[1]);
    printf("%s\n",fifoname);
    char input[MAX];
    
    while(1){ 
        mkfifo(fifoname,S_IRUSR|S_IWUSR);
        fd = open(fifoname,O_RDONLY);
        read(fd,input,MAX);
        close(fd);  
        if(input[0]=='M')
        {
            //divisione_stringa(input);
            sscanf(input,"%s %s %s",tmp,dest,msg);
            
            printf("[%s]Sent '%s' to %s\n",argv[1],msg,dest);
            
            strcpy(pipe_dest,pipe_name);
            strcat(pipe_dest,dest);

            mkfifo(pipe_dest,S_IRUSR|S_IWUSR);
            fd = open( pipe_dest, O_WRONLY);
            write(fd,msg,strlen(msg)+1);
            close(fd);
        }
        else{
             printf("[%s]Ricevuto '%s'\n\n",argv[1],input);      
        }
            
        sleep(1);
    }
    
}