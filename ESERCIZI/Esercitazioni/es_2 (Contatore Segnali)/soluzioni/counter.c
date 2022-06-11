/*

  Applicazione per gestire il conteggio dei segnali ricevuti/gestiti da processi esterni:
  - alla ricezione/gestione di un SIGUSR1 o SIGUSR2 si deve tenere un conteggio
    di ciascun segnale suddiviso in base al PID del processo che lo invia
  - ad ogni ricezione si deve mostrare un feedback delle informazioni (almeno: codice segnale
    e pid del mittente) ma solo se l'applicazione non è eseguita in background
  - alla ricezione di SIGTERM/SIGINT si deve uscire mostrando un report dei conteggi

*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

// INITIALIZATION ----------------------------------------------------------------------------------

// Maximum number of processes to manage:
#define MAXPROCS 10

// Pre-declare signal functions
static void sig_handler(int signo, siginfo_t *info, void *ctx);
void set_handler();

// Wrapper to print errors
void perr(char *msg) { fprintf(stderr, "?%s\n", msg); };

// Custom boolean type + Counter type
typedef enum { false, true } bool;
typedef struct Counter {
   int pid;
   int usr1;
   int usr2;
} Counter;

bool    idle = true;       // used to wait until an exit signal is received
Counter counter[MAXPROCS]; // array of counters (pid, usr1, usr2)
int     cur=0;            // pointer to first available slot into above array
int     er=0;              // keep track of errors
struct  sigaction act;     // structure to handle signals

// -------------------------------------------------------------------------------------------------

// check if application run in foreground
bool foreground() {
  //  tcgetpgrp(fd):
  //    returns process group ID of the foreground process group on the terminal associated to fd
  //    (should be the controlling terminal of the calling process)
  //  getpgrp():
  //    returns process group ID of the calling process
  bool fg=false;
  if (getpgrp() == tcgetpgrp(STDOUT_FILENO)) 
    fg=true;
  return fg;
}

// look for a PID inside counter array and return index 
//If not found insert it into the counter array at the next available slot
int posof(int pid) {
  int p=0;     //counter for scanning array
  int found=-1; //it will hold the position of the PID inside the counter
  
  // scan array
  while( p < MAXPROCS){       //Cycle through the entire counter array
    if (counter[p].pid==pid) {  //Found the PID
      found=p; 
      break; 
    }
    p++;
  } 

  if (found<0 && cur<MAXPROCS) {            // IF pid not found and there are slots available...
    counter[cur].pid=pid;                   // ...initialize blank item with pid...
    counter[cur].usr1=0; counter[p].usr2=0; // ...and counters...
    found=cur;                              // ...then returns position
    cur++;                                  // Increase cur counter
  } else {                                  // ...ELSE...
    if (found<0) 
      perr("Contacts overflow");            // ...dump error if not found (otherwise has been found or set)...
  };                                        // ENDIF
  return found;                             // return position (-1 if not found and no slot available)
};

// -------------------------------------------------------------------------------------------------

// signal setup: handle all signals here with the same handler
void set_handler() {
  act.sa_flags = SA_SIGINFO;           // retrieve additional info when signal arrives (as pid of sender)
  act.sa_sigaction = &sig_handler;
  er = er+sigaction(SIGUSR1, &act, 0);
  er = er+sigaction(SIGUSR2, &act, 0);
  er = er+sigaction(SIGTERM, &act, 0);
  er = er+sigaction(SIGINT , &act, 0);
}

// signal handler
static void sig_handler(int signo, siginfo_t *info, void *ctx) {
  int sender;
  sender=(int)info->si_pid; // retrieve pid of sender
  int p=posof(sender);      // look for pid of sender inside counter array
  if(p == -1){
    exit(2); //Terminate execution
  }
  if (foreground()==true){ 
    printf("Signal %d from pid %d (index: %d) (idle is: %d)\n", signo, sender, p, idle); // feedback
  }

  // ending signal
  if (signo==SIGTERM || signo==SIGINT) { 
    idle = false; // this is checked in main loop
  } else {
    switch (signo) { // update counter based on signal received
      case SIGUSR1: 
        counter[p].usr1 = counter[p].usr1+1; 
        break;
      case SIGUSR2: 
        counter[p].usr2 = counter[p].usr2+1; 
        break;
    };
  };
};

// -------------------------------------------------------------------------------------------------

int main(void) {
  
  set_handler();                                // initialize handling of signals
  
  printf("[main: #%d]\n", getpid());            // dump own pid
  
  while(idle==true){
    sleep(1);           // idling loop (break by termination signal)
  };    

  // Final feedback:
  printf("Summary:\n\n");
  
  int p;
  for (p=0; p<cur; p++) {                                                     // scan counter array...
    printf("#%d: %d / %d\n", counter[p].pid, counter[p].usr1, counter[p].usr2); // ...and dump data...
  };                                                                            
  printf("\n\n");
  return 0;
}

// -------------------------------------------------------------------------------------------------
