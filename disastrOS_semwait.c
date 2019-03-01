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
  
  // prendo il fc del semaforo dal pcb del processo in running, che ha chiamato la syscall
  int fd = running->syscall_args[0];

  SemDescriptor* sem_des = SemDescriptorList_byFd((ListHead*)&(running->sem_descriptors),fd);

  //controllo che il descrittore sia valido
  if(!sem_des){
    running->syscall_retvalue = DSOS_ESEM_DES_NOT_FOUD;
    return;
  }

  Semaphore* sem = sem_des->semaphore;

  //decremento il semaforo
  sem->count --;

  if(sem->count <= -1){
    running->status=Waiting;

    //alloco il descrittore da mettere nella coda di wait del semaforo
    SemDescriptor* sem_des = SemDescriptor_alloc(fd, sem, running);
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) sem_des);

    //schedulo manualmente poichè la funzione che implementa lo scheduling setta a ready il processo
    //che ho appena settato a waiting
    PCB* PCB_next = (PCB*)List_detach(&ready_list, ready_list.first);
    running=PCB_next;
  }

  running->syscall_retvalue = 0;
  return;
}
