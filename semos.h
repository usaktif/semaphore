/* Filename	: SemOS.H
** Author	: Anung Barlianto/1292000058
** Revision history:
**	1996.11.01	Finished!
**	1996.10.29	Got bright ideas on the algorithm.
**	1996.10.02	Start playing semaphore for Down() and Up().
**	1996.10.01	Start programming; fork 2 children.
**	1996.09.30	Description was posted on "forum.iki.os".
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef LINUX
#include <linux/ipc.h>
#include <linux/sem.h>
#else
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};
#endif
/* RISC/6000 AIX specific */
#ifdef AIX
#include <sys/mode.h>
#endif

extern void perror();

#ifndef FILE_COMMON
#define FILE_COMMON

#define NUM_OF_PROCESS	3	/* Number of processes involved */
#define MAKE_CHILD	2	/* Defining how many child will be created */
#define SEM_KEY		1292L	/* Defining Semaphore key on semget() call */
/* Well, I need a set of semaphores that contain 4 members.
** First of them is used for P/V semaphore. The second one
** is for 'Whose Turn Is It?' semaphore. The third one is
** for Number_Of_Period semaphore, and the last of them is
** used for Number_Of_Process_Remains. */
#define NUM_OF_SEM	4	/* Number of semaphore in the set. */
#define SEM_FLAG	IPC_CREAT | 0664
				/* Semaphore's flag on semget() */
#define INITIAL_TURN	2	/* Which process hold the semaphore first? */
#define NUM_OF_PERIOD	4	/* Number of period */
#define SEM_TURN	1	/* Which semaphore used for turn? */
#define SEM_PERIOD	2	/* Which semaphore used for period? */
#define SEM_NUM_OF_PROC	3	/* Which semaphore used for num_of_process? */

/*** Defining variables ***/
/* Semaphore for lock */
static struct sembuf op_lock[3] = {
	0, 0, 0,	/* wait for sem#0 to become greater than zero */
	0, 1, SEM_UNDO	/* then decrement sem#0 by 1 */
};

/* Semaphore for unlock */
static struct sembuf op_unlock[1] = {
	0, -1, (IPC_NOWAIT | SEM_UNDO)
			/* increment sem#0 by 1 */
};

/* Global variables */
int num_of_process = NUM_OF_PROCESS;

/* Function prototypes in common */
void my_info (int proc, int proc_id, int ctr_jatah, int jatah,
    int sem_id, int periode);
void child (int child_id, int sem_id, int jatah);

#endif

