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

//VARIABILI GLOBALI
char op1[MAX];
char op2[MAX]; 
char msg[MAX];
char tmp[MAX];
char direzione=' '; //Direzione '<' || '>'
char input[MAX];    //Input da tastiera

//FUNZIONI
//Controlli parametri
int controlli_input(int argc,char** argv){
    if(argc!=2){ //2 argomenti
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

//Controlli input da tastiera
int control_command(char* command){
    //Reset Stringhe
    strcpy(op1,"");
    strcpy(op2,"");
    strcpy(msg,"");
    int i=0;  //Contatore generale
    int j=0;  //Contatore secondo parametro
    int k=0;  //Contatore messaggio (terzo parametro)
    int fase=0;  //0=op1 1=op2 2=messaggio
    while(command[i]!='\n'){
        switch(fase){
            case 0:
                if(command[i]=='<' || command[i]=='>'){
                    fase=1; //Passo a fase 1
                    direzione=command[i];
                    op1[i]='\0';    //Concludo op1
                    
                }
                else{
                    op1[i]=command[i];  //Salvo op1
                }
                break;
            case 1:
                if(command[i]==' '){
                    fase=2;  //Passo a fase 2
                    op2[j]='\0';    //Concludo op2
                }
                else{
                    op2[j]=command[i]; //Salvo op2
                    j++;
                }
            break;
            case 2:
                msg[k]=command[i];  //Salvo msg
                k++;
                msg[k]='\0';
                break;
            default: break;
        }
        i++;
    }

    //Faccio i controlli finali su quello che ho ottenuto
    if(strlen(op1)==0 || strlen(op2)==0 || strlen(msg)==0 || strcmp(op1,op2)==0){
        printf("%s %s %s\n",op1,op2,msg);
        return 0;
    }
    //Controllo i numeri
    int source=atoi(op1);
    int op2inatario=atoi(op2);
    if(source<0 || source>=MAX_SLAVES || op2inatario<0 || op2inatario>=MAX_SLAVES){      
        return 0;
    }
    return 1;
}

int main(int argc,char** argv){
    int fd; //File descriptor master
    char mess[MAX] = "";
    char pipe_op2[MAX]="";
    char *pipe_name = "/tmp/fifo";  //Base name per pipe

    printf("%scontrolli input\n",GREEN);
    if(controlli_input(argc,argv)){
        exit(2);
    }

    int n_slave=atoi(argv[1]);  //N slave totali

    char * argExec[] = {"./s","0",NULL};    //Argomento per lo slave
    //Creazione slave
    printf("%screazione slave\n",GREEN);
    for(int i=0; i<n_slave;i++){
        sprintf(input, "%d", i);
        argExec[1]=input;
        if(fork()==0)
            execv("./slave.out",argExec);
    }
    printf("%sslave creati\n",GREEN);

    sleep(1);    //Wait all process

    while(1){
        //Prendo il comando
        printf("%sComando--> ",DF); fflush(stdout); read(0,input,MAX);

        //Controllo input da tastiera
        if(!control_command(input)){
            printf("%sWRONG COMMAND\n",RED);
            continue;
        }

        //Controllo direzione
        if(direzione=='>'){
            //op1=mittente   op2=destinatario
            strcpy(pipe_op2,pipe_name);
            strcat(pipe_op2,op1);
            //Creo il messaggio M=Master
            strcpy(mess,"");
            strcat(mess,"M");
            strcat(mess," ");
            strcat(mess,op2);
            strcat(mess," ");
            strcat(mess,msg);

            //Creazione Pipe e invio messaggio al figlio giusto
            mkfifo(pipe_op2,S_IRUSR|S_IWUSR);
            fd = open( pipe_op2, O_WRONLY);

            write(fd,mess,sizeof(mess));
            close(fd);

            sleep(1);  //Wait all process 
        }
        if(direzione=='<'){
            //op2=Mittente   op2=destinatario
            strcpy(pipe_op2,pipe_name);
            strcat(pipe_op2,op2);
            //Creo il messaggio da inviare
            strcpy(mess,"");
            strcat(mess,"M");
            strcat(mess," ");
            strcat(mess,op1);
            strcat(mess," ");
            strcat(mess,msg);

            //Creazione Pipe e invio messaggio al figlio giusto
            mkfifo(pipe_op2,S_IRUSR|S_IWUSR);
            fd = open( pipe_op2, O_WRONLY);

            write(fd,mess,strlen(mess)+1);
            close(fd);
           
            sleep(1);   //Wait all process;
        }
    }

}