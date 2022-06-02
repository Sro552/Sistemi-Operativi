/*****
 * In the following code we will create a single message queue to handle the communication
 * between the various children. In particular we'll use the message type to send
 * messages at different depths. The 'kill' message will be recursive, where each 
 * process that receives it will send it to its children and then wait for their 
 * termination before terminating itself.
 * NB: differently from the signal-based one, this program terminates all the children
 * of a killed child --> cannot have hanging branches.
 * */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define MAX_ARG 10
#define MAX_DEPTH 5
#define MAX_BROADTH 10
#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

int createFlag=0;
int isChild = 0;
pid_t myParent = 0;

int depth = 0, broadth = 0;
int treeStruct[MAX_DEPTH+1]; //Keep track of how many children are at each level
pid_t children[MAX_BROADTH]; //Store pid of children

//Message struct
typedef struct msg{
    long mtype;
    char mtext[2];
} Msg_packet;


// Error printing function
void perr(char * str){
    fprintf(stderr,"%s%s%s",RED,str,DF);
}

//Tabulation printing function
void printTab(){
    for(int i = 1; i <= depth ; i++){
        printf("\t");
    }
}

//Child creation function
int createChild(int queue,int level,int * treeStruct){
    //Create Creation message
    Msg_packet msgCreate;
    //The creation of children at level N must be initiated by children at level N-1
    msgCreate.mtype=level-1; 
    strncpy(msgCreate.mtext,"c",2);

    //Send enough messages for all children
    for(int j = 0; j < treeStruct[level-1];j++){
        msgsnd(queue,&msgCreate,sizeof(msgCreate.mtext),0);
        //Update tree struct
        treeStruct[level]++;
    }
    return 0;
}

//Print tree
void printTree(int queue,int * treeStruct){
    printTab();
    printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,depth,DF);
    
    //Define print message
    Msg_packet msgPrint;
    strncpy(msgPrint.mtext,"p",2);
    int counter = 0;
    //Send enough messages for every children
    for(int i = 1; i<=MAX_DEPTH;i++){
        //Send message to each depth
        msgPrint.mtype=i;

        //At each level send enough messages for the number of processes at that level
        for(int j = 0; j < treeStruct[i];j++){
            msgsnd(queue,&msgPrint,sizeof(msgPrint.mtext),0);
            counter++;
        }
    }
}

//Kill children
void killChildren(int queue, int level, int * treeStruct){
    Msg_packet msgKill;
    msgKill.mtype=level;
    strncpy(msgKill.mtext,"k",2);
    
    //Send message to each children of that level which, in turn, will notify its children
    for(int j = 0; j < treeStruct[level];j++){
        msgsnd(queue,&msgKill,sizeof(msgKill.mtext),0);
    }
    for(int x = level; x <= MAX_DEPTH; x++){
        treeStruct[x]=0; //Reset the various depth counters
    }
    
}


//Quit function
void quit(int queue, int * treeStruct){
    killChildren(queue,1,treeStruct);
    printf("%sSent quit message to all on %d. Waiting for their termination%s\n",GREEN,queue,DF);
    while(wait(NULL)>0);
    printf("Terminating program\n");
    exit(0);
}

int main(void){
    char buffer[MAX_ARG]; //Buffer for input parameters
    int level = 0; //Tmp variable

    //Create file for queue
    creat("/tmp/tree", 0777);
    key_t treeKey1 = ftok("/tmp/tree",1);
    
    //Remove queue if already exists
    int treeeQueue = msgget(treeKey1,0777|IPC_CREAT);
    msgctl(treeeQueue,IPC_RMID,NULL);
    treeeQueue = msgget(treeKey1,0777|IPC_CREAT);
    
    //Enter infinite loops for parsing commands
    while(1){
        sleep(1); //Avoid graphic glithces
        printf("\nNext command: ");fflush(stdout);

        //Fetch command from STDIN
        int r = read(STDIN_FILENO, buffer, MAX_ARG-1);
        buffer[r] = 0; //Terminate string
        
            

        //Parse command
        switch(buffer[0]){

            //Creation of children at a certain level
            case 'c': 
                level = atoi(buffer+1); //Get level of new child to be created
                
                //Create immediate child if space is available
                if(level == 1){ 
                    if(broadth < MAX_BROADTH){
                        printf("Creating child at level %d\n",level);
                        broadth++;
                        treeStruct[level]++; //Update tree structure
                        isChild = fork();
                        if(isChild != 0){
                            children[broadth-1]=isChild; //Keep track of children ID
                        }
                    }else{  
                        perr("Too many children\n");
                    }
                }else if(level > 1 && level <= MAX_DEPTH){ //Issue creation of grandchild
                    printf("Creating grandchild at level %d\n",level);
                    createChild(treeeQueue,level,treeStruct);
                }else{
                    perr("Invalid parameter\n");
                    continue;
                }
                break;
            case 'k': //Kill child/grandchild
                level = atoi(buffer+1); //Get level of child
                
                //Kill children
                if(level > -1 && level <= MAX_DEPTH)
                    killChildren(treeeQueue,level,treeStruct);
                else
                    perr("Invalid parameter\n");
                break;
            case 'p':
                printf("Printing Tree:\n");
                printTree(treeeQueue,treeStruct);
                break;
            case 'q':
                quit(treeeQueue,treeStruct);
            default:
                perr("Invalid parameter\n");
        }
        if(isChild == 0){ //If it's a level 1 child
            myParent = getppid(); //Keep track of who is the parent
            depth++; //Increase depth at which the child is 
            broadth = 0; // Set broadth of its children branch to 0 (number of children)
            printf("I'm new child at level %d with id = %d\n",depth,getpid());
            break; //Exit while loop to process messages
        }
    }

    //Child waiting for messages
    Msg_packet msgRcv;
    while(1){
        //Hang for next message at a specific depth (message type)
        msgrcv(treeeQueue,&msgRcv,sizeof(msgRcv.mtext),depth,0);

        //Check message text
        if(strcmp(msgRcv.mtext,"c")==0){ //Create child
            if(broadth++ < MAX_BROADTH){ //Create new child if allowed
                children[broadth-1] = fork(); // Keep track of child id
                if (children[broadth-1] == 0){ //Child that is created
                    depth++; //Increase own depth
                    broadth=0; //Initialise broadth
                    myParent = getppid(); //Keep track of parent
                    printf("I'm new child at level %d with id = %d\n",depth,getpid());
                }
            }else{
                perr("Too many children\n");
            }
        }else if(strcmp(msgRcv.mtext,"k")==0){ //Kill
            //Send termination enough termination message (for each child)
            Msg_packet msgKill;
            //Send message at a further depth
            msgKill.mtype=depth+1;
            strncpy(msgKill.mtext,"k",2);

            //Send enough messages (NB: these messages might be read by other non children --> 
            // not a problem since other messages will be received )
            for(int i = 0; i<broadth; i++){
                msgsnd(treeeQueue,&msgKill,sizeof(msgKill.mtext),0);
            }
            while(wait(NULL)>0); //Wait for the recursion to end
            printf("[%d] Terminating\n",getpid()); 
            exit(0);
        }else if(strcmp(msgRcv.mtext,"p")==0){ //Print
            printTab();
            printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,depth,DF);
        }else{
            printf("[%d]Invalid message read '%s'\n",getpid(),msgRcv.mtext);
        }
        sleep(1); //Prevent process from reading additional messages
    }
    
}