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
#define MAX 100

//VARIABILI GLOBALI
int n_slave = 0; //Numero di slave passato come parametro
char *masterPipe = "/tmp/pipeMaster"; //Nome base pipe master
char *slavePipe = "/tmp/pipeSlave";  //Nomw base pipe slave
char fifoMaster[MAX_SLAVES][100]; //The names of the pipes used to communicate with slaves
int fifoMasterFD[MAX_SLAVES]; //File descriptors of the pipes used to communicate with slaves
char buf[MAX];  //Buffer per comandi
char op1[MAX];  //Primo operatore (MITTENTE o DESTINATARIO)
char op2[MAX];  //Secondo operatore (MITTENTE o DESTINATARIO)
char msg[MAX];  //Messaggio da inviare
char dir = ' ';  //Direzione puo essere '<' o '>'


//FUNZIONI
//Stampa errori
void perr(char str[]) {
    fprintf(stderr, "%s%s%s\n", RED, str, DF);
}

//Controlli parametri passati
int errori_input(int argc, char **argv) {
    if (argc != 2) {
        perr("PARAMETRI ERRATI ./master.out <n_slave>");
        return 1;
    }
    if (atoi(argv[1]) <= 0 || atoi(argv[1]) > MAX_SLAVES) {
        perr("NUMERO DI SLAVE ERRATO ./master.out <n_slave>");
        return 1;
    }
    return 0;
}

//Creazione delle pipe per master e per gli slave
void crea_pipe() {
    char tmp[MAX]; //Stringa di appoggio

    for (int i = 0; i < n_slave; i++) {
        //CREAZIONE STRINGA PER MASTER
        sprintf(fifoMaster[i], "%s%d", masterPipe, i);
        //Creazione pipe master
        unlink(fifoMaster[i]);
        mkfifo(fifoMaster[i], 0666);

        //Creazione pipe per tutti gli slave 
        //(li creo qua come il prof ma è da giga nabbi)
        for (int j = i + 1; j < n_slave; j++) {
            sprintf(tmp, "%s%d%d", slavePipe, i, j);

            unlink(tmp);
            mkfifo(tmp, 0666);
        }
    }
}

//Controllo input da tastiera 
int check_command(char *command) {
    int i = 0;  //Contatore generale
    int j = 0;  //contatore per secondo num
    int k = 0;  //contatore per messaggio
    int fase = 0;   //==0 primo num ==1 secondo num ==2 messaggio

    //Scorro la stringa
    while (command[i] != '\0') {
        switch (fase) { //controllo fase
            case 0:
                if (command[i] == '<' || command[i] == '>') {
                    fase = 1;  //Passo a fase 2
                    dir = command[i]; //prendo la direzione
                    op1[i] = '\0';  //concludo il primo operatore
                } else {
                    op1[i] = command[i]; //salvo primo operatore
                }
                break;
            case 1:
                if (command[i] == ' ') {
                    fase = 2;  //passo a fase 2
                    op2[j] = '\0';  //concludo il mio secondo operatore
                } else {
                    op2[j] = command[i]; //salvo secondo operatore
                    j++;
                }
                break;
            case 2:
                //Ultima fase continuo a salvare il messaggio
                msg[k] = command[i];
                k++;
                msg[k] = '\0';
                break;
            default:
                break;
        }
        i++;
    }
    //Effettuo controlli di ritorno
    if (strlen(op1) == 0 || strlen(op2) == 0 || strlen(msg) == 0 || strcmp(op1, op2) == 0) {
        return 0; //ritorno errore
    }
    //controllo che atoi funzi
    int source = atoi(op1);
    int op2inatario = atoi(op2);
    if (source < 0 || source >= n_slave || op2inatario < 0 || op2inatario >= n_slave) {
        return 0;
    }
    //Tutto ok!
    return 1;
}

//Quit method
void quit(int signo) {
    printf("\nQuitting programm...\n");

    for (int i = 0; i < MAX_SLAVES; i++) {
        close(fifoMasterFD[i]);
    }

    //Terminate all children
    kill(0, SIGTERM);
    while (wait(NULL) > 0);
    printf("END OF PROGRAM\n");
    exit(0);
}

int main(int argc, char **argv) {
    //Force quit on CTRL+C
    signal(SIGINT, quit);

    //Check params
    if (errori_input(argc, argv)) {
        return 1;
    }

    n_slave = atoi(argv[1]); //Numero di slave passato da parametro

    crea_pipe();  //Create pipe for Master and Children
    printf("%spipe create per la comunicazione%s\n", GREEN, DF);


    char totslave[MAX];
    sprintf(totslave, "%d", n_slave);
    //Eseguo tutti gli slave
    for (int i = 0; i < n_slave; i++) {
        char mynum[MAX]; //Num slave
        sprintf(mynum, "%d", i);
        //Creo parametri per lo slave
        char *listArg[] = {"./slave.out", mynum, totslave, masterPipe, slavePipe, NULL};

        if (!fork()) {
            execv("./slave.out", listArg);//Eseguo lo slave
        }
    }

    //Apro tutte le pipe per i master
    for (int i = 0; i < n_slave; i++) {
        fifoMasterFD[i] = open(fifoMaster[i], O_WRONLY);
    }

    sleep(1);  //Wait for all process

    //Fase di attesa comando per Master
    while (1) {
        //Prelevo comando
        printf("Next command: ");
        fflush(stdout);
        read(0, buf, MAX);
        //Check comando
        if (!check_command(buf)) {
            perr("ERRORI COMANDO <op1>'<''>'<op2> <msg>");
            continue;
        }

        int n_dest = 0; //conterra il n destinatario

        if (dir == '<') {
            n_dest = atoi(op2); //dest è op2
            //Invio op1 msg
            strcat(op1, " ");
            strcat(op1, msg);
            write(fifoMasterFD[n_dest], op1, strlen(op1));
        }
        if (dir == '>') {
            n_dest = atoi(op1); //dest è op1
            //Invio op2 msg
            strcat(op2, " ");
            strcat(op2, msg);
            write(fifoMasterFD[n_dest], op2, strlen(op2));
        }

        sleep(2); //Wait all process
    }
}