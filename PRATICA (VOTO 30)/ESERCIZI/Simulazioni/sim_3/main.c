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

#define MAX 100

//Strutture 
typedef struct msg {
    long mtype;
    char mtext[MAX];
} msg;

//Variabili
char path[MAX];
int n_child;
key_t key;
int queueID;
char path_queue[MAX];


int controlli_input(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "%sPARSMETRI ERRATI ./app <PATH> <N>%s\n", RED, DF);
        return 1;
    } else if (atoi(argv[2]) == 0) {
        fprintf(stderr, "%sPARSMETRI ERRATI ./app <PATH> <N>%s\n", RED, DF);
        return 1;
    }
    return 0;
}

void terminatorHandler(int signo) {
    kill(0, SIGTERM);
    while(wait(NULL)>0);
    exit(0);
}

void crea_file(int pid[]) {
    char path_file[MAX]; //conterrà il path per la creazione dei file
    char str_key[MAX]; //Stringa da scrivere su key.txt
    char tmp[MAX]; //Variabile di appoggio per la creazione

    //Creo il file path/key.txt
    strcpy(path_file, path);
    strcat(path_file, "/key.txt");
    creat(path_file, 0777);

    sprintf(str_key, "%d", pid[0]); //salvo pid padre
    strcat(str_key, "\n");

    for (int i = 1; i < n_child + 1; i++) {
        sprintf(path_file, "%d", pid[i]);
        strcat(str_key, path_file);

        //creazione file path/PID.txt
        strcpy(tmp, path_file);
        strcpy(path_file, path);
        strcat(path_file, "/");
        strcat(path_file, tmp);
        strcat(path_file, ".txt");
        creat(path_file, 0777);
    }

    //Scrittura su path/key.txt dei PID dei figli
    strcpy(path_file, path);
    strcat(path_file, "/key.txt");
    int openFile = open(path_file, O_CREAT | O_RDWR);
    write(openFile, str_key, strlen(str_key));
    close(openFile);
}

void stampa_stdout(int pid[]) {
    char out[MAX]; //Stringa da scrivere su stdout
    char tmp1[MAX]; //Stringa di appoggio

    strcpy(out, "");
    //Concateno i PID
    for (int i = 1; i < n_child + 1; i++) {
        sprintf(tmp1, "%d", pid[i]);
        strcat(out, tmp1);
        strcat(out, " ");
        strcpy(tmp1, "");
    }

    strcat(out, "\n");
    fflush(stdout);
    write(1, out, strlen(out)); //Stampo su stdout
}

void message_queue(int pid[]) {
    //Creo il messaggio da inviare nella queue
    msg msg_send;
    msg_send.mtype = 1;
    sprintf(msg_send.mtext, "%d", pid[0]);

    //Creo il path per la queue
    strcpy(path_queue, path);
    strcat(path_queue, "/key.txt");


    key = ftok(path_queue, 32);  //Chiave della queue
    msgctl(key, IPC_RMID, NULL);  //Cancello se esiste già
    queueID = msgget(key, 0777);  //Prendo ID
    msgsnd(queueID, &msg_send, sizeof(msg_send.mtext), 0);  //Invio il messaggio

}

void SIGUSR1_handler(int signo) {
    char pid_path[MAX];  //Contiene il path del file con nome PID.txt
    char str_pid[MAX];   //Contiene il PID convertito in stringa

    //Creo il path
    strcpy(pid_path, path);
    sprintf(str_pid, "%d", getpid());
    strcat(pid_path, "/");
    strcat(pid_path, str_pid);
    strcat(pid_path, ".txt");

    //Apro e scrivo sul file
    int openFile1 = open(pid_path, O_RDWR | O_APPEND);
    write(openFile1, "SIGUSR1\n", strlen("SIGUSR1\n"));
    close(openFile1);
}

void SIGUSR2_handler(int signo) {
    //Creo il messaggio da inviare sulla queue
    msg msg_send2;
    msg_send2.mtype = 1;
    sprintf(msg_send2.mtext, "%d", getpid());

    //Scrivo il messaggio nella queue creata in precedenza
    msgsnd(queueID, &msg_send2, sizeof(msg_send2.mtext), 0);
}

int main(int argc, char **argv) {
    //Imposto segnale di terminazione
    signal(SIGINT, terminatorHandler);

    //Faccio i controlli dell'input
    if (controlli_input(argc, argv)) {
        return 10;
    }

    //Creo la cartella info dato il path da input 
    strcpy(path, argv[1]);
    strcat(path, "/info");
    mkdir(path, 0755);
    if (errno == 2) {   //Controllo se il path dato da input è corretto
        fprintf(stderr, "%sPERCORSO INSERITO ERRATO ./app <PATH> <N>%s\n", RED, DF);
        return 10;
    }


    n_child = atoi(argv[2]); //Numero di figli da creare
    int pid[n_child + 1];  //Tengo traccia di tutti i pid dei figli

    pid[0] = getpid();  //Father PID

    //Creo i figli
    for (int i = 1; i < n_child + 1; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            //PARTE 4 gestione segnale SIGUSR1
            signal(SIGUSR1, SIGUSR1_handler);
            //PARTE 5 gestione segnale SIGUSR2
            signal(SIGUSR2, SIGUSR2_handler);
            break;
        }
    }

    //Blocco i figli
    if (getpid() != pid[0]) {
        while (1) {
            sleep(1);
        }
    }

    //PARTE 1 creo i file
    crea_file(pid);

    //PARTE 2 stampo su stdout
    stampa_stdout(pid);

    //PARTE 3 message_queue
    message_queue(pid);

    //Blocco il Padre
    while (1) {
        sleep(1);
    }

    return 0;
}