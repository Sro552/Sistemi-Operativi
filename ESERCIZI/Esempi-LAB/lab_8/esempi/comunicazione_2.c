#include <sys/types.h> 
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h> //ipcBook.c
typedef struct book{
    char title[10];
    char description[200];
    short chapters;
} Book;
    struct msg_buffer{
    long mtype;
    Book mtext;
} msgp_snd, msgp_rcv; //Two different message buffers

int main(void){
    msgp_snd.mtype = 20;
    strcpy(msgp_snd.mtext.title,"Title");
    strcpy(msgp_snd.mtext.description,"This is a description");
    msgp_snd.mtext.chapters = 17;
    key_t queue1Key = ftok("prova.txt", 1);
    int queueId = msgget(queue1Key , 0777 | IPC_CREAT);
    int esito = msgsnd(queueId , &msgp_snd, sizeof(msgp_snd.mtext),0);
    esito = msgrcv(queueId , &msgp_rcv, sizeof(msgp_rcv.mtext),20,0);
    printf("Received: %s %s %d\n",msgp_rcv.mtext.title,
    msgp_rcv.mtext.description, msgp_rcv.mtext.chapters);
}