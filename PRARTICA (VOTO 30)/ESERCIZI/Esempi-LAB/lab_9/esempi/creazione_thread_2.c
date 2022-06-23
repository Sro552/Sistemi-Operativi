#include <pthread.h> //threadList.c
void * my_fun(void * param){
while(1);
}
void main(){
pthread_t t_id;
pthread_create(&t_id, NULL, my_fun, NULL);
while(1);
}