/*3. Fare un programmino in C “charcount” che accetta come parametro un carattere e
ne conta il numero di occorrenze nel flusso in ingresso. Ad esempio con ls -alh
/tmp | ./charcount “x” devono essere contate le “x” presenti.
OPZIONALMENTE: aggiungere la ristampa dei dati in ingresso mostrando il
carattere colorato.
Per stampare "a colori" sul terminale si possono usare le "sequenze di ESCAPE".
Ad esempio per stampare una parola in rosso si può usare:
printf("\033[0;31m WORD \033[0;0m");
(in questo caso la parola WORD preceduta e seguita da uno spazio)
● \033 è il codice ESC (Escape)
● [0;31m è il codice per impostare i colori
● [0;0m è il codice per eliminare l'impostazione dei colori
Si possono cercare "sequenze ANSI" o "sequenze di Escape" per maggiori dettagli e
formati.*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX 100

//Variabili globali
char buf[MAX];

//Funzioni
int errori_input(int argc,char** argv){
    //Controllo che il parametro sia 1
    if(argc!=2){
        fprintf(stderr,"Errore parametri\n");
        return 1;
    }

    //Controllo che sia un solo carattere
    if(strlen(argv[1])!=1){
        fprintf(stderr,"Errore inserire un char\n");
        return 1;
    }

    //Tutto ok
    return 0;
}

int main(int argc, char **argv) {
    int ricorrenze=0;

    if(errori_input(argc,argv)){
        return 1;
    }

    //leggo l'input e lo metto in buf
    read(0,buf,MAX); fflush(stdin);


    //Conto le occorrenze
    for(int i=0;i<strlen(buf);i++){
        if(buf[i]==argv[1][0]){
            ricorrenze++;
        }
    }
    //Stampo i risultati
    printf("Il carattere %s è presente --> %d volte\n",argv[1],ricorrenze);
}