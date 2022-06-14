#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define PATH_MAXLEN 60
#define CHILDREN_MAX 10
#define PIDLEN 8

char info[PATH_MAXLEN];
char key[PATH_MAXLEN];
char file[PATH_MAXLEN];

key_t queueKey;
int queueId;

typedef struct Msg
{
    long mtype;
    char text[PATH_MAXLEN];
} Msg;

void send_msg()
{
    Msg msg;
    memset(msg.text, '\0', PATH_MAXLEN);
    sprintf(msg.text, "%d", getpid());
    msg.mtype = 1;

    msgsnd(queueId, &msg, sizeof(msg.text), 0);
}

int folderExists(char *folder)
{
    char current_working_dr[PATH_MAXLEN];
    getcwd(current_working_dr, PATH_MAXLEN);

    int result = (chdir(folder) == 0) ? 1 : 0;

    chdir(current_working_dr);

    return result;
}

void handler(int sigNum)
{
    if (sigNum == SIGUSR1)
    {
        FILE *ptr = fopen(file, "a");
        fprintf(ptr, "SIGUSR1\n");
        fclose(ptr);
    }
    else if (sigNum == SIGUSR2)
    {
        send_msg();
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Parametri errati\n");
        return 1;
    }

    char path[PATH_MAXLEN];
    memset(path, '\0', PATH_MAXLEN);
    strcpy(path, argv[1]);

    int n;

    if (!sscanf(argv[2], "%d", &n))
    {
        fprintf(stderr, "'%s' non e' un numero valido\n", argv[2]);
        return 2;
    }

    if (!folderExists(path))
    {
        fprintf(stderr, "La cartella '%s' non esiste\n", path);
        return 3;
    }

    if (n > CHILDREN_MAX)
    {
        fprintf(stderr, "Numero massimo di figli consentiti: %d\n", CHILDREN_MAX);
        return 4;
    }

    memset(info, '\0', PATH_MAXLEN);
    strcpy(info, path);

    if (path[strlen(path) - 1] != '/')
        strcat(info, "/");

    strcat(info, "info/");
    mkdir(info, 0755);

    memset(key, '\0', PATH_MAXLEN);
    strcpy(key, info);
    strcat(key, "key.txt");
    creat(key, 0755);

    char pid[PIDLEN];
    memset(pid, '\0', PIDLEN);
    sprintf(pid, "%d", getpid());

    FILE *ptr = fopen(key, "w");
    fprintf(ptr, "%s\n", pid);
    fclose(ptr);

    queueKey = ftok(key, 32);
    queueId = msgget(queueKey, 0777);

    if (queueId != -1)
        msgctl(queueId, IPC_RMID, NULL);

    queueId = msgget(queueKey, 0777 | IPC_CREAT);
    send_msg();

    for (int i = 0; i < n; i++)
    {
        if (!fork())
        {
            signal(SIGUSR1, handler);

            memset(file, '\0', PATH_MAXLEN);
            strcpy(file, info);

            memset(pid, '\0', PIDLEN);
            sprintf(pid, "%d", getpid());

            strcat(file, pid);
            strcat(file, ".txt");

            creat(file, 0755);

            printf("%d%c", getpid(), (i == n - 1) ? '\n' : ' ');
            fflush(stdout);

            while (1)
                ;
        }
    }

    return 0;
}