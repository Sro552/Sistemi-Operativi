#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define MAX_SLAVES 10
#define MAX_ARGS 600 //Maximum command length
#define MAX_MSG 500 //Maximum message length
#define PIPE_NAME "/tmp/fifo"  //Name of the slave pipes
#define PIPE_MASTER "/tmp/fifoMaster" //Name of the master pipes

#define MAX 100

char mitt[MAX];
char dest[MAX];
char msg[MAX];
char tmp[MAX];
char direzione=' ';


int controlli_input(int argc,char** argv){
    if(argc!=2){
        printf("%sNUMERO ARGOMENTI SBAGLIATO\n",RED);
        return 1;
    }
    int n_slave = atoi(argv[1]);
    if(n_slave>MAX_SLAVES){
        printf("%sTROPPI SLAVE\n",RED);
        return 1;
    }
    return 0;   
}
int control_command(char* command){
    strcpy(mitt,"");
    strcpy(dest,"");
    strcpy(msg,"");
    int i=0;
    int j=0;
    int k=0;
    int fase=0;  //0=mittente 1=destinatario 2=messaggio
    while(command[i]!='\n'){
        switch(fase){
            case 0:
            if(command[i]=='<' || command[i]=='>'){
                fase=1;
                direzione=command[i];
                mitt[i]='\0';
                
            }
            else{
                mitt[i]=command[i];
            }
            break;
            case 1:
            if(command[i]==' '){
                fase=2;
                dest[j]='\0';
            }
            else{
                dest[j]=command[i];
        
                j++;
            }
            break;
            case 2:
            msg[k]=command[i];
            k++;
            msg[k]='\0';
            break;
            default: break;
        }
        i++;
    }
    if(strlen(mitt)==0 || strlen(dest)==0 || strlen(msg)==0){
        return 0;
    }
    int source=atoi(mitt);
    int destinatario=atoi(dest);
    if(source<0 || source>10 || destinatario<0 || destinatario>10){
        
        return 0;
    }

    
    return 1;
}

int main(int argc,char** argv){
    int fd;
    char input[MAX];
    char mess[MAX] = "";
    char pipe_dest[MAX]="";
    char *pipe_name = "/tmp/fifo";

    printf("%scontrolli input\n",GREEN);
    if(controlli_input(argc,argv)){
        exit(2);
    }
    int n_slave=atoi(argv[1]);

    char * argExec[] = {"./s","0",NULL};
    //Creazione slave
    printf("%screazione slave\n",GREEN);
    for(int i=0; i<n_slave;i++){
        sprintf(input, "%d", i);
        argExec[1]=input;
        if(fork()==0)
            execv("./s",argExec);
    }
    printf("%sslave creati\n",GREEN);
    sleep(1);    

    while(1){
        printf("%sComando--> ",DF);
        fgets(input,MAX,stdin);
        fflush(stdin);


        if(!control_command(input)){
            printf("%sWRONG COMMAND\n",RED);
            continue;
        }

        if(direzione=='>'){
            strcpy(pipe_dest,pipe_name);
            strcat(pipe_dest,mitt);

            strcpy(mess,"");
            strcat(mess,"M");
            strcat(mess," ");
            strcat(mess,dest);
            strcat(mess," ");
            strcat(mess,msg);

            //printf("\n%s - %s\n",pipe_dest,mess);

            mkfifo(pipe_dest,S_IRUSR|S_IWUSR);
            fd = open( pipe_dest, O_WRONLY);

            write(fd,mess,strlen(mess)+1);
            close(fd);

            
            sleep(1);
        }
        if(direzione=='<'){
            strcpy(pipe_dest,pipe_name);
            strcat(pipe_dest,dest);

            strcpy(mess,"");
            strcat(mess,"M");
            strcat(mess," ");
            strcat(mess,mitt);
            strcat(mess," ");
            strcat(mess,msg);

            //printf("\n%s - %s\n",pipe_dest,mess);

            mkfifo(pipe_dest,S_IRUSR|S_IWUSR);
            fd = open( pipe_dest, O_WRONLY);

            write(fd,mess,strlen(mess)+1);
            close(fd);
           
            sleep(1);
        }
    }

}