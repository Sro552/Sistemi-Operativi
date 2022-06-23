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

#define MAX 1000

int fileExists(char *fileName)
{
    FILE *ptr = fopen(fileName, "r");

    int result = (ptr) ? 1 : 0;

    if (ptr)
        fclose(ptr);

    return result;
}

int main(int argc, char **argv)
{
    int mode_l = -1;
    int mode_c = -1;

    char fileName[MAX];
    memset(fileName, '\0', MAX);

    int normal = 0;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-l"))
            mode_l = i;
        else if (!strcmp(argv[i], "-c"))
            mode_c = i;
        else
        {
            normal = 1;

            if (!fileExists(argv[i]))
                continue;

            strcpy(fileName, argv[i]);
        }
    }

    if (!strlen(fileName) && normal)
    {
        fprintf(stderr, "Il file specificato non esiste\n");
        return 1;
    }
    else if (mode_l == -1 && mode_c == -1)
    {
        fprintf(stderr, "Parametri non validi\n");
        return 2;
    }

    if (!fork())
    {
        int l = 0;
        int c = 0;

        if (!strlen(fileName))
        {
            // modalità pipe
            char buf[MAX];
            memset(buf, '\0', MAX);
            read(0, &buf, MAX);

            for (int i = 0; i < strlen(buf); ++i)
            {
                if (buf[i] == '\n')
                    l++;

                c++;
            }
        }
        else
        {
            // modalità normale
            FILE *ptr = fopen(fileName, "r");

            char ch;

            while ((ch = fgetc(ptr)) != EOF)
            {
                if (ch == '\n')
                    l++;

                c++;
            }

            fclose(ptr);
        }

        if (mode_l != -1 && mode_c != -1)
        {
            if (mode_l < mode_c)
                printf("%d %d\n", l, c);
            else
                printf("%d %d\n", c, l);
        }
        else if (mode_l != -1 || mode_c != -1)
            printf("%d\n", (mode_l != -1) ? l : c);
    }

    while (wait(NULL) > 0)
        ;

    return 0;
}