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

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define TRUE 1
#define FALSE 0

#define MAX 100

//VARIABILI GLOBALI
int isRighe=FALSE;
int isCaratteri=FALSE;
int isFile=FALSE;
char nomeFile[MAX];
char buf[MAX*100];
char out[MAX];
char err[MAX];

void perr(char* str){
    printf("%s%s%s\n",RED,str,DF);
}

int errori_input(int argc,char** argv){
    if(argc>4){
        return 1;
    }

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-c")==0){
            isCaratteri=TRUE;
        }
        else if(strcmp(argv[i],"-l")==0){
            isRighe=TRUE;
        }
        else{
            isFile=TRUE;
            strcpy(nomeFile,argv[i]);
        }
    }

    return ((isCaratteri+isRighe)==0);
}

void wc_magic(){
    char tmp[MAX];
    int caratteri=0;
    int righe=0;
    while(buf[caratteri]!='\0'){
        if(buf[caratteri]=='\n')
            righe++;
        caratteri++;
    }

    strcpy(out,"");

    if(isCaratteri){
        sprintf(tmp,"%d",caratteri);
        strcat(out,tmp);
    }
    if(isRighe){
        sprintf(tmp,"%d",righe);
        strcat(out,"\t");
        strcat(out,tmp);
    }
    strcat(out,"\t (stdout) |\0");
}

int main(int argc,char **argv){
    strcpy(out,"\t(vuoto stdout) | ");
    strcpy(err,"\t(vuoto stderr) [piping]\n");

    if(errori_input(argc,argv)){
        //perr("ERROR PARAMETERS");
        strcpy(err," ?Opzione errata (stderr) [piping]\n");
        write(1,out,sizeof(out));
        write(2,err,sizeof(err));
        return 1;
    }

    

    if(fork()){
        while(wait(NULL)>0);
        return 0;
    }

    if(isFile){
        int openedFile=open(nomeFile,O_RDONLY);
        if(openedFile==-1){
            strcpy(err," Il file non esiste (stderr) [diretta]\n");
            write(1,out,sizeof(out));
            write(2,err,sizeof(err));
            close(openedFile);
            return 1;
        }
        strcpy(err,"\t(vuoto stderr) [diretta]\n");
        read(openedFile,buf,MAX*100);
        close(openedFile);
    }
    else{
        read(0,buf,MAX*100);
    }
    
    wc_magic();

    write(1,out,sizeof(out));
    write(2,err,sizeof(err));
}