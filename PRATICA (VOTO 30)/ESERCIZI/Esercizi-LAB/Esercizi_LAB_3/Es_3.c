/*Realizzare funzioni per stringhe char *stringrev(*char str) (inverte ordine caratteri) e int stringpos(*char str,
char chr) (cerca chr in str e restituisce la posizione) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
char* stringrev(char *str){
    char tmp[50]; //Declare temporary string
    short len = strlen(str); //Save string length
    int ind=len;
    for(int i=0; i<len;i++){ //Cycle through string chars
        tmp[i]=str[--ind]; // Save ind-th string element as the i-th
    }
    tmp[len]='\0'; //Save termination char
    strcpy(str,tmp);
    return str; 
}

int stringpos(char* str, char chr){
    for(int i=0;i<strlen(str);i++){
        if(str[i] == chr){
            return i;
        }
    }
    return -1;
}

int main (int argc, char** argv){
    if(argc < 3){
        printf("Mona gli argomenti devono essere almeno 2\n");
        exit(2);
    }
    if(strlen(argv[1]) > 100){
        printf("Mona la stringa è troppo lunga zio pera\n");
        exit(3);
    }
    if(strlen(argv[2]) > 1){
        printf("Mona il secondo parametro è un char\n");
        exit(4);
    }

    printf("str--> %s\npos--> %d\n",stringrev(argv[1]),stringpos(argv[1],argv[2][0]));

    return 0;
}