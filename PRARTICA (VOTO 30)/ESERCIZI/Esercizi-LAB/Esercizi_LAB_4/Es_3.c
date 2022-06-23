/*
Dati due eseguibili come argomenti del tipo ls e wc si eseguono in due processi
distinti: il primo deve generare uno stdout redirezionato su un file temporaneo,
mentre il secondo deve essere lanciato solo quando il primo ha finito leggendo lo
stesso file come stdin.
Ad esempio ./main ls wc deve avere lo stesso effetto di ls | wc.
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

//Funzioni
int errori_input(int argc,char **argv){
    if(argc!=3){
        printf("Errore numero di parametri errato\n");
        return 1;
    }
    return 0;
}

int main(int argc, char ** argv) {
    //Check input error
    if(errori_input(argc,argv)){
        return 1;
    }
    char* argList[2];//variable for argumento to execvp

    //file temporaneo
    int openFile = open("temp.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); //Create temporary file


    if(!fork()){
        argList[0]=argv[1];
        argList[1]=NULL;
        //rediriziono su stdout
        dup2(openFile,1);
        execvp(argv[1],argList);
    }
    else{
        while(wait(NULL)>0);

        argList[0]=argv[1];
        argList[1]=NULL;
        //redireziono su stdin
        dup2(openFile,0);
        lseek(openFile, 0, SEEK_SET); //Reset I/O pnt
        execvp(argv[2],argList);
    }
    return 0;
}