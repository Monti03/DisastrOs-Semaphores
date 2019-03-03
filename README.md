# DisastrOs-Semaphores

In questa repository è presente l'implementazione dei semafori in DisastrOS.  
Sono state implementate le seguenti funzioni dei semafori:  
```C  
int sem_open(int id)
```
```C  
int sem_close(int fd)
```
```C  
int sem_wait(int fd)
```
```C  
int sem_post(int fd)
```

### SEM_OPEN ###  
```C  
int sem_open(int id)
```
##### PARAMETRI #####
`int id` : un intero che rappresenta il semaforo globalmente.

##### DESCRIZIONE #####
Crea un nuovo semaforo o ne apre uno già esistente con identificativo uguale al parametro della system call.  
Viene aggiunto alla lista dei semafori aperti dal processo un SemDescriptor sem_des e al semaforo un SemDescriptorPtr relativo a sem_des.

##### RETURN #####
In caso di successo viene ritornato un intero maggiore o uguale di zero, in caso di errore un intero negativo

##### ERRORI ##### 
DSOS_ESEM_MAX_NUMBER_OF_SEMDESCRIPTORS : sono stati aperti il numero massimo di semafori dal processo  
DSOS_ESEM_NAME : l'id del semaforo è negativo, sono accettati solo id positivi  
DSOS_ESEM_ALLOC : errore nella allocazione del nuovo semaforo  
DSOS_ESEM_DES_ALLOC : errore nella allocazione del SemDescriptor del semaforo  
DSOS_ESEM_DES_PTR_ALLOC : errore nella allocazione del SemDescriptorPtr del semaforo

### SEM_CLOSE ###
```C  
int sem_close(int fd)
```
##### PARAMETRI #####
`int fd` : il file descriptor del semaforo.

##### DESCRIZIONE #####
Chiude il semaforo (se il processo ha aperto più volte lo stesso semaforo, dovrà chiudere tutti i descrittori).  
Libera SemDescriptor e SemDescriptorPtr rispettivamente presenti nella lista dei SemDescriptor del processo e nella lista dei SemDescriptorPtr del semaforo.
Se nessun processo condivide quel semafo erosegue l'operazione di unlink: libera la memoria occupata dal semaforo.

##### RETURN #####
In caso di successo viene ritornato un intero maggiore o uguale di zero, in caso di errore un intero negativo

##### ERRORI ##### 
DSOS_ESEM_DES_NOT_FOUD : il file descriptor passato come parametro non è associato a nessun semaforo aperto dal processo  



### SEM_WAIT ###
```C  
int sem_wait(int fd)
```
##### PARAMETRI #####
`int fd` : il file descriptor del semaforo.

##### DESCRIZIONE #####
Decrementa il contatore del semaforo.
Se il valore del contatore è minore o uguale a `-1` il processo è inserito nella waiting_list ed è allocato un SemDescriptorPtr che sarà inserito nella lista dei SemDescriptorPtr bloccati dal semaforo.

##### RETURN #####
In caso di successo viene ritornato un intero maggiore o uguale di zero, in caso di errore un intero negativo

##### ERRORI ##### 
DSOS_ESEM_DES_NOT_FOUD : il file descriptor passato come parametro non è associato a nessun semaforo aperto dal processo  

### SEM_POST ###
```C  
int sem_post(int fd)
```
##### PARAMETRI #####
`int fd` : il file descriptor del semaforo.

##### DESCRIZIONE #####
Incrementa il contatore del semaforo.  
Se il contatore diviene minore o uguale a zero viene rimosso un SemDescriptorPtr dalla lista dei processi bloccati dal semaforo, si ottiene il processo relativo che sarà inserito nella coda di ready. Il SemDescriptorPtr allocato nella sem_wait verrà liberato.

##### RETURN #####
In caso di successo viene ritornato un intero maggiore o uguale di zero, in caso di errore un intero negativo

 ##### ERRORI ##### 
DSOS_ESEM_DES_NOT_FOUD: il file descriptor passato come parametro non è associato a nessun semaforo aperto dal processo 

# ESECUZIONE
Per eseguire il programma bisogna:  
compilare con il comando  
`make`  
quindi eseguire con il comando  
`./disastrOS_test`  
oppure se si vuole usare valgrind  
`valgrind ./disastrOS_test`







