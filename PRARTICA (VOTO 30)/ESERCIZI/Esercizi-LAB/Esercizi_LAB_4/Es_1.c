// Avendo come argomenti dei “binari”, si eseguono con exec ciascuno in un sottoprocesso (generando un figlio)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

//Funzione per controllare l'input
int errori_input(int argc, char **argv) {
    if(argc==1){ //Controllo se ho inserito almeno un binario
        printf("Errore input inserisci qualche binario!!\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    if (errori_input(argc, argv)) {
        return 1;
    }
    
    //Scorro tutti i binari e li eseguo in un figlio
    for(int i=1;i<argc;i++){
        if(!fork()){
            execv(argv[i],argv);
        }
    }
    
    while(wait(NULL)>0); //Wait for all children
    return 0;
}