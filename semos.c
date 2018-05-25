/* Filename	: SemOS.C
** Author	: Anung Barlianto/1292000058
** fork()-nya di sini
** Revision history:
**	1996.11.01	Finished!
**	1996.10.29	Got bright ideas on the algorithm.
**	1996.10.02	Start playing semaphore for Down() and Up().
**	1996.10.01	Start programming; fork 2 children.
**	1996.09.30	Description was posted on "forum.iki.os".
*/

#include "semos.h"

main () {
	int child_pid;
	int process;
	int sem_id;
	union semun arg_semctl;

	/* Make NUM_OF_SEM semaphores */
	if ((sem_id = semget (SEM_KEY, NUM_OF_SEM, SEM_FLAG)) == -1) {
		perror ("semget failed");
		exit (1);
	}
	/* Initialize three of semaphores that successfully created */
	arg_semctl.val = INITIAL_TURN;
	if (semctl (sem_id, SEM_TURN, SETVAL, arg_semctl) == -1) {
		perror ("semctl (SEM_TURN, SETVAL) failed");
		exit (2);
	}
	arg_semctl.val = NUM_OF_PERIOD;
	if (semctl (sem_id, SEM_PERIOD, SETVAL, arg_semctl) == -1) {
		perror ("semctl (SEM_PERIOD, SETVAL) failed");
		exit (2);
	}
	arg_semctl.val = NUM_OF_PROCESS;
	if (semctl (sem_id, SEM_NUM_OF_PROC, SETVAL, arg_semctl) == -1) {
		perror ("semctl (SEM_NUM_OF_PROC, SETVAL) failed");
		exit (2);
	}

	for (process=0; process<MAKE_CHILD; process++) {
		/* Parent code forks one more process. */
		if ((child_pid = fork ()) < 0) {
			perror ("Fork error");
			exit (3);
		} else if (child_pid == 0) {
			/* Child code goes here */
			child (process+1, sem_id, process+2);
		}
	}
	/* Real parent code goes here */
	child (0, sem_id, 1);
	return 0;
}

void my_info (int proc, int proc_id, int ctr_jatah, int jatah,
    int sem_id, int periode) {
    fprintf (stdout, "\tProc %d: pid %5d; jatah %d/%d; periode %d; sem_id %d\n",
	proc, proc_id, ctr_jatah, jatah, periode, sem_id);
    sleep (1);
}

void child (int child_id, int sem_id, int jatah) {
    int giliran;
    int periode;
    int old_periode;
    int counter_jatah = jatah;
    union semun arg;

    arg.val = 0;
    /* Get SEM_PERIOD's value. */
    if ((periode = semctl (sem_id, SEM_PERIOD, GETVAL, arg)) == -1) {
	perror ("**semctl (SEM_PERIOD, GETVAL) failed");
	exit (2);
    }
    old_periode = periode;
    while (1) {
	/* Attempt to Down(). */
	if (semop (sem_id, &op_lock[0], 2) <  0) {
	    perror ("semop lock (Down) error.");
	    exit (4);
	}
	/* Ok, we got the semaphore.. Entering critical section. */
	/* Whose TURN is it?  Get SEM_TURN's value. */
	arg.val = 0;
	if ((giliran = semctl (sem_id, SEM_TURN, GETVAL, arg)) == -1) {
	    perror ("semctl (SEM_TURN, GETVAL) failed");
	    exit (2);
	}
	/* If this is process' turn:
	** 1. It prints its own message.
	** 2. Set SEM_TURN so other process could gain their turns.
	** 3. It also decrements its own counter_jatah.
	*/
	if (giliran == child_id) {
	    if (counter_jatah > 0) {
		my_info (child_id+1, getpid(), counter_jatah,
			 jatah, sem_id, periode);
		/* Decrease counter_jatah. If it is equal zero, decrease
		** number of process involved. */
		if (--counter_jatah == 0) {
		    /* Decrease number of process. If it is equal zero,
		    ** decrease number of period. */
		    arg.val = 0;
		    if ((num_of_process = semctl (sem_id, SEM_NUM_OF_PROC,
		    				  GETVAL, arg)) == -1) {
		        perror ("semctl (SEM_NUM_OF_PROC, GETVAL) failed");
		        exit (2);
		    }
		    if (--num_of_process == 0) {
		        /* Get number of period from SEM_PERIOD. */
		        arg.val = 0;
		        if ((periode = semctl (sem_id, SEM_PERIOD, GETVAL,
		        		       arg)) == -1) {
		            perror ("semctl (SEM_PERIOD, GETVAL) failed");
		            exit (2);
		        }
			/* decrement the value. */
			/* Set SEM_PERIOD with decremented value. */
			arg.val = periode--;
			if (semctl (sem_id, SEM_PERIOD, SETVAL, arg) == -1) {
			    perror ("semctl (SEM_PERIOD, SETVAL) failed");
			    exit (2);
			}
		    }
		    arg.val = num_of_process;
		    if (semctl (sem_id, SEM_NUM_OF_PROC, SETVAL, arg) ==  -1) {
			perror ("semctl (SEM_NUM_OF_PROC, SETVAL) failed");
			exit (2);
		    }
		}
	    }
            /* Set SEM_TURN for other process. */
	    giliran = (giliran - 1) + NUM_OF_PROCESS;
            arg.val = giliran % NUM_OF_PROCESS;
            if (semctl (sem_id, SEM_TURN, SETVAL, arg) == -1) {
                perror ("semctl (SEM_TURN, SETVAL) failed");
                exit (2);
            }
            /* If num_of_process becomes zero, set SEM_NUM_OF_PROC's value
            ** back to NUM_OF_PROCESS. Decrements SEM_PERIOD's value.
            */
            arg.val = 0;
            if ((num_of_process = semctl (sem_id, SEM_NUM_OF_PROC,
            				  GETVAL, arg)) == -1) {
            	perror ("semctl (SEM_NUM_OF_PROC, GETVAL) failed");
            	exit (2);
            }
            if (num_of_process == 0) {
                arg.val = 0;
                if ((periode = semctl (sem_id, SEM_PERIOD, GETVAL, arg)) == -1) {
                    perror ("semctl (SEM_PERIOD, GETVAL) failed");
                    exit (2);
                }
                arg.val = --periode;
                if ((semctl (sem_id, SEM_PERIOD, SETVAL, arg)) == -1) {
                    perror ("semctl (SEM_PERIOD, SETVAL) failed");
                    exit (2);
                }
                arg.val = NUM_OF_PROCESS;
                if ((semctl (sem_id, SEM_NUM_OF_PROC, SETVAL, arg)) == -1) {
                    perror ("semctl (SEM_NUM_OF_PROC, SETVAL) failed");
                    exit (2);
                }
            }
	    /* If counter_jatah equals zero, check if new
	    ** SEM_PERIOD's value differs from old_periode. */
	    arg.val = 0;
	    if ((periode = semctl (sem_id, SEM_PERIOD, GETVAL, arg)) == -1) {
	        perror ("semctl (SEM_PERIOD, GETVAL) failed");
	        exit (2);
	    }
	    if (periode != old_periode) {
		counter_jatah = jatah;
		old_periode = periode;
	    }
        }
        if (periode == 0)
            exit (0);

        if (semop (sem_id, &op_unlock[0], 1) < 0) {
            perror ("semop unlock (Up) error");
            exit (4);
        }
    }
}

