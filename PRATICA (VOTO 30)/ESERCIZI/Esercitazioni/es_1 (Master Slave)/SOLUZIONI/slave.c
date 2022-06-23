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
#define MAX_MSG 500

char fifoMaster[100];
int fifoMasterFD;
char fifoSlave[MAX_SLAVES][100]; //Array of FIFO between slaves
int fifoSlaveFD[MAX_SLAVES]; //Array of FD for the fifo between slaves

int canRead = 1;

int slaveNumber; //Current process number

// Error printing function
void perr(char * str){
    fprintf(stderr,"%s[%d]%s%s\n",RED,slaveNumber,str,DF);
}

//MEthod to receive the messages from the master
void  messageReceiveHandler(){
    //Open master pipe in read
    char buf[MAX_MSG+10],dest[10],msg[MAX_MSG];
    int r,ind,target;
    
    //Keep on receiving messages (until SIGTERM is received)
    while(1){
        r = read(fifoMasterFD,&buf,MAX_MSG-1);
        if (r==0){
            printf("[%d]Nothing to read\n",slaveNumber);
            continue;
        }
        buf[r] = 0;
        ind = 0;
        //Extract target process
        for(int i = 0; i < r; i++){
            if(buf[i] == ' '){
                ind = i;
                break;
            }
        }
        //Store in 'dest' the target node
        strncpy(dest,buf,ind);
        target = atoi(dest);

        //Check if target is valid
        if(target <= 0 || target>MAX_SLAVES){
            printf("[%d]Wrong target '%s'\n",slaveNumber,buf);
            continue;
        }
        strcpy(msg,buf+ind+1);
        //Set read flag to 0 so that this process (sender) doesn't read the message on the thread
        canRead=0;

        //Send message
        write(fifoSlaveFD[target-1],msg,strlen(msg));
        printf("[%d]Sent '%s' to %d\n",slaveNumber,msg,target);
        //Give time to the other process to read message, then reactivate reading
        sleep(1);
        canRead=1;
    }
}

//Thread for incoming messages
void * slaveCommunication(void * param){
    int fd = *(int *)param;
    char buf[MAX_MSG];
    printf("%s[%d]Thread for watching FD %d%s\n",RED,slaveNumber,fd,DF);
    
    //Keep on receiving messages (until SIGTERM)
    while(1){
        if(canRead){
            int r = read(fd,buf,MAX_MSG-1); //Read from pipe
            if(r>0){
                buf[r] = 0;
                printf("[%d]Received '%s'\n",slaveNumber,buf);
            }
        }
    }
}

//SIGTERM
void termHandler(int signo){
    //Close pipes
    for (int i = 1; i <= MAX_SLAVES; i++){
        close(fifoSlaveFD[i-1]);
    }
    close(fifoMasterFD);
    printf("[%d]Terminating",slaveNumber);
}

int main(int argc, char ** argv){
    
    //Check parameters
    if(argc < 5){
        perr("Invalid number of parameters");
        exit(1);
    }

    

    //Get own process number
    slaveNumber = atoi(argv[1]);
    //Get total processes
    int totSlaves = atoi(argv[2]);
    if(slaveNumber <= 0 || totSlaves<0 || totSlaves > MAX_SLAVES || totSlaves < slaveNumber){
        perr("Invalid argument");
        exit(2);
    }
    

    //Craft the fifo names
    char str1[10];
    strcpy(fifoMaster,argv[3]);
    sprintf(str1, "%d", slaveNumber);
    strcat(fifoMaster,str1);

    //Open master pipe for incoming commands
    fifoMasterFD = open(fifoMaster,O_RDONLY);

    signal(SIGTERM,termHandler);

    pthread_t t_ids[MAX_SLAVES]; //Arrays of threads
    pthread_attr_t attr;
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    
    //Compute the pipe names
    for (int i = 1; i <= totSlaves; i++){
        //Craft the various pipe names
        if(i == slaveNumber){
            continue;
        }else if(i<slaveNumber){
            sprintf(str1, "%d%d", i,slaveNumber);
        }else{
            sprintf(str1, "%d%d", slaveNumber, i);
        }
        strcpy(fifoSlave[i-1],argv[4]); //Copy the base fifo name
        strcat(fifoSlave[i-1],str1); //Concat the slave numbers
        //printf("[%d]Using %s\n",slaveNumber,fifoSlave[i-1]);

        //Open fifo in read write mode for slave communication
        fifoSlaveFD[i-1] = open(fifoSlave[i-1],O_RDWR|O_NONBLOCK); //Prevent blocking

        //Create a thread for each pipe, on which the programm will listen for incoming messages
        pthread_create(t_ids+i-1, &attr, slaveCommunication, (void *)&fifoSlaveFD[i-1]);
    }
    
    messageReceiveHandler();

}
        
    
