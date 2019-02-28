#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  // creates a semaphore in the system, having num semnum
  // the semaphore is accessible throughuot the entire system
  // by its id.
  // on success, the function call returns semnum (>=0);
  // in failure the function returns an error code <0
  
  // prendo l'id del semaforo dal pcb del processo in running, che ha chiamato la syscall
  int id = running->syscall_args[0];

  // controllo che l'id sia positivo 
  if(id < 0){
    running->syscall_retvalue = DSOS_ESEM_NAME; 
    return;
  }
  
  //cerco tra i semafori aperti globalmente se è presente un semaforo con quell'id
  Semaphore* sem = SemaphoreList_byId((SemaphoreList*)&semaphores_list, id);


  if(!sem){
    //non è presente -> dobbiamo creare il semaforo
    sem = Semaphore_alloc(id, 1);
    
    if( ! sem ){
      //errore nella allocazione, può essere causato dal numero eccessivo di semafori aperti
      running->syscall_retvalue = DSOS_ESEM_ALLOC; 
      return;
    }

    //poichè abbiamo aperto il semaforo va aggiunto nella lista dei semafori globali
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*)sem);
  }

  //creaiamo il SemDescriptor
  SemDescriptor* sem_des=SemDescriptor_alloc(running->last_fd, sem, running);
  if (! sem_des){
    //errore nella allocazione, può essere causato dal numero eccessivo di semafori aperti
    running->syscall_retvalue=DSOS_ESEM_DES_ALLOC;
    return;
  }
  //aggiorno il fd che sarà dato al prossimo semaforo
  running->last_fd++; 

  //alloco il SemDescriptorPtr
  SemDescriptorPtr* sem_des_ptr =SemDescriptorPtr_alloc(sem_des);
  
  //inserisco sem_des tra i sem_descriptors aperti dal processo
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sem_des);
  
  sem_des->ptr= sem_des_ptr;
  
  //inserisco sem_des_ptr tra i SemDescriptorPtr che puntatno al semaforo
  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_des_ptr);

  // return the FD of the new descriptor to the process
  running->syscall_retvalue = sem_des->fd; 
}
