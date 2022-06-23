/*Scrivere un’applicazione che data una stringa come argomento ne stampa a video la lunghezza, ad esempio:
./lengthof "Questa frase ha 28 caratteri" deve restituire a video il numero 28. */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int code=0;  //Codice di ritorno del main
    int len=0;  //Lunghezza della stringa data
    char *p;

    //Controllo parametri
    if (argc!=2) {
        printf("Usage: %s <string>\n", argv[0]);
        code=2;
    } else {
        p=argv[1];

        //Conto i caratteri
        while (*p !=0 ){
            p++;
            len++;
        }
        
        //Stampo il risultato
        printf("Mio conto--> %d\n", len);

        //verifico stampando anche strlen()
        printf("Verifica con strlen--> %ld\n",strlen(argv[1]));
    }

    //Ritorno code (se == 2 ERRORE)
    return code;
}