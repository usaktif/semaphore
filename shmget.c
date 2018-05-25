/*
** shmget.c: Illustrate the shmget() system call.
**
**	This is a simple exerciser of the shmget () system call.
** It prompts for the arguments, makes the call, and reports the results.
*/

/**
 * LOG
 * 20120330 Conditional compilation for Cygwin environment.
 * - ipc.h and shm.h files are located under sys/ directory.
 */

#include <stdio.h>
#include <sys/types.h>
#ifdef LINUX
#include <linux/ipc.h>
#include <linux/shm.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#ifdef AIX
#include <sys/mode.h>
#endif

extern void exit ();
extern void perror ();

main () {
  key_t key;	/* key to be passed to shmget() */
  int shmflg;	/* shmflg to be passed to shmget() */
  int shmid;	/* return value from shmget() */
  int size;	/* size to be passed to shmget() */

  (void) fprintf (stderr,
  "All numeric input is expected to follow C conventions:\n");
  (void) fprintf (stderr, "\t0x... is interpreted as hexadecimal,\n");
  (void) fprintf (stderr, "\t0...  is interpreted as octal,\n");
  (void) fprintf (stderr, "\totherwise, decimal.\n");

  /* Get the key. */
  (void) fprintf (stderr, "IPC_PRIVATE == %#lx\n", IPC_PRIVATE);
  (void) fprintf (stderr, "Enter desired key: ");
  (void) scanf ("%li", &key);

  /* Get the size of the segment. */
  /*
  (void) fprintf (stderr, "Expected size for shm is: %d <= size <= %d\n",
    SHMMIN, SHMMAX);
   */
  (void) fprintf (stderr, "Enter desired size: ");
  (void) scanf ("%i", &size);

  /* Get the shmflg value. */
  (void) fprintf (stderr, "Expected flags for the shmflg argument are:\n");
  (void) fprintf (stderr, "\tIPC_CREAT =\t%#8.8o\n", IPC_CREAT);
  (void) fprintf (stderr, "\tIPC_EXCL =\t%#8.8o\n", IPC_EXCL);
  (void) fprintf (stderr, "\towner read =\t%#8.8o\n", 0400);
  (void) fprintf (stderr, "\towner write =\t%#8.8o\n", 0200);
  (void) fprintf (stderr, "\tgroup read =\t%#8.8o\n", 040);
  (void) fprintf (stderr, "\tgroup write =\t%#8.8o\n", 020);
  (void) fprintf (stderr, "\tother read =\t%#8.8o\n", 04);
  (void) fprintf (stderr, "\tother write =\t%#8.8o\n", 02);
  (void) fprintf (stderr, "Enter desired shmflg: ");
  (void) scanf ("%i", &shmflg);

  /* Make the call and report the results. */
  (void) fprintf (stderr, "shmget: Calling shmget (%#lx, %d, %#o)\n",
    key, size, shmflg);
  if ((shmid = shmget (key, size, shmflg)) == -1) {
    perror ("shmget: shmget failed");
    exit (1);
  } else {
    (void) fprintf (stderr, "shmget: shmget returned %d\n", shmid);
    exit (0);
  }
  /* NOTREACHED */
}

