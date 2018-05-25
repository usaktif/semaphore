
#include "common.h"

/* SHARED MEMORY CODEs
 */

void createsem (int* sid, key_t key, int n_members)
{
  int counter;
  union semun semopts;

  printf ("\tAttempting to create new semaphore");
  printf (" set with %d members\n", n_members);

  // periksa semaphore-nya udah ada atau belum
  if ( (*sid = semget (key, n_members, 0666)) == -1)
  {
    if ( (*sid = semget (key, n_members, IPC_CREAT | IPC_EXCL | 0666) ) == -1)
    {
      perror ("Error in semget");
    }
    else
    {
      printf ("\tSuccessfully creating semaphores\n");
      // set semaphore mutex value to 1
      semopts.val = 1;
      semctl (*sid, mutex, SETVAL, semopts);
  
      // set semaphore empty value to N_BUFFER
      semopts.val = N_BUFFER;
      semctl (*sid, empty, SETVAL, semopts);
  
      // set semaphore full value to 0
      semopts.val = 0;
      semctl (*sid, full, SETVAL, semopts);
    }
  }
  else
  {
    printf ("\tThere is already a semaphore set with id# %d\n", *sid);
  }
}

void showsem (int sid)
{
  int ctr = 0;
  int num_sems;
  int semval;
  union semun semopts;

  printf ("\tSemaphore\tValue\n");
  while (ctr < N_SEMS)
  {
    semval = semctl (sid, ctr, GETVAL, semopts);
    switch (ctr)
    {
      case mutex:
        printf ("\tMUTEX");
        break;
      case full:
        printf ("\tFULL ");
        break;
      case empty:
        printf ("\tEMPTY");
        break;
    }
    printf ("\t\t%d\n", semval);
    ctr++;
  }
}

void showshm (char* segptr)
{
  int ii=0;

  printf ("\n");
  for (ii=0; ii<N_BUFFER; ii++)
    printf (" %2d", ii);
  printf ("\n");
  printf ("+");
  for (ii=0; ii<SEGSIZE; ii++)
    printf ("--+");
  printf ("\n");

  printf ("|");
  for (ii=0; ii<SEGSIZE-2; ii++)
    if (segptr[ii] == ' ')
      printf ("  |");
    else
      printf ("%2c|", segptr[ii]);
  printf ("%02d|", segptr[NEXT_EMPTY_PLATE]);
  printf ("%02d|", segptr[NEXT_FILLED_PLATE]);
  printf ("\n");

  printf ("+");
  for (ii=0; ii<SEGSIZE; ii++)
    printf ("--+");
  printf ("\n");
}

