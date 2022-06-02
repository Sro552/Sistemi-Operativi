/* lengthof.c */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int code=0, len=0;
    char *p;
    if (argc!=2) {
        printf("Usage: %s <string>\n", argv[0]);
        code=2;
    } else {
        p=argv[1];
        while (*p !=0 ){
        p++;
        len++;
    }
    
    printf("%d\n", len);
    printf("%ld\n",strlen(argv[1]));
    }
    return code;
}