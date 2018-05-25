/* Filename	: ShmOS.C
** Author	: Anung Barlianto/1292000058
** fork()-nya di sini
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

#include "shmos.h"

main () {
	int child_pid;
	int process;
	int sem_id;
	int shm_id;
	/* Variabel untuk shared memory */
	char *shm_char;
	int *shm_int;

	/* Make NUM_OF_SEM semaphores */
	if ((sem_id = semget (SEM_KEY, NUM_OF_SEM, SEM_FLAG)) == -1) {
		perror ("semget failed");
		exit (1);
	}
	/* Untuk kemudahan portabilitas, semaphore yang banyak itu
	** nggak diutak-utik dulu. Pake aja shared memory yang tiga
	** itu. */
	/* Bikin shared memory */
	if ((shm_id = shmget (SHM_KEY, (3*sizeof (int)), SHM_FLAG)) == -1) {
		perror ("shmget() failed");
		exit (2);
	}

	/* Inisialisasi nilai shared memory */
	/* Pertama, attach dulu ke shared memory.. */
	if ((shm_char = shmat (shm_id, 0, SHM_RND)) == (char *)-1) {
		perror ("shmat() init failed");
		exit (3);
	}
	/* Inisialisasinya di sini. */
	/* Dijadiin int[3] dulu */
	shm_int = (int *) (void *) shm_char;
	shm_int[IDX_TURN] = INITIAL_TURN;
	shm_int[IDX_NUMOFPROC] = NUM_OF_PROCESS;
	shm_int[IDX_PERIOD] = 0;
	/* Detach! */
	if (shmdt (shm_char) == -1) {
		perror ("shmdt() init failed");
		exit (4);
	}

	for (process=0; process<MAKE_CHILD; process++) {
		/* Parent code forks one more process. */
		if ((child_pid = fork ()) < 0) {
			perror ("Fork error");
			exit (5);
		} else if (child_pid == 0) {
			/* Child code goes here */
			child (process+1, sem_id, shm_id, process+2);
		}
	}
	/* Real parent code goes here */
	child (0, sem_id, shm_id, 1);
	return 0;
}

/* Nge-print info, sekalian kasih delay */
void my_info (int proc, int proc_id, int ctr_jatah, int jatah,
    int sem_id, int shm_id, int periode) {
    fprintf (stdout, "\tProc %d: pid %5d; jatah %d/%d; periode %d; sem_id %d; shm_id %d\n",
	proc, proc_id, ctr_jatah, jatah, periode, sem_id, shm_id);
    sleep (1);
}

/* Tiap proses mengerjakan tiap-tiap bagiannya di sini,
** setelah di-fork() */
void child (int child_id, int sem_id, int shm_id, int jatah) {
    int old_periode;
    int counter_jatah = jatah;
    /* Shared memory variables. */
    char* shm_addr;
    int* shared_m;
    union semun arg;
    struct shmid_ds *buf;

    /* Attach to shared memory.. hii.. */
    if ((shm_addr = shmat (shm_id, 0, SHM_RND)) == (char *)-1) {
        perror ("shmat() failed");
        exit (3);
    }
    /* Take the array of integer from shm_addr. */
    shared_m = (int *) (void *) shm_addr;
    /* Ambil periode.  */
    old_periode = shared_m[IDX_PERIOD];
    while (1) {
	/* Attempt to Down(). */
	Down (sem_id);
	/* Ok, we got the semaphore.. Entering critical section. */
	/* Whose TURN is it?  Get shared_m[IDX_TURN]'s value. */
	/* If this is process' turn:
	** 1. It prints its own message.
	** 2. Set shared_m[IDX_TURN] so other process could gain their turns.
	** 3. It also decrements its own counter_jatah.
	*/
	if (child_id == shared_m[IDX_TURN]) {
	    if (counter_jatah > 0) {
	        my_info (child_id+1, getpid(), counter_jatah, jatah,
	        	sem_id, shm_id, old_periode);
		/* Decrease counter_jatah. If it is equal zero, decrease
		** number of process involved. */
	        if (--counter_jatah == 0) {
		    /* Decrease number of process. If it is equal zero,
		    ** decrease number of period. */
	            if (--shared_m[IDX_NUMOFPROC] == 0) {
		        /* Tambah satu periode. */
	                shared_m[IDX_PERIOD]++;
	            }
	        }
	    }
	    /* Set untuk giliran proses berikutnya. */
	    shared_m[IDX_TURN] = ((shared_m[IDX_TURN] - 1) + NUM_OF_PROCESS) % NUM_OF_PROCESS;
            /* If num_of_process becomes zero, set SEM_NUM_OF_PROC's value
            ** back to NUM_OF_PROCESS. */
	    if (shared_m[IDX_NUMOFPROC] == 0)
	        shared_m[IDX_NUMOFPROC] = NUM_OF_PROCESS;
	    /* If counter_jatah equals zero, check if new
	    ** SEM_PERIOD's value differs from old_periode. */
	    if (old_periode != shared_m[IDX_PERIOD]) {
	        counter_jatah = jatah;
	        old_periode = shared_m[IDX_PERIOD];
	    }
	}

        /* Jika periodenya udah empat kali, detach shared memory,
        ** dan exit. Process selesai. */
        if (old_periode >= NUM_OF_PERIOD) {
	    /* Jika saya adalah satu-satunya proses yang masih ada,
	    ** sebelum exit, remove dulu semaphore dan shared memory. */
	    if (shared_m[IDX_NUMOFPROC] == 1) {
	        /* Remove semaphore.. */
	        arg.val = 0;
	        // if ((semctl (sem_id, 0, 0, arg.val)) == -1)
	        if ((semctl (sem_id, 0, 0, arg)) == -1)
		{
	            perror ("semctl (IPC_RMID) failed");
	            exit (1);
	        }
	        /* Remove shared memory.. */
	        /* But first, detached shared memory. */
	        if (shmdt (shm_addr) == -1) {
	            perror ("shmdt() done failed");
	            exit (4);
	        }
	        if ((shmctl (shm_id, 0, buf)) == -1) {
	            perror ("shmctl (IPC_RMID) failed");
	            exit (1);
	        }
	    }
	    /* ..bila bukan, kurangi jumlah proses yang tinggal.
	    ** dan detached shared memory. */
	    else {
	        shared_m[IDX_NUMOFPROC]--;
	        if (shmdt (shm_addr) == -1) {
	            perror ("shmdt() done failed");
	            exit (4);
	        }
            }
            exit (0);
        }

        /* Kalo udah, di-UP lagi dong semaphore-nya! Jangan rakus! */
        Up (sem_id);
    }
}

/* Nge-Down semaphore */
void Down (int sem_id) {
	if (semop (sem_id, &op_lock[0], 2) <  0) {
	    perror ("semop lock (Down) error.");
	    exit (6);
	}
}

/* Nge-Up semaphore */
void Up (int sem_id) {
        if (semop (sem_id, &op_unlock[0], 1) < 0) {
            perror ("semop unlock (Up) error");
            exit (7);
        }
}

