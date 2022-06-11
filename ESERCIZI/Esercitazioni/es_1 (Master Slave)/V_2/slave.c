#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <signal.h>


#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"
#define MAX_SLAVES 10
#define MAX 100

//Variabili globali
int n_slave;  //Numero di slave
char masterPipe[MAX]; //Nome base pipe master preso da parametro
char slavePipe[MAX_SLAVES][MAX]; //Nome base pipe slave preso da parametro
int slavePipeFD[MAX_SLAVES]; //File descriptor slave
int mynum; //My slave number
char buff[MAX];  //Buffer per input
char dest[MAX];  //destinatario
char msg[MAX];  //Msg da invire
int canread = 1;  //bool non far ricevere allo stsso processo
int fd_master; //File descriptor del master

//Funzioni
//Funzione per rimanere in ascolto del master
void ascoltoMaster() {
    //Creo nome pipe master 
    char tmpp[MAX];
    sprintf(tmpp, "%d", mynum);
    strcat(masterPipe, tmpp);
    fd_master = open(masterPipe, O_RDONLY); //Apro la pipe in sola lettura

    while (1) {
        read(fd_master, buff, MAX); //Leggo messaggi dal master
        canread = 0;  //Setto canread a 0 cosi non puo leggere il messsggio il thread stesso

        sscanf(buff, "%s %s", dest, msg); //Prendo i parametri
        int n_dest = atoi(dest); //n FD destinatario

        printf("[%d] Sent '%s' to %d\n", mynum, msg, n_dest);

        sleep(1); //Aspetto che il thread stesso non ascolti
        write(slavePipeFD[n_dest], msg, sizeof(msg)); //Invio messaggio

        canread = 1; //lo rimetto in ascolto
    }
    //Chiudo la pipe
    close(fd_master);
}

//Thread in ascolto dello slave
void *ascoltoSlave(void *param) {
    int fd = *(int *) param; //Prendo file descriptor passato da parametro
    char buf[MAX]; //Buffer di input
    printf("%s[%d]Thread for watching FD %d%s\n", RED, mynum, fd, DF);

    while (1) {
        if (canread) { //Controllo se posso leggere
            int r = read(fd, buf, MAX); //leggo msg
            if (r > 0) //Check se è stato inviato qualcosa
                printf("[%d] Reiceved '%s'\n\n", mynum, buf);
        }
    }
}

//SIGTERM
void termHandler(int signo) {
    //Close pipes
    for (int i = 1; i <= MAX_SLAVES; i++) {
        close(slavePipeFD[i - 1]);
    }
    //close(masterPipe);
    printf("[%d]Terminating", mynum);
}

int main(int argc, char **argv) {
    //Signal sigterm per vhiudere file descriptor
    signal(SIGTERM, termHandler);

    mynum = atoi(argv[1]); //My slave number
    n_slave = atoi(argv[2]); //tot slave
    strcpy(masterPipe, argv[3]); //Base nome pipe master

    pthread_t t_ids[MAX_SLAVES]; //Arrays of threads
    pthread_attr_t attr;    //Attribute for thread
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    //Apertura thread per ascolto da slave
    for (int i = 0; i < n_slave; i++) {
        //Creao nome pipe
        char tmp[MAX];
        if (i == mynum) {
            continue;
        } else if (i < mynum) {
            sprintf(tmp, "%d%d", i, mynum);
        } else if (i > mynum) {
            sprintf(tmp, "%d%d", mynum, i);
        }

        strcpy(slavePipe[i], argv[4]);
        strcat(slavePipe[i], tmp);

        //Apro pipe
        slavePipeFD[i] = open(slavePipe[i], O_RDWR | O_NONBLOCK);//O_NONBLOCK non blocca il read
        //Faccio partire il thread
        pthread_create(t_ids + i, NULL, ascoltoSlave, (void *) &slavePipeFD[i]);

    }
    //Sto in ascolto del master
    ascoltoMaster();
}