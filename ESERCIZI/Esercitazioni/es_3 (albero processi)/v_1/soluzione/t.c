/**
Creare un’applicazione in C che gestisca un albero di processi tramite dei segnali. 
In particolare, il programma, una volta lanciato, deve accettare i seguenti comandi:
- cn 🡪 crea nuovi figli al livello n
- kn 🡪 termina i figli al livello n
- p 🡪 stampa in output l’albero dei processi
- q 🡪 termina il processo dopo aver terminato tutti i figli
L’intera comunicazione deve avvenire esclusivamente tramite segnali inviati dal processo principale. 
L’output del comando ‘p’ non deve essere ordinato ma deve essere ben chiaro il livello di ogni processo 
(per esempio usando la tabulazione).




 * The tree is managed through N master processes, where N is the maximum depth.
 * Each master process is a group leader and each children created at depth M will
 * enter the group of master process M. This allows the main process to issue signal
 * only at a certain depth. 
 * NB: the kill command will leave hanging branches (gaps in the tree)
 * */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_ARG 10
#define MAX_DEPTH 5 //Number of levels
#define MAX_BROADTH 10  //Number of children at the same depth
#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

int createFlag = 0;
int isChild = 0;
pid_t myParent = 0;

int depth = 0, broadth = 0;

int br[MAX_DEPTH+1]; //Master processes

// Error printing function
void perr(char * str){
    fprintf(stderr,"%s?%s%s",RED,str,DF);
}

//Tabulation printing function
void printTab(){
    for(int i = 1; i <= depth ; i++){
        printf("\t");
    }
}

//Child creation function
int createChild(short level){
    // Issue a SIGUSR1 to instruct the children at level to create a new child
    return kill(-br[level-1],SIGUSR1); //Target the group of the master
}

// Handler to initiate the creation of the child
void signalHandlerCreate(int signo){
    // Interrupt while(1) loop by setting the flag for the creation of the child. See end of main
    createFlag = 1;
}

// Signal all the children to print to stdout their info
void signalPrintHandler(int signo){
    printTab();
    printf("%s[ID %d - Parent: %d - Group %d] depth %d%s\n",GREEN,getpid(),myParent,getpgid(0),depth,DF);
}

//Signal handler to terminate the process
void signalTermHandlerChild(int signo){
    printf("[Child terminating]\n");
    exit(0); //This will leave a hanging branch
}
//Signal handler for the master process
void signalIntHandlerMasterParent(int signo){
    while(wait(NULL)>0);
    printf("[Terminating master parent]\n");
    exit(0);
}

//Quit function
void quit(int signo){
    for(int i = 1; i<=MAX_DEPTH; i++){
        kill(-br[i],SIGTERM); //Terminate children
        kill(br[i],SIGINT); //Terminate master parent
    }
    
    while(wait(NULL)>0);
    exit(0);
}

//Kill children at a certain depth
int killChildren(int level){
    //Issue SIGTERM to all children at level depth
    printf("Killing children at level %d\n",level);
    return kill(-br[level],SIGTERM);
}

//PRint entire tree
void printTree(){
    for(int i = 0; i<MAX_DEPTH; i++){
        //Use SIGUSR2 to signal print
        kill(-br[i],SIGUSR2);
    }
}

int main(){
    char buffer[MAX_ARG]; //Buffer for input parameters
    int level = 0; //Default level to create child
    
    //Initiate the master parent: each child at depth n will belong to the group of Master Parent br[n].
    br[0] = getpid(); //Set current process as Master parent level 0
    
    //Create all of the master parents
    for(int i = 1; i <= MAX_DEPTH; i++){
        br[i] = fork();
        if(br[i]==0) break; 
    }
    
    // Each Master parent becomes group leader
    setpgid(0,getpid());
    
    

    //Block all master parents
    if(br[0] != getpid()){
        signal(SIGTERM,SIG_IGN); //Ignore TERM since its used to terminate children
        signal(SIGINT,signalIntHandlerMasterParent); // Set SIGINT as signal to terminate master parents
        while(1){
            sleep(1);
        }; //Block all master parents
    }

    printf("Master processes = ");
    for(int i = 0; i<MAX_DEPTH; i++){
        printf("[%d] ",br[i]);
    }fflush(stdout);
    
    //Assign handlers for the various events.
    signal(SIGUSR2,signalPrintHandler);
    signal(SIGUSR1,signalHandlerCreate);
    //signal(SIGINT,quit);

    //Enter infinite loops for parsing commands
    while(1){
        sleep(1); //Delay to avoid graphic glitches
        printf("\nNext command: ");fflush(stdout);

        //Fetch command from STDIN
        int r = read(STDIN_FILENO, buffer, MAX_ARG-1);
        buffer[r] = 0; //Terminate string
        level = atoi(buffer+1); //Get n parameter
        //Parse command
        switch(buffer[0]){
            case 'c': //Create child
                if(level == 1){ //Create immediate child
                    if(broadth < MAX_BROADTH){
                        printf("Creating child at level %d\n",level);
                        broadth++;
                        isChild = !fork();
                    }else{  
                        perr("Too many children\n");
                    }
                }else if(level > 1 && level <= MAX_DEPTH){ //Issue creation of grandchild
                    printf("Creating grandchild at level %d\n",level);
                    createChild(level);
                }else{
                    perr("Invalid parameter\n");
                }
                break;
            case 'k': //Kill child/grandchild
                //Kill children by issuing the termination signal
                if(level > -1 && level <= MAX_DEPTH)
                    killChildren(level);
                else
                    perr("Invalid parameter\n");
                break;
            case 'p':
                printf("Printing Tree:\n");
                printTree();
                break;
            case 'q':
                printf("Terminating...\n");
                quit(0);
                exit(0);
            default:
                perr("Invalid parameter\n");
        }
        if(isChild){ //If it's the first child
            signal(SIGTERM,signalTermHandlerChild); //Set termination handler
            myParent = getppid(); //Keep track of who is the parent
            depth++; //Increase depth at which the child is 
            broadth = 0; // Set broadth of own branch 
            setpgid(0,br[depth]); //Join group of master parent
            printf("I'm new child at level %d with id = %d\n",depth,getpid());
            break;
        }
    }
    //Child
    signal(SIGINT,SIG_IGN);
    while(1){
        
        if(createFlag){ //This flag is set by the signal handler!
            if(broadth < MAX_BROADTH){ //Create new child if allowed
                broadth++;
                if (!fork()){ //Child that is created
                    depth++; //Increase own depth
                    broadth=0; //Initialise broadth
                    myParent = getppid(); //Keep track of parent
                    setpgid(0,br[depth]); // Join master parent group
                    printf("I'm new child at level %d with group = %d\n",depth,getpgid(0));
                }
            }else{
                perr("Too many children\n");
            }
            //Reset create flag
            createFlag = 0;
        }
    }
    
}