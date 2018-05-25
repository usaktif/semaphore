/*
** semop.c:	Illustrate the semop() system call.
**
**	This is a simple exerciser of the semop() system call. It allows
** you to set up arguments for semop(), make the call, and reports the
** results repeatedly on one semaphore set. You must have read
** permission on the semaphore set or this exerciser will fail. (It needs
** read permission to get the number of semaphores in the set and report
** their values before and after calls to semop().)
*/

/**
 * LOG
 * 20120330 Conditional compilation for Cygwin environment:
 * - ipc.h and sem.h are located in sys/ directory.
 */

#include <stdio.h>
#include <sys/types.h>
#ifdef LINUX
#include <linux/types.h>
#include <linux/ipc.h>
#include <linux/sem.h>
#else
#include <sys/ipc.h>
#include <sys/sem.h>
#endif
#ifdef AIX
#include <sys/mode.h>

union semun {
    int val;			/* used for SETVAL only */
    struct semid_ds *buf;	/* used for IPC_STAT and IPC_SET */
    ushort *array;		/* used for IPC_GETALL & IPC_SETALL */
};
#endif

#ifdef CYGWIN
union semun {
    int val;			/* used for SETVAL only */
    struct semid_ds *buf;	/* used for IPC_STAT and IPC_SET */
    ushort *array;		/* used for IPC_GETALL & IPC_SETALL */
};
#endif

static int ask();
extern void exit();
extern void free();
extern char* malloc();
extern void perror();

static struct semid_ds semid_ds;	/* status of semaphore set */

static char error_mesg1[] = "semop: Can't allocate space for %d\
    semaphore values.  Giving up.\n";
static char error_mesg2[] = "semop: Can't allocate space for %d\
    sembuf structures.  Giving up.\n";

main () {
    register int i;	/* work area */
    int nsops;		/* number of operations to be performed */
    int semid;		/* semid of semaphore set */
    struct sembuf *sops;/* ptr to operations to be performed */

    (void) fprintf (stderr,
        "All numeric input is expected to follow C conventions:\n");
    (void) fprintf (stderr, "\t0x... is interpreted as hexadecimal,\n");
    (void) fprintf (stderr, "\t0...  is interpreted as octal,\n");
    (void) fprintf (stderr, "\totherwise, decimal.\n");

    /* Loop until the invoker doesn't want to do anymore. */
    while (nsops = ask (&semid, &sops)) {
        /* Initialize the array of operations to be performed. */
        for (i=0; i<nsops; i++) {
            (void) fprintf (stderr,
                "\nEnter desired values for operations %d of %d.\n",
                i+1, nsops);
            (void) fprintf (stderr,
                "sem_num (valid values are 0 <= sem_num < %d): ",
                semid_ds.sem_nsems);
            (void) scanf ("%hi", &sops[i].sem_num);
            (void) fprintf (stderr, "sem_op: ");
            (void) scanf ("%hi", &sops[i].sem_op);
            (void) fprintf (stderr,
                "Expected flags in sem_flg are:\n");
            (void) fprintf (stderr, "\tIPC_NOWAIT =\t%#6.6o\n", IPC_NOWAIT);
            (void) fprintf (stderr, "\tSEM_UNDO =\t%#6.6o\n", SEM_UNDO);
            (void) fprintf (stderr, "sem_flg: ");
            (void) scanf ("%hi", &sops[i].sem_flg);
        }

        /* Recap the call to be made. */
        (void) fprintf (stderr,
            "\nsemop: Calling semop  (%d, &sops, %d) with:",
            semid, nsops);
        for (i=0; i<nsops; i++) {
            (void) fprintf (stderr, "\nsops[%d].sem_num = %d, ", i,
                sops[i].sem_num);
            (void) fprintf (stderr, "sem_op = %d, ", sops[i].sem_op);
            (void) fprintf (stderr, "sem_flg = %#o\n", sops[i].sem_flg);
        }

        /* Make the semop() call and report the results. */
        if ((i = semop (semid, sops, nsops)) == -1) {
            perror ("semop: semop failed.");
        } else {
            (void) fprintf (stderr, "semop: semop returned %d\n", i);
        }
    }
    /* NOTREACHED */
}

/*
**	Ask user if (s)he wants to continue.
**
** On the first call:
**	Get the semid to be processed and supply it to the caller.
** On each call:
**	1. Print current semaphore values.
**	2. Ask user how many operations are to be performed on next call to
**	   semop. Allocate an array of sembuf structures sufficient for the
**	   job and set caller supplied pointer to that array. (The array
**	   is reused on subsequent calls as long as it is big enough. If
**	   it isn't big enough, it is freed and a larger array is allocated.)
*/
static
ask (semidp, sopsp)
int *semidp;		/* pointer to semid (only used first time) */
struct sembuf **sopsp;
{
    static union semun arg;	/* argument to semctl */
    int i;			/* work area */
    static int nsops = 0;	/* size of currently allocated sembuf array */
    static int semid = -1;	/* semid supplied by user */
    static struct sembuf *sops;	/* pointer to allocated array */

    if (semid <  0) {
        /* First call; get semid from user and the current state of
        the semaphore set. */
        (void) fprintf (stderr,
            "Enter semid of the semaphore set you want to use: ");
        (void) scanf ("%i", &semid);
        *semidp = semid;
        arg.buf = &semid_ds;
        if (semctl (semid, 0, IPC_STAT, arg) == -1) {
            perror ("semop: semctl (IPC_STAT) failed.");
            /* Note that if semctl fails, semid_ds remains filled with
            zeroes, so later test for number of semaphores will be zero. */
            (void) fprintf (stderr,
                "Before and after values will not be printed.\n");
        } else {
            if ((arg.array = (ushort *) malloc ((unsigned)
                (sizeof (ushort) * semid_ds.sem_nsems))) == NULL) {
                (void) fprintf (stderr, error_mesg1, semid_ds.sem_nsems);
                exit (1);
            }
        }
    }

    /* Print current semaphore values. */
    if (semid_ds.sem_nsems) {
        (void) fprintf (stderr, "There are %d semaphore(s) in the set.\n",
            semid_ds.sem_nsems);
        if (semctl (semid, 0, GETALL, arg) == -1) {
            perror ("semop: semctl (GETALL) failed.");
        } else {
            (void) fprintf (stderr, "Current semaphore values are:");
            for (i=0; i<semid_ds.sem_nsems;
                (void) fprintf (stderr, " %d", arg.array[i++]))
                ;
            (void) fprintf (stderr, "\n");
        }
    }

    /* Find out how many operations are going to be done in the next
    call and allocate enough space to do it. */
    (void) fprintf (stderr, "How many semaphore operations do you want %s\n",
        "on the next call to semop() ?");
    (void) fprintf (stderr, "Enter 0 or control-D to quit: ");
    i = 0;
    if (scanf ("%i", &i) == EOF || i == 0)
        exit (0);
    if (i > nsops) {
        if (nsops)
            free ((char *) sops);
        nsops = i;
        if ((sops = (struct sembuf *) malloc ((unsigned) (nsops *
            sizeof (struct sembuf)))) == NULL) {
            (void) fprintf (stderr, error_mesg2, nsops);
            exit (2);
        }
    }
    *sopsp = sops;
    return (i);
}

