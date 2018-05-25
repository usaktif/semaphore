/* Filename	: ShmOS.H
** Author	: Anung Barlianto/1292000058
** Revision history:
**	1996.12.06	Finished! Semaphre and Shared memory removal.
**	1996.12.06	Pengembangan berikutnya.. coba hapus sem dan shm.
**	1996.11.30	Shared Memory version finished!.
**			Actually shm is much easier and shorter.
**			Gilee.. siapa dulu asistennya?
**	1996.11.29	<SunSITE> Coba pake shared memory.
**			shared_memory: 3 * sizeof (int): shm[0]=INITIAL_TURN;
**			shm[1]=NUM_OF_PROCESS; shm[2]=NUM_OF_PERIOD;
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
/* IPC header file */
#include <sys/ipc.h>
/* Semaphore header file */
#include <sys/sem.h>
/* Shared memory header file */
#include <sys/shm.h>
/* RISC/6000 AIX specific */
#ifdef AIX
#include <sys/mode.h>
#endif

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

#ifndef FILE_COMMON
#define FILE_COMMON

extern void perror();

#define NUM_OF_PROCESS	3	/* Number of processes involved */
#define MAKE_CHILD	2	/* Defining how many child will be created */
#define SEM_KEY		0x1292	/* Semaphore key on semget() call */
#define SEM_FLAG	IPC_CREAT | 0664
				/* Semaphore's flag on semget() */
#define SHM_KEY		0x12931294	/* Shared Memory key on shmget() */
#define SHM_FLAG	IPC_CREAT | 0664
				/* Shared memory flag on shmget() */
/* Well, I need one set of semaphores that contain only one member.
** (thanks to shared memory!) It is used for P/V semaphore. */
#define NUM_OF_SEM	1	/* Number of semaphore in the set. */
/* As a companion, I need one shared memory whose size is 3*sizeof(int).
** It'll act as an array of int contains 3 elements. */
#define NUM_OF_SHM	1	/* Number of shared memories. */
#define IDX_TURN	0	/* Index to shared memory for turn. */
#define IDX_NUMOFPROC	1	/* Index to shared memory for number of ps. */
#define IDX_PERIOD	2	/* Index to shared memory for
				** number of period. */
#define INITIAL_TURN	2	/* Which process hold the semaphore first? */
#define NUM_OF_PERIOD	4	/* Number of period */

/*** Defining variables ***/
/* Semaphore for lock/Down() */
static struct sembuf op_lock[3] = {
	0, 0, 0,	/* wait for sem#0 to become greater than zero */
	0, 1, SEM_UNDO	/* then decrement sem#0 by 1 */
};

/* Semaphore for unlock/Up() */
static struct sembuf op_unlock[1] = {
	0, -1, (IPC_NOWAIT | SEM_UNDO)
			/* increment sem#0 by 1 */
};

/* Function prototypes in common */
void my_info (int proc, int proc_id, int ctr_jatah, int jatah,
    int sem_id, int shm_id, int periode);
void child (int child_id, int sem_id, int shm_id, int jatah);
void Down (int sem_id);
void Up (int sem_id);

#endif

