#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  //decrements the given semaphore
  //if the semaphore was? 0, the caller is put onto wait
  //returns an error code

  // prendo il fd del semaforo dal pcb del processo in running, che ha chiamato la syscall
  int fd = running->syscall_args[0];

  SemDescriptor* sem_des = SemDescriptorList_byFd((ListHead*)&(running->sem_descriptors),fd);

  //controllo che il descrittore sia valido
  if(!sem_des){
    running->syscall_retvalue = DSOS_ESEM_DES_NOT_FOUD;
    return;
  }

  Semaphore* sem = sem_des->semaphore;

  PCB* old_running = running;

  //decremento il semaforo
  sem->count --;
  disastrOS_debug("il semaforo:%d è stato decrementato-> count:%d\n", sem->id, sem->count);
  if(sem->count <= -1){
    
    running->status=Waiting;
    disastrOS_debug("il processo:%d è in wait nel semaforo:%d\n",running->pid, sem->id);
    printf("il processo:%d è in wait nel semaforo:%d\n",running->pid, sem->id);
    
    //alloco il SemDescriptorPtr relativo a sem_des per metterlo nella coda dei descrittori in waiting
    SemDescriptorPtr* sem_des_ptr =SemDescriptorPtr_alloc(sem_des);
    assert(sem_des_ptr);

    List_insert((ListHead*)&sem->waiting_descriptors,(ListItem*) sem->waiting_descriptors.last, (ListItem*) sem_des_ptr);
    
    //inserisco il processo nella coda di wait
    List_insert((ListHead*)&waiting_list,(ListItem*) waiting_list.last,(ListItem*)running);

    //schedulo manualmente poichè la funzione che implementa lo scheduling setta a ready il processo
    //che ho appena settato a waiting
    PCB* PCB_next = (PCB*)List_detach(&ready_list, ready_list.first);
    running=PCB_next;
    disastrOS_debug("il processo:%d è messo in running manualmente\n",PCB_next->pid);    
  }

  old_running->syscall_retvalue = 0;
  return;
}
