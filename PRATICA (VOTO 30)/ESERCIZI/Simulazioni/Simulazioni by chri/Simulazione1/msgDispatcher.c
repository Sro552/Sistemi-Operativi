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
#include <pthread.h>
#include <sys/wait.h>

#define MAX 100
#define MAX_CHD 5
#define READ 0
#define WRITE 1

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"

char msg[MAX];
int children_count = 0;
int children_pids[MAX_CHD];
int pipes[MAX_CHD][2];

int father = -1;

void *send_msg(void *param)
{
    int sigNum = *(int *)param;

    FILE *ptr = fopen("/tmp/log.txt", "a");
    fprintf(ptr, "%s[THREAD] I'm a new thread generated by signal %d%s\n", RED, sigNum, RESET);
    fprintf(ptr, "%s[THREAD] Sending message to children%s\n", RED, RESET);

    for (int i = 0; i < children_count; i++)
    {
        char tmp[MAX];
        memset(tmp, '\0', MAX);

        char id[MAX];
        memset(id, '\0', MAX);
        sprintf(id, "%d ", i);

        strcpy(tmp, id);
        strcat(tmp, msg);

        write(pipes[i][WRITE], tmp, MAX);
        close(pipes[i][WRITE]);
    }

    fprintf(ptr, "%s[THREAD] Children reset%s\n", RED, RESET);
    memset(children_pids, -1, MAX_CHD);
    children_count = 0;

    fprintf(ptr, "%s[THREAD] Terminating thread%s\n", RED, RESET);
    fclose(ptr);
}

void handler(int sigNum)
{
    if (father != getpid())
        return;

    pthread_t t_id;
    pthread_create(&t_id, NULL, send_msg, (void *)&sigNum);
}

void terminate(int sigNum)
{
    if (father != getpid())
        return;

    for (int i = 0; i < children_count; i++)
    {
        if (children_pids[i] == -1)
            continue;

        kill(children_pids[i], SIGKILL);
    }
}

int main(int argc, char **argv)
{
    father = getpid();

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGINT, terminate);

    char buf[MAX];
    int n = 0;
    memset(children_pids, -1, MAX_CHD);

    printf("%s[MAIN] my id is %d%s\n", GREEN, getpid(), RESET);

    while (1)
    {
        memset(buf, '\0', MAX);
        fgets(buf, MAX, stdin);

        if (sscanf(buf, "%d", &n))
        {
            if (n <= 0 || children_count + 1 > MAX_CHD)
            {
                printf("%s[MAIN] Too many children. Send msg to free them%s\n", GREEN, RESET);
                continue;
            }

            pipe(pipes[children_count]);

            if (!fork())
            {
                exit(0);
                printf("%s[CHD] I'm a new child with id %d, and I'm waiting for msg from my father%s\n", YELLOW, getpid(), RESET);

                children_pids[children_count] = getpid();

                close(pipes[children_count][WRITE]);

                memset(buf, '\0', MAX);
                read(pipes[children_count][READ], &buf, MAX);

                int id;
                memset(msg, '\0', MAX);

                sscanf(buf, "%d %[^\t\n]", &id, msg);
                printf("%s[CHD] I received the following message: To Child %d PID: %d: %s%s\n", YELLOW, id, getpid(), msg, RESET);

                close(pipes[children_count][READ]);

                exit(0);
            }
            else
            {
                close(pipes[children_count][READ]);
            }

            children_count++;
        }
        else
        {
            printf("%s[MAIN] Msg saved%s\n", GREEN, RESET);

            buf[strlen(buf) - 1] = '\0';

            memset(msg, '\0', MAX);
            strcpy(msg, buf);
        }
    }

    while (wait(NULL) > 0)
        ;

    return 0;
}