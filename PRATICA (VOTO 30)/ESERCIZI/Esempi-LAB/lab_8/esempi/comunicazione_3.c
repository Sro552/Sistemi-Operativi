#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h> //ipcType.c
#include <stdio.h>
struct msg_buffer{
    long mtype;
    char mtext[100];
} mspg_snd,msgp_rcv; //Two different message buffers

int main(int argc, char ** argv){
    int to_fetch = atoi(argv[0]); //Input to decide which msg to get
    key_t queue1Key = ftok("unique", 1);
    int queueId = msgget(queue1Key , 0777 | IPC_CREAT);
    mspg_snd.mtype = 20;
    strcpy(mspg_snd.mtext,"A message of type 20");
    int esito = msgsnd(queueId , &mspg_snd, sizeof(mspg_snd.mtext),0);
    mspg_snd.mtype = 10; //Re-use the same message
    strcpy(mspg_snd.mtext,"Another message of type 10");
    esito = msgsnd(queueId , &mspg_snd, sizeof(mspg_snd.mtext),0);
    esito = msgrcv(queueId , &msgp_rcv, sizeof(msgp_rcv.mtext),
    to_fetch,0);
    printf("Received: %s\n",msgp_rcv.mtext);
}