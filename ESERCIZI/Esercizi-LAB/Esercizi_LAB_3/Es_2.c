/*Scrivere un’applicazione che definisce una lista di argomenti validi e legge quelli passati alla chiamata
verificandoli e memorizzando le opzioni corrette, restituendo un errore in caso di un’opzione non valida. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define N_PARAMETRI 4

//Lista di comandi validi
const char* valid[] = {"-h" , "-m" , "-n" , "--help"}; 

//Funzione per vedere se il comando è valido
int isValid(char* str){
    for(int i=0 ; i < N_PARAMETRI ; i++){
        if(!strcmp(valid[i],str))
            return TRUE;
    }
    return FALSE;
}

int main(int argc, char** argv){
    char* paramValidi[N_PARAMETRI];  //Variabile che conterrà i parametri validi
    int indice = 0; //Variabile per l'indice dei parametri

    //Controllo parametri
    for(int i=1 ; i < argc ; i++){
        if(isValid(argv[i])){
            paramValidi[indice] = argv[i];
            indice++;
        }
        else{
            fprintf(stdout,"'%s' is a invalid parameter\n",argv[i]);
            exit(2); 
        }
    }

    //Stampa parametri validi
    for(int i=0; i < indice ; i++){
        printf("%s\n",paramValidi[i]);
    }

    return 0;
}