#include <time.h>
#include <stdio.h>


//Il primo restituisce il tempo di oggi mesa anno ecc
//Il secondo restituisce quanto è passato dal 1970
int main(int argc, char** argv){
    time_t theTime;
    time_t whatime = time(&theTime);// seconds s in ce 1/1/1970
    // Print date i n Www Mmm dd hh :mm: ss yyyy

    printf("Tempo corrente: %s / %d \n",ctime(&whatime),theTime);
}