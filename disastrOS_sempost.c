#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  //increments the given semaphore
  //if the semaphore was at 0, and some other thread was waiting
  //the thread is resumed
  //returns 0 on success, an error code on failure

  // prendo il fc del semaforo dal pcb del processo in running, che ha chiamato la syscall
  int fd = running->syscall_args[0];

  SemDescriptor* sem_des = SemDescriptorList_byFd((ListHead*)&(running->sem_descriptors),fd);

  //controllo che il descrittore sia valido
  if(!sem_des){
    running->syscall_retvalue = DSOS_ESEM_DES_NOT_FOUD;
    return;
  }

  Semaphore* sem = sem_des->semaphore;
  
  //incremento il semaforo
  sem->count ++;
  disastrOS_debug("il semaforo:%d è stato incrementato-> count:%d\n", sem->id, sem->count);
  //se il semaforo era a -1 -> ora è a 0 -> avrò un processo in waiting
  if(sem->count <= 0){
    
    SemDescriptor* sem_desc_next = (SemDescriptor*)List_detach(&sem->waiting_descriptors, sem->waiting_descriptors.first);

    //prendo il pcb del primo processo nella coda di waiting e setto a ready lo stato
    PCB* PCB_next = sem_desc_next->pcb;
    PCB_next->status=Ready;
    disastrOS_debug("il processo:%d è stato messo in ready\n", PCB_next->pid);
    printf("il processo:%d è stato messo in ready\n", PCB_next->pid);

    //libero la memoria
    SemDescriptorPtr_free(sem_desc_next);

    List_insert(&ready_list, ready_list.first, (ListItem*) PCB_next);
  }
  running->syscall_retvalue = 0;
}
