/*
** semget.c:	Illustrate the semget() system call.
**
**	This is a simple exerciser of the semget() system call.
** It prompts for the arguments, makes the call, and reports the
** results.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

extern void exit();
extern void perror ();

main () {
	key_t key;	/* key to be passed to semget() */
	int semflg;	/* semflg to be passed to semget() */
	int nsems;	/* nsems to be passed to semget() */
	int semid;	/* return value from semget() */

	(void) fprintf (stderr,
		"All numeric input is expected to follow C conventions:\n");
	(void) fprintf (stderr, "\t0x... is interpreted as hexadecimal,\n");
	(void) fprintf (stderr, "\t0...  is interpreted as octall,\n");
	(void) fprintf (stderr, "\totherwise, decimal.\n");

	(void) fprintf (stderr, "IPC_PRIVATE == %#lx\n", IPC_PRIVATE);
	(void) fprintf (stderr, "Enter desired key: ");
	(void) scanf ("%li", &key);

	(void) fprintf (stderr, "Enter desired nsems value: ");
	(void) scanf ("%i", &nsems);

	(void) fprintf (stderr, "\nExpected flags for semflg are:\n");
	(void) fprintf (stderr, "\tIPC_EXCL = \t%#8.8o\n", IPC_EXCL);
	(void) fprintf (stderr, "\tIPC_CREAT = \t%#8.8o\n", IPC_CREAT);
	(void) fprintf (stderr, "\towner read = \t%#8.8o\n", 0400);
	(void) fprintf (stderr, "\towner alter = \t%#8.8o\n", 0200);
	(void) fprintf (stderr, "\tgroup read = \t%#8.8o\n", 040);
	(void) fprintf (stderr, "\tgroup alter = \t%#8.8o\n", 020);
	(void) fprintf (stderr, "\tother read = \t%#8.8o\n", 04);
	(void) fprintf (stderr, "\tother alter = \t%#8.8o\n", 02);
	(void) fprintf (stderr, "Enter desired semflg value: ");
	(void) scanf ("%i", &semflg);

	(void) fprintf (stderr, "\nsemget: Calling semget (%#lx, %d, %#o)\n",
		key, nsems, semflg);

	if ((semid = semget (key, nsems, semflg)) == -1) {
		perror ("semget: semget failed");
		exit (1);
	} else {
		(void) fprintf (stderr, "semget: semget succeeded: semid = %d\n",
			semid);
		exit (0);
	}
	/* NOTREACHED */
}

