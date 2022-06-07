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

int main(int argc, char **argv) {
    char cr, ch;
    int count;
    if (argc!=2) {
        fprintf(stderr, "?Usage: %s <char>\n", argv[0]);
        return 1;
    }
    ch = argv[1][0];
    while ((cr = getchar()) != EOF) {
        // if (cr == ch) count++; // short version
        if (cr == ch) { // full version...
            count++;
            printf("\033[0;31m%c\033[0;0m", ch);   //Stampo la lettera colorata
        } else {
            printf("%c", cr);
        }
    }
    printf("\nNumero di %c in stream: %d\n", ch, count);
}