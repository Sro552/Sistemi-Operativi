#include <stdio.h> //pipe.c
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char** argv){
    int fd[2], cnt = 0;
    while(pipe(fd) == 0){ //Create unnamed pipe using 2 file descriptors
        cnt++;
        printf("[%d,%d] ",fd[0],fd[1]);

    }
    printf("[%d,%d] ",fd[0],fd[1]);
    printf("\n Opened %d pipes, then error\n",cnt);
    int op = open("file.txt", O_RDWR|O_CREAT , S_IRUSR|S_IWUSR );
    printf("File opened with fd %d\n",op); 
}