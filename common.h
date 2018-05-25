
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#ifdef LINUX
#include <linux/ipc.h>
#include <linux/sem.h>
#include <linux/shm.h>
#else
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#endif

// number of buffer slots in Shared Memory Buffer
#define N_BUFFER 15

typedef int semaphore;

#define N_SEMS 3          // number of semaphores
#define SEGSIZE ( (2 * sizeof (char)) + (N_BUFFER * sizeof(char)) )
#define NEXT_EMPTY_PLATE N_BUFFER
#define NEXT_FILLED_PLATE N_BUFFER+1

// types of semaphore used
enum prod_cons {mutex, empty, full};

/*
semaphore mutex = 1;
semaphore empty = N;
semaphore full = 0;
 */

// semaphore for lock / down
static struct sembuf sem_down = {
  0, -1, 0
};

// semaphore for unlock / up
static struct sembuf sem_up = {
  0, 1, 0
};

#ifdef CYGWIN
union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};
#endif


// prototype(s)
void createsem (int* sid, key_t key, int n_members);
void showsem (int sid);
void showshm (char* segptr);


#endif // _COMMON_H_

