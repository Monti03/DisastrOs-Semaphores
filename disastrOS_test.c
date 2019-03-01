#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <assert.h>

#include "disastrOS.h"

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}


void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  // apro i semafori
  int sem1 = disastrOS_semOpen(0);
  int sem2 = disastrOS_semOpen(disastrOS_getpid());
  // riapro il semaforo con id 0 -> avrò un fd diverso, ma le operazioni avverranno sullo stesso semaforo
  int sem3 = disastrOS_semOpen(0);

  //controllo che i descrittori siano validi -> se negativi c è stato un errore
  assert(sem1 >= 0 && sem2 >= 0 && sem3 >= 0);

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);

    // wait su i due semafori prima definiti 
    // sem2 non bloccherà mai il processo poichè solo lui sarà connesso a quel semaforo
    disastrOS_semWait(sem1);
    disastrOS_semWait(sem2);

    // supposed critical section
    printf("processo %d in sezione critica!\n", disastrOS_getpid());
    disastrOS_sleep((int)disastrOS_getpid()/2);

    // posting after exiting from critical section
    printf("processo %d è uscito dalla sezione critica!\n", disastrOS_getpid());
    disastrOS_semPost(sem2);
    disastrOS_semPost(sem3);

  }

  // chiudo i semafori
  disastrOS_semClose(sem1);
  disastrOS_semClose(sem2);
  disastrOS_semClose(sem3);

  disastrOS_exit(disastrOS_getpid()+1);
}



void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
