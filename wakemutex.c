
#include "producer.h"

int main ()
{
  key_t key;
  int semset_id;
  int choice;
  char item = 0;
  int done = 0;

  int shm_id;
  char* segptr;

  int slot = 0;

  // create unique key
  key = ftok (".", 's');

#ifdef DEMO
  createsem (&semset_id, 1292, N_SEMS);
  printf ("\tsemaphore id: %d\n", semset_id);
  createshm (&shm_id, &segptr, 1292);
  printf ("\tSharedMemory id: %d\n", shm_id);
#else
  createsem (&semset_id, key, N_SEMS);
  printf ("\tsemaphore id: %d\n", semset_id);
  createshm (&shm_id, &segptr, key);
  printf ("\tSharedMemory id: %d\n", shm_id);
#endif

  printf (" UP operation on MUTEX\n");
  up (semset_id, mutex);

  return 0;
}

void down (int sid, int semaphore)
{
  sem_down.sem_num = semaphore;

  printf ("==> PRODUCER attempting a DOWN on ");
  switch (semaphore)
  {
    case mutex:
      printf ("MUTEX... ");
      break;
    case full:
      printf ("FULL... ");
      break;
    case empty:
      printf ("EMPTY... ");
      break;
  }
  if ( (semop (sid, &sem_down, 1)) == -1)
    printf ("failed!\n");
  else
    printf ("success!\n");
}

void up (int sid, int semaphore)
{
  sem_up.sem_num = semaphore;

  printf ("==> WAKEMUTEX attempting an UP on ");
  switch (semaphore)
  {
    case mutex:
      printf ("MUTEX... ");
      break;
    case full:
      printf ("FULL... ");
      break;
    case empty:
      printf ("EMPTY... ");
      break;
  }
  if ( (semop (sid, &sem_up, 1)) == -1)
    printf ("failed!\n");
  else
    printf ("success!\n");
}

/* SHARED MEMORY CODEs
 */

void createshm (int* shmid, char** segptr, key_t key)
{
  int ii;
  int isClient;

  printf ("\tAttempting to create new shared memory...\n");

  isClient = (*shmid = shmget (key, SEGSIZE,
        IPC_CREAT|IPC_EXCL|0666));
  if ( isClient == -1)
  {
    printf ("\tShared memory already exists, try as a client...\n");
    if ( (*shmid = shmget (key, SEGSIZE, 0)) == -1)
    {
      perror ("shmget");
      exit (2);
    }
  }
  else
  {
    printf ("\tCreating new shared memory segment\n");
  }
  // attach the shared memory segment into the current process
  if (( *segptr = (char *) shmat (*shmid, 0, 0)) == (char *) -1)
  {
    perror ("segptr = shmat");
    exit (3);
  }
  else
  {
    printf ("\tAttaching to new shared memory segment...\n");
    if (isClient != -1)
    {
      printf ("\tInitializing shared memory segment...\n");
      for (ii=0; ii<N_BUFFER+2; ii++)
        (*segptr)[ii] = ' ';
      (*segptr)[NEXT_EMPTY_PLATE] = 0;
      (*segptr)[NEXT_FILLED_PLATE] = 0;
    }
  }
}

void removeshm (int shmid)
{
#ifdef DEMO
  printf ("\tDEMO VERSION -- removing semaphore not implemented.\n");
#else
  shmctl (shmid, IPC_RMID, 0);
  printf ("\tSharedMemory marked for deletion\n");
#endif
}

