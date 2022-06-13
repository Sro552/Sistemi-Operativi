#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

#define MAX_LEVEL 5
#define MAX 100
#define TRUE 1
#define FALSE 0

//VARIABILI GLOBALI
int pid_leader[MAX_LEVEL]; //Array contenente i pid dei processi leader
int mylevel = 0;  //Mio livello di profondità nell'albero
int create = FALSE;   //Bool per sapere se posso procreare
char buf[MAX];  //Buffer per input da tastiera
int command;
int isChild;
int myParent; //PID parent
int level_top = 1; //Tengo il livello piu alto


//FUNZIONI
//Stampa in rosso per gli errori
void perr(char *str) {
    printf("%s%s%s\n", RED, str, DF);
}

//SIGTERM for children terminate child
void terminationChild_handler(int signo) {
    printf("%sTerminate child pid: %d%s\n", RED, getpid(), DF);
    exit(0);
}

//SIGINT for leader terminate leader
void terminationLeader_handler(int signo) {
    //printf("sium\n");
    while (wait(NULL) > 0);  //Wait to terminate all process child
    printf("%sTerminate leader pid: %d%s\n", RED, getpid(), DF);
    exit(0);
}

//Create children manda il segnale al gruppo scelto
void create_child() {
    kill(-pid_leader[command - 1], SIGUSR1);
}

//Create handler SIGUSR1 set bool create=TRUE
void create_handler(int signo) {
    create = TRUE;
}

//Print a child
void print_handler(int signo) {
    for (int i = 1; i < mylevel; i++) {
        printf("\t");   //TAB
    }
    printf("%s[ID %d - Parent: %d ]depth %d%s\n", GREEN, getpid(), myParent, mylevel, DF);
}

//Print all tree with signal SIGUSR2
void print_tree() {
    for (int i = 1; i < MAX_LEVEL; i++) {
        kill(-pid_leader[i], SIGUSR2);
    }
}

//Quit all process
void quit() {
    for (int i = 1; i < MAX_LEVEL; i++) {
        kill(-pid_leader[i], SIGTERM);
        kill(pid_leader[i], SIGINT);
    }
    while (wait(NULL) > 0);
    exit(1);
}

int main(int argc, char **argv) {
    //Set all process leader 
    pid_leader[0] = getpid();
    for (int i = 1; i < MAX_LEVEL; i++) {
        pid_leader[i] = fork();   //Save pid of process leader
        if (pid_leader[i] == 0) {
            setpgid(0, getpid()); //Become group leader
            signal(SIGTERM, SIG_IGN);    //Ignora SIGTERM
            signal(SIGINT, terminationLeader_handler);   //SIGINT terminate
            //Block process leader
            while (1) {
                sleep(1);
            }
        }
    }

    signal(SIGUSR1, create_handler); //Set SIGUSR1=create_handler
    signal(SIGUSR2, print_handler); //Set SIGUSR2=print_handler

    //Block principal process
    if (getpid() == pid_leader[0]) {
        while (1) {
            //Prendo il comando da tastiera
            printf("Next command: ");
            fflush(stdout);
            read(0, buf, MAX);
            isChild = 1;
            //Controllo input
            switch (buf[0]) {
                case 'c'://Create
                    command = atoi(buf + 1); //prendo il numero del livello
                    if (command == 1) {   //Se livello 1 creo subito 
                        printf("Creating child at level 1\n");
                        isChild = fork();
                        if (isChild == 0) {
                            mylevel = command;
                            myParent = getppid(); //Save parent 
                            setpgid(0, pid_leader[mylevel]); //set process group
                            signal(SIGTERM, terminationChild_handler);   //Set SIGTERM to terminate process
                            printf("I'm new childat level 1 whith id = %d\n", getpid());
                        }
                    } else if (command > 1 && command < MAX_LEVEL) {    //Se sono dal livello 2 in giu
                        create_child();
                    } else {
                        perr("Errore number of level");
                    }
                    break;
                case 'k':
                    command = atoi(buf + 1); //prendo il numero del livello
                    if (command >= 1 && command < MAX_LEVEL) {    //Se sono dal livello 2 in giu
                        for (int i = MAX_LEVEL - 1; i >= command; i--) {  //termino i processi da command in su
                            kill(-pid_leader[i], SIGTERM);
                        }
                    } else {
                        perr("Errore number of level");
                    }
                    break;
                case 'p':   //Print
                    print_tree();
                    break;
                case 'q':   //Quit
                    quit();
                    break;
                default:
                    break;
            }

            //Esco se sono il figlio appena creato
            if (isChild == 0) {
                break;
            }
            sleep(1);
        }
    }

    //Blocco i figli
    while (1) {
        int tmp = 0;
        if (create) { //Se create=1 creo bro
            tmp = mylevel + 1;
            isChild = fork();
            if (isChild == 0) {
                printf("Creating child at level %d\n", tmp);
                mylevel = tmp;
                myParent = getppid();
                setpgid(0, pid_leader[mylevel]);
                printf("I'm new childat level %d whith id = %d\n", mylevel, getpid());
            }
            create = FALSE;
        }
    }
}