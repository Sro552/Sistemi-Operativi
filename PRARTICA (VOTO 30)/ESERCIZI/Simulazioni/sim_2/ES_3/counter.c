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

//Variabili
int isRighe=FALSE;
int isCTot=FALSE;
int isFile=FALSE;
char nomiFile[MAX][MAX];
int nFile=0;
char buf[MAX];
int tot_caratteri=0;
int tot_righe=0;
char err[MAX];
char out[MAX];
char tmp[MAX];

int controlli_input(int argc,char** argv){
    if( argc <= 1 ){
        return TRUE;
    }
    
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-l")==0){
            isRighe=TRUE;
        }
        else if(strcmp(argv[i],"-c")==0){
            isCTot=TRUE;
        }
        else{
            isFile=TRUE;
            strcpy(nomiFile[nFile],argv[i]);
            nFile++;
        }
    }

    if((isCTot+isRighe)>0){
        return FALSE;
    }

    return TRUE;
}

void wc_magic(char buf[]){
    while(buf[tot_caratteri]!='\0'){
        if(buf[tot_caratteri]=='\n' && isRighe){
            tot_righe++;
        }
        tot_caratteri++;    
    }
}

void stampa_finale(){
    strcpy(out," ");  
    if(isRighe){       
        sprintf(tmp,"%d",tot_righe);
        strcpy(out,tmp);
        strcat(out," ");
    }
        

    if(isCTot){
        sprintf(tmp,"%d",tot_caratteri);
        strcat(out,tmp);
        strcat(out," ");
    }

    strcat(out,"\t(stdout) |");

}

int main(int argc,char** argv){

    
    strcpy(err,"\t(vuoto stderr)");

    if(controlli_input(argc,argv)){
        strcpy(out,"\t(vuoto stdout) |");
        strcpy(err,"OPZIONI ERRATE  (stderr)\n");
        write(1,out,sizeof(out));
        write(2,err,sizeof(err));
        return 10;
    }

    if(fork()){
        while(wait(NULL)>0);
        return 0;
    }

    if(!isFile){

        read(0,buf,MAX);
        
        wc_magic(buf);

        stampa_finale();

        strcat(err," [PIPING]\n");
        write(1,out,sizeof(out));
        write(2,err,sizeof(err));

        return 0;
    }
    int openedFile;

    for(int i=0;i<nFile;i++){
        openedFile=open(nomiFile[i],O_RDWR);
        if(errno!=0){
            strcpy(out,"\t(vuoto stdout) |");
            strcpy(err,"FILE INSESISTENTE  (stderr)\n");
            write(1,out,sizeof(out));
            write(2,err,sizeof(err));
            return 10;
        }
        read(openedFile,buf,MAX);
        
        wc_magic(buf);

        stampa_finale();

        strcat(err," [DIRETTA]\n");
        write(1,out,sizeof(out));
        write(2,err,sizeof(err));
    }

    
    return 0;
}