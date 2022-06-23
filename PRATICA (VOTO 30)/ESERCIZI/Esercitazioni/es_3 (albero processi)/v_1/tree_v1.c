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

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

#define MAX_LEVEL 5
#define MAX 100
#define TRUE 1
#define FALSE 0


//Variabili globali
int level = 1;    //Livello piu alto arrivato
int command_level;  //Livello preso da console
int my_level;   //Livello del processo
int isChild = 1;
int myParent;   //PID padre
int master[MAX_LEVEL];  //PID dei master
int create = FALSE;   //Bool per poter creare o no

//HANDLER
//Terminazione child
void signalTermHandlerChild(int signo) {
    printf("[Child terminating]\n");
    exit(0); //This will leave a hanging branch
}

//Signal handler for the master process
void eliminateMaster_handler(int signo) {
    while (wait(NULL) > 0);
    printf("[Terminating master parent]\n");
    exit(0);
}

//Create Handler setto create = true
void create_handler(int signo) {
    create = TRUE;
}

//Stampo l'albero
void print_handler(int signo) {
    for (int i = 1; i < my_level; i++) {
        printf("\t");
    }
    printf("%s[ID %d - Parent: %d - Group %d] depth %d%s\n", GREEN, getpid(), myParent, getpgid(0), my_level, DF);
}

//Funzioni
//Quit function
void quit(int signo) {
    //Termino tutti i processi
    for (int i = 1; i <= MAX_LEVEL; i++) {
        kill(-master[i], SIGTERM); //Terminate children
        kill(master[i], SIGINT); //Terminate master parent
    }

    while (wait(NULL) > 0);
    exit(0);
}

//Funzione create mando segnale con SIGUSR1
void creat_children(int command) {
    kill(-master[command - 1], SIGUSR1);
}

//Funzione per stampare
void print_children() {
    for (int i = 1; i < MAX_LEVEL; i++) {
        kill(-master[i], SIGUSR2);
    }

}

//Funzione per killare
void kill_children(int command) {
    kill(-master[command], SIGTERM); //Terminate children
}


int main(int argc, char **argv) {

    char command[MAX];

    //Padre supremo
    master[0] = getpid();

    //Creo i master
    for (int i = 1; i < MAX_LEVEL; i++) {
        master[i] = fork();
        if (master[i] == 0) {
            break;
        }
    }

    //Rendo i master capogruppo
    setpgid(0, getpid());


    //Imposto i master e li blocco
    if (getpid() != master[0]) {
        signal(SIGTERM, SIG_IGN); //Ignoro SIGTERM perchè sara il segnale che userp per eliminare i figli
        signal(SIGINT, eliminateMaster_handler); // Setto SIGINT come segnale per eliminare i master
        while (1) {
            sleep(1);
        }
    }

    //stampo i master per comodità
    printf("Master processes = ");
    for (int i = 0; i < MAX_LEVEL; i++) {
        printf("[%d] ", master[i]);
    }
    fflush(stdout);

    //setto i segnali per creare e stampare
    signal(SIGUSR1, create_handler);
    signal(SIGUSR2, print_handler);


    while (1) {
        sleep(1); //Avoid graphic glithces
        printf("\nNext command: ");
        fgets(command, MAX, stdin);
        fflush(stdin);


        switch (command[0]) {

            case 'c': //create
                command_level = atoi(command + 1);
                printf("\n\n%d\n\n", command_level);

                if (command_level == 1) {   //Se promo livello creo subito il figlio
                    printf("Creating child at level 1\n");
                    isChild = fork();
                    if (level == 1)
                        level++;
                } else if (command_level > 1 && command_level <= level) {   //Senno mando segnale
                    if (command_level == level)
                        level++;

                    printf("Creating child at level %d\n", command_level);
                    //richiamo la funzione che manderà il segnale al gruppo
                    creat_children(command_level);
                } else {
                    printf("%s IL NUMERO DI LIVELLO TROPPO GRANDE\n%s", RED, DF);
                }
                break;
            case 'p'://print
                print_children();
                break;
            case 'k'://kill
                command_level = atoi(command + 1);
                if (command_level == (level - 1)) {
                    level--;
                    command_level = atoi(command + 1);
                    kill_children(command_level);
                } else {
                    printf("%sSPIAZE DEVI ELIMINARE DAL LIVELLO PIU BASSO%s\n", RED, DF);
                }

                break;
            case 'q':
                printf("terminating...\n");
                quit(0);
                exit(0);
            default:
                break;
        }
        if (isChild == 0) {
            signal(SIGTERM, signalTermHandlerChild); //Set termination handler
            printf("I'm new child at level 1 whith id = %d\n", getpid());
            my_level = 1;
            myParent = getppid();
            setpgid(0, master[1]);
            isChild = 1;
            break;
        }
    }
    signal(SIGINT, SIG_IGN);

    while (1) {

        int tmp = 0;

        if (create) { //Creo figlio

            tmp = my_level + 1;
            isChild = fork();

            if (isChild == 0) {
                my_level = tmp;
                myParent = getppid();
                setpgid(0, master[my_level]);
                printf("I'm new child at level %d whith id = %d\n", my_level, getpid());
            }
            create = FALSE;
        }
    }


}