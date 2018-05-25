
#include "monitorshm.h"

int main ()
{
  key_t key;
  int semset_id;
  int choice;
  int done = 0;
  int shm_id;
  char* segptr;

  // create unique key
  key = ftok (".", 's');

#ifdef DEMO
  createsem (&semset_id, 1292, N_SEMS);
  attachshm (&shm_id, &segptr, 1292);
  fprintf (stdout, "\tSharedMemory id: %d\n", shm_id);
#else
  createsem (&semset_id, key, N_SEMS);
  attachshm (&shm_id, &segptr, key);
  fprintf (stdout, "\tSharedMemory id: %d\n", shm_id);
#endif

  while ( !done && (choice = ask ()) )
  {
    switch (choice)
    {
      case 1:
        showsem (semset_id);
        break;
      case 2:
        showshm (segptr);
        break;
      case 3:
        done = 1;
        break;
    }
  }
  return 0;
}

int ask (void)
{
  char option;

  fprintf (stdout, "\n");
  fprintf (stdout, "\tsee s(e)maphore status\n");
  fprintf (stdout, "\tsee s(h)ared memory contents\n");
  fprintf (stdout, "\te(x)it\n");

  fprintf (stdout, "\n[Monitor SHM] ");
  scanf ("%c", &option);
  fflush (stdin);
  
  switch (option)
  {
    case 'e':
      return 1;
    case 'h':
      return 2;
    case 'x':
      return 3;
  }
}

/* SHARED MEMORY CODEs
 */

void attachshm (int* shmid, char** segptr, key_t key)
{
  fprintf (stdout, "\tAttempting to attach to shared memory...\n");

  if ( (*shmid = shmget (key, SEGSIZE, 0)) == -1)
  {
    perror ("shmget");
    exit (2);
  }

  // attach the shared memory segment into the current process
  if (( *segptr = (char *) shmat (*shmid, 0, 0)) == (char *) -1)
  {
    perror ("segptr = shmat");
    exit (3);
  }
  else
  {
    fprintf (stdout, "\tAttaching to new shared memory segment...\n");
  }
}

