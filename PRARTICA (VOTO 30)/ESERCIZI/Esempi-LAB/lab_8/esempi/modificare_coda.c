#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

int main(void){
struct msqid_ds mod;
int esito = open("prova.txt", O_CREAT,0777);
key_t queue1Key = ftok("prova.txt", 1);
int queueId = msgget(queue1Key , IPC_CREAT|S_IRWXU );
msgctl(queueId,IPC_RMID,NULL);
queueId = msgget(queue1Key , IPC_CREAT | S_IRWXU );
esito = msgctl(queueId,IPC_STAT,&mod); //Get info on queue
printf("Current permission on queue: %d\n",mod.msg_perm.mode);
mod.msg_perm.mode= 0000;
esito = msgctl(queueId,IPC_SET,&mod); //Modify queue
printf("Current permission on queue: %d\n\n",mod.msg_perm.mode);
}