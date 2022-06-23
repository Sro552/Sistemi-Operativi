//DEFAULT
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
#include <errno.h>
#include <sys/msg.h>
#include <errno.h>  
#include <pthread.h>

int main(int argc, char **argv){
    
}

//BECOME GROUP LEADER
setpgid(0, getpid());

//se pid è un mitt noto restituisce l'indice corrispondente, 
//altrimenti inserisce il nuovo pid nell'array e restituisce l'indice corrispondente

int n_proc = 0;
int ind(int pid){ 
    for(int i = 0; i < n_proc; ++i){
        if(processes[i] == pid){
            return i;
        }
    }
    return -1;
}


//PIPE FIFO
char *nomeFifo = "/tmp/nomeFifo";   //Importante il tmp senno non funzia
    
    unlink(nomeFifo);  //Elimino se esiste già
    mkfifo(fifoMaster, 0666);    //Creo la fifo

    fifoFD = open(nomeFifo[i], O_WRONLY | O_RDONLY | O_RDWR);   //Apro

    write(fifoFD, str, strlen(str));    //Scrivo 

    int r = read(fifoFD,buf,MAX); //Leggo
    if(r>0){
        buf[r] = 0;
    }

//SIGACTION con info 
struct sigaction s;

void handler(int signo, siginfo_t *info, void *empty){
    ... //info->si_pid è il pid del mandante del segnale    
}

void setSigaction(){
    s.sa_sigaction = &handler;
    s.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &s, NULL);
    sigaction(SIGUSR2, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
    sigaction(SIGINT, &s, NULL);
}

siginfo_t {
               int      si_signo;     /* Signal number */
               int      si_errno;     /* An errno value */
               int      si_code;      /* Signal code */
               int      si_trapno;    /* Trap number that caused
                                         hardware-generated signal
                                         (unused on most architectures) */
               pid_t    si_pid;       /* Sending process ID */
               uid_t    si_uid;       /* Real user ID of sending process */
               int      si_status;    /* Exit value or signal */
               clock_t  si_utime;     /* User time consumed */
               clock_t  si_stime;     /* System time consumed */
               union sigval si_value; /* Signal value */
               int      si_int;       /* POSIX.1b signal */
               void    *si_ptr;       /* POSIX.1b signal */
               int      si_overrun;   /* Timer overrun count;
                                         POSIX.1b timers */
               int      si_timerid;   /* Timer ID; POSIX.1b timers */
               void    *si_addr;      /* Memory location which caused fault */
               long     si_band;      /* Band event (was int in
                                         glibc 2.3.2 and earlier) */
               int      si_fd;        /* File descriptor */
               short    si_addr_lsb;  /* Least significant bit of address
                                         (since Linux 2.6.32) */
               void    *si_lower;     /* Lower bound when address violation
                                         occurred (since Linux 3.19) */
               void    *si_upper;     /* Upper bound when address violation
                                         occurred (since Linux 3.19) */
               int      si_pkey;      /* Protection key on PTE that caused
                                         fault (since Linux 4.6) */
               void    *si_call_addr; /* Address of system call instruction
                                         (since Linux 3.5) */
               int      si_syscall;   /* Number of attempted system call
                                         (since Linux 3.5) */
               unsigned int si_arch;  /* Architecture of attempted system call
                                         (since Linux 3.5) */
}

//creo n processi figli e instauro una comunicazione bidirezionale tramite due pipe anonime tra padre
//e figlio
//da pipe.c (es_lab_os/comunicazione_pipe_anonime)

for(int i = 1; i <= n; ++i){    
    pipe(inputPipe[i]);
    pipe(outputPipe[i]);
    isChild = fork();

    if(isChild != 0){ //master
        PIDs[i] = isChild;
        close(inputPipe[i][READ]);
        close(outputPipe[i][WRITE]);
    }else{ //figlio
        mio_index = i;
        printf("Creando figlio %d...\n", mio_index);
        close(inputPipe[mio_index][WRITE]);
        close(outputPipe[mio_index][READ]);
        break;       //fondamentale
    }
}

//handler SIGINT che termina tutti i figli

void signalIntHandler(int signo){
    if(getpid() != pidPadre){
        return;
    }

    for(int i = 0; i < totChilds; i++){
        kill(childrenList[i], SIGTERM);
    }
    
    while(wait(NULL) > 0);

    printf("Ho terminato\n");

    exit(0);
}

//creo un file log.txt dove scrivere le operazioni effettuate

openedFile = open("log.txt", O_CREAT|O_RDWR|O_APPEND,S_IRUSR|S_IWUSR);
strcat(wr, "Sono il thread generato dal segnale numero ");
sprintf(str, "%d", segnale);
strcat(wr, str);
strcat(wr, "\nSto inviando un messaggio al figlio\n");
write(openedFile, wr, strlen(wr));
close(openedFile);


    //Scrittura con file descriptor
    int openedFile=open("tmp/log.txt",O_CREAT|O_RDWR|O_APPEND,S_IRUSR|S_IWUSR); //Apro il file
    write(openedFile,messaggio,strlen(messaggio));
    close(openedFile);

    //Lettura con file descriptor
    int openedFile=open("tmp/log.txt",O_RDONLY); //Apro il file
    if(errno!=0){   //Check errori
        return 1;
    }
    read(openedFile,buf,MAX); //leggo
    close(openedFile);


    //scrittura con streams
    FILE *ptr = fopen("/tmp/log.txt", "a"); //a=write at end 
    fprintf(ptr, messaggio);
    fclose(ptr);

    //Lettura con streams

    FILE *ptr = fopen(fileName, "r");
    char mex[MAX];

    while (!feof(ptr))
    {
        fscanf(ptr,"%s",mex);
    }
    fclose(ptr);


//creare un thread
pthread_create(&tid, NULL, myfunc, NULL);

//funzione che conta caratteri (count_c) e righe (count_l)

void count(char *buf){
    while(buf[count_c] != '\0'){
        if(l == 1 && buf[count_c] == '\n'){
            ++count_l;
        }
        ++count_c;
    }
}

//QUEUE

typedef struct msg {
    long mtype;
    char mtext[MAX];
} msg;

void message_queue(int pid[]) {
    //Creo il messaggio da inviare nella queue
    msg msg_send, msg_rec;
    msg_send.mtype = 1;
    sprintf(msg_send.mtext, "%d", pid[0]);

    //Creo il path per la queue
    strcpy(path_queue, path);
    strcat(path_queue, "/key.txt");


    key = ftok(path_queue, 32);  //Chiave della queue
    msgctl(key, IPC_RMID, NULL);  //Cancello se esiste già
    queueID = msgget(key, 0777);  //Prendo ID
    msgsnd(queueID, &msg_send, sizeof(msg_send.mtext), 0);  //Invio il messaggio
    msgrcv(queueID, &msg_rec, sizeof(msg_send.mtext), 1, 0); //Riceve il messaggio di tipo 1
}

//CREAZIONE CARTELLE E FILE
    mkdir(path, 0755);  //Creazione cartelle
    if (errno == 2) {   //Controllo se il path dato da input è corretto
        return 10;
    }
    creat(path_file, 0777); //Creazione file

    //Funz per sapere se esiste la cartella
    int folderExists(char *folder)
    {
        char current_working_dr[MAX];
        getcwd(current_working_dr, MAX);

        int result = (chdir(folder) == 0) ? 1 : 0;

        chdir(current_working_dr);

        return result;
    }

//ESERCIZI CHRI

// 1. Stampa “T” (per True) o “F” (per False) a seconda che il valore rappresenti un file o cartella
// esistente
//DATA è una variabile locale
if [[ -f $DATA ]]; then echo True; else echo False; fi

// 2. Stampa “file”, “cartella” o “?” a seconda che il valore rappresenti un file (esistente), una
// cartella (esistente) o una voce non presente nel file-system
//DATA è una variabile locale
if [[ -f $DATA ]]; then echo file; elif [[ -d $DATA ]]; then echo cartella;
else echo ?; fi

// 3. Stampa il risultato di una semplice operazione aritmetica (es: ‘1 < 2’) contenuta nel file
// indicato dal valore di DATA, oppure “?” se il file non esiste
//DATA è una variabile locale
if [[ -f $DATA ]]; then if [[ $(cat $DATA) ]]; then echo True; else echo
False; fi; else echo ?; fi

// 4. Scrivere uno script che dato un qualunque numero di argomenti li restituisca in output in
// ordine inverso.
//bash ./es4.sh 1 2 3 4
lista = ()
while [[ $1 != "" ]]; do
    lista[${#lista[@]}]=$1
    shift
done
for((i=${#lista[@]}-1; i>=0; i--)); do
    echo ${lista[$i]}
done

// 5. Scrivere uno script che mostri il contenuto della cartella corrente in ordine inverso rispetto
// all’output generato da “ls” (che si può usare ma senza opzioni)
//bash ./es5.sh
lista=($(ls))
for((i=${#lista[@]}-1; i>=0; i--)); do
    echo ${lista[$i]}
done

// 6. Creare un makefile con una regola help di default che mostri una nota informativa, una
// regola backup che crei un backup di una cartella appendendo “.bak” al nome e una restore
// che ripristini il contenuto originale. Per definire la cartella sorgente passarne il nome come
// variabile, ad esempio: make -f mf-backup FOLDER=... (la variabile FOLDER è disponibile
// dentro il makefile)
# make help
# make FOLDER="test" backup
# make FOLDER="test" restore

help:
    @echo "Nota informativa"

backup:
    @cp -r $FOLDER "$(basename ${FOLDER}).back"

restore:
    @rm -r $FOLDER
    @mv "$(basename ${FOLDER}).back" $FOLDER

// 7. creare un “alias” in bash denominato “feedback” che se invocato attende dall’utente un input
proponendo il prompt “Come ti chiami?” e rispondendo con “Ciao !” (dove è l’input immesso)
// senza però sovrascrivere o impostare alcuna nuova variabile nella shell attiva.
alias feedback="read -p \"Come ti chiami? \" var && echo Ciao $var"

// 8. creare un “alias” in bash denominato “somma” che legge un numero (intero con segno) alla
// volta (numero+INVIO, numero+INVIO, …) e alla fine (immissione vuota premendo solo INVIO)
// stampa la somma dei numeri inseriti.
alias somma='sum=0 && while true; do read var; if [[ -z ${var} ]]; then echo
$sum; break; else sum="$sum + $var"; fi; done'


// 9. Fare un programmino in C “charcount” che accetta come parametro un carattere e ne conta
// il numero di occorrenze nel flusso in ingresso. Ad esempio con ls -alh /tmp | ./charcount “x”
// devono essere contate le “x” presenti. OPZIONALMENTE: aggiungere la ristampa dei dati in
// ingresso mostrando il carattere colorato.
#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char** argv){
    if(argc!=2)
    return -1;
    char c1 = argv[1][0];
    char c2;
    int count = 0;
    while((c2 = getchar()) != EOF){
    if(c1==c2){
        printf("\033[0;32m%c\033[0;0m",c2);
        count++;
    }else{
        printf("%c",c2);
    }
    printf("\nIl carattere %c compare %d volte\n",c1,count);
    }
}

// 10. Scrivere un’applicazione che data una stringa come argomento ne stampa a video la 
// lunghezza, ad esempio: ./lengthof "Questa frase ha 28 caratteri" deve restituire a video il
// numero 28.
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if(argc==1)
        return -1;
    
    int len=0;
    while(argv[1][len] != '\0')
        len++;
    
    printf("%d\n",len);
}

// 11. Scrivere un’applicazione che definisce una lista di argomenti validi e legge quelli passati alla
// chiamata verificandoli e memorizzando le opzioni corrette, restituendo un errore in caso di
// un’opzione non valida.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define MAX_PARAMS 4
typedef char bool;
const char * valid[] = {"-h", "-m", "-n", "--help"};
//Return TRUE is parameter is valid, FALSE otherwise
bool isValid(char * arg){
    for (int i = 0; i < 4; i++) { // Cycle through all of the valid parameters
    //Compare the i-th element of the valid list with the parameter
    if (!strcmp(arg, valid[i])) {
        return TRUE;
    }
}
return FALSE;
}

int main(int argc, char ** argv){
        
    char * paramList[MAX_PARAMS]; //Define list of stored parameters
    int ind = 0;
    for (int i = 1; i < argc; i++) { //Cycle through parameters
        if (isValid(argv[i])) { //Check if parameter is valid
            paramList[ind++] = argv[i]; //Save valid parameter
        } else {
            fprintf(stdout, "'%s' is a invalid parameter\n", argv[i]);
            exit(2);
        }
    }
    //Print list of recognised parameters
    for (int i = 0; i < ind; i++) {
        printf("%s\n", paramList[i]);
    }
    return 0;
}


// 12. Realizzare funzioni per stringhe char stringrev(char str) (inverte ordine caratteri) e int
// stringpos(*char str, char chr) (cerca chr in str e restituisce la posizione)
#include <stdio.h>
char* stringrev(char *str){
    int len=0;
    while(str[len]!='\0')
        len++;
    char nuovo_array[len];
    for(int i=len-1; i>=0; i--)
        nuovo_array[len-1-i] = str[i];
    for(int i=0; i<len; i++)
        str[i] = nuovo_array[i];
    return str;
}
int stringpos(char *str, char chr){
int len=0;
while(str[len]!='\0')
    len++;
for(int i=0; i<len; i++){
    if(str[i] == chr)
    return i;
}
return -1;
}

int main(int argc, char** argv){
    switch(argc)
    {
    case 0:
        return -1;
        break;
    case 1:
        printf("%s\n",stringrev(argv[0]));
        break;
    case 2:
        printf("%s\n",stringrev(argv[1]));
        break;
    case 3:
        printf("%s\n",stringpos(argv[1],argv[2][0]));
        break;
    }
}

// 13. Avendo come argomenti dei “binari”, si eseguono con exec ciascuno in un sottoprocesso
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char** argv){
    for(int i=1; i<argc; i++){
        if(!fork()){
            char *args[] = {NULL};
            execvp(argv[i],args);
        }
    }
}

