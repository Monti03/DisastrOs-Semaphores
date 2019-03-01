#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  //releases from an application the given
  //returns 0 on success
  //returns an error code if the semaphore is not owned by the application

  // prendo il fd del semaforo dal pcb del processo in running, che ha chiamato la syscall
  int fd = running->syscall_args[0];

  SemDescriptor* sem_des = SemDescriptorList_byFd((ListHead*)&(running->sem_descriptors),fd);

  //controllo che il descrittore sia valido
  if(!sem_des){
    running->syscall_retvalue = DSOS_ESEM_DES_NOT_FOUD;
    return;
  }

  //rimuovo il descrittori dal pcb
  sem_des = (SemDescriptor*) List_detach(&running->sem_descriptors, (ListItem*) sem_des);
  assert(sem_des);

  Semaphore* sem=sem_des->semaphore;

  //rimuovo il descrittore dal semaforo
  SemDescriptorPtr* sem_des_ptr=(SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*)(sem_des->ptr));
  assert(sem_des_ptr);

  //libero la memoria
  Descriptor_free(sem_des);
  DescriptorPtr_free(sem_des_ptr);
  
  //se non sono presenti altri processi che condividono il semaforo -> libero la memoria 
  if(sem->descriptors.size == 0){
    //unlink
    disastrOS_debug("ilsemaforo:%dverrà eliminato\n", sem->id);
    SemaphoreList_print(&(sem->descriptors));
    sem = (Semaphore*) List_detach(&semaphores_list, (ListItem*) sem);
    assert(sem);
    Semaphore_free(sem);
  }

  running->syscall_retvalue=0;
  return;
}
