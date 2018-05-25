/*
** shmctl.c: Illustrate the shmctl() system call.
**
** This is a simple exerciser of the shmctl() system call.  It allows
** you to perform one control operation on one shared memory segment.
** (Some operations are done for the user whether requested or not.  It gives
** up immediately if any control operation fails.  Be careful not to set
** permissions to preclude read permission; you won't be able to reset the
** permissions with this code if you do.)
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
#include <time.h>
#ifdef AIX
#include <sys/mode.h>
#endif

static void do_shmctl();
extern void exit ();
extern void perror ();

main () {
  int cmd;
  int shmid;
  struct shmid_ds shmid_ds;

  (void) fprintf (stderr,
  "All numeric input is expected to follow C conventions:\n");
  (void) fprintf (stderr, "\t0x... is interpreted as hexadecimal,\n");
  (void) fprintf (stderr, "\t0...  is interpreted as octal,\n");
  (void) fprintf (stderr, "\totherwise, decimal.\n");

  /* Get shmid and cmd. */
  (void) fprintf (stderr, "Enter the shmid for the desired segment: ");
  (void) scanf ("%i", &shmid);

  (void) fprintf (stderr, "Valid shmctl cmd values are:\n");
  (void) fprintf (stderr, "\tIPC_RMID =\t%d\n", IPC_RMID);
  (void) fprintf (stderr, "\tIPC_SET =\t%d\n", IPC_SET);
  (void) fprintf (stderr, "\tIPC_STAT =\t%d\n", IPC_STAT);
  /*
  (void) fprintf (stderr, "\tSHM_LOCK =\t%d\n", SHM_LOCK);
  (void) fprintf (stderr, "\tSHM_UNLOCK =\t%d\n", SHM_UNLOCK);
  */
  (void) fprintf (stderr, "Enter the desired cmd value: ");
  (void) scanf ("%i", &cmd);

  switch (cmd) {
    case IPC_STAT:
      /* Get shared memory segment status. */
      break;
    case IPC_SET:
      /* Set owner UID and GID and permissions. */
      /* Get and print current values. */
      do_shmctl (shmid, IPC_STAT, &shmid_ds);

      /* Set UID, GID and permissions to be loaded. */
      (void) fprintf (stderr, "\nEnter desired shm_perm.uid: ");
      (void) scanf ("%hi", &shmid_ds.shm_perm.uid);
      (void) fprintf (stderr, "Enter desired shm_perm.gid: ");
      (void) scanf ("%hi", &shmid_ds.shm_perm.gid);
      (void) fprintf (stderr, "Note: Keep read permission for yourself.\n");
      (void) fprintf (stderr, "Enter desired shm_perm.mode: ");
      (void) scanf ("%hi", &shmid_ds.shm_perm.mode);
      break;
    case IPC_RMID:
      /* Remove the segment when the last attach point is detached. */
      break;
    /*
    case SHM_LOCK:
      // Lock the shared memory segment.
      break;
    case SHM_UNLOCK:
      // Unlock the shared memory segment.
      break;
     */
    default:
      // Unknown command will be passed to shmctl.
      break;
  }
  do_shmctl (shmid, cmd, &shmid_ds);
  exit (0);
  /* NOTREACHED */
}

/*
** Display the arguments being passed to shmctl(), call shmctl(), and
** report the results.
** If shmctl() fails, do not return; this example doesn't deal with
** errors, it just reports them.
*/
static void do_shmctl (shmid, cmd, buf)
int shmid;
int cmd;
struct shmid_ds *buf;
{
  register int rtrn;	/* hold area */

  (void) fprintf (stderr, "shmctl: Calling shmctl (%d, %d, buf)\n",
    shmid, cmd);
  if (cmd == IPC_SET) {
    (void) fprintf (stderr, "\tbuf->shm_perm.uid == %d\n", buf->shm_perm.uid);
    (void) fprintf (stderr, "\tbuf->shm_perm.gid == %d\n", buf->shm_perm.gid);
    (void) fprintf (stderr, "\tbuf->shm_perm.mode == %#o\n",
      buf->shm_perm.mode);
  }
  if ((rtrn = shmctl (shmid, cmd, buf)) == -1) {
    perror ("shmctl: shmctl failed");
    exit (1);
  } else {
    (void) fprintf (stderr, "shmctl: shmctl returned %d\n", rtrn);
  }
  if (cmd != IPC_STAT && cmd != IPC_SET)
    return;

  /* Print the current status. */
  (void) fprintf (stderr, "\nCurrent status:\n");
  (void) fprintf (stderr, "\tshm_perm.uid = %d\n", buf->shm_perm.uid);
  (void) fprintf (stderr, "\tshm_perm.gid = %d\n", buf->shm_perm.gid);
  (void) fprintf (stderr, "\tshm_perm.cuid = %d\n", buf->shm_perm.cuid);
  (void) fprintf (stderr, "\tshm_perm.cgid = %d\n", buf->shm_perm.cgid);
  (void) fprintf (stderr, "\tshm_perm.mode = %#o\n", buf->shm_perm.mode);
  (void) fprintf (stderr, "\tshm_perm.key = %#x\n", buf->shm_perm.key);
  (void) fprintf (stderr, "\tshm_segsz = %d\n", buf->shm_segsz);
  (void) fprintf (stderr, "\tshm_lpid = %d\n", buf->shm_lpid);
  (void) fprintf (stderr, "\tshm_cpid = %d\n", buf->shm_cpid);
  (void) fprintf (stderr, "\tshm_nattch = %d\n", buf->shm_nattch);
  (void) fprintf (stderr, "\tshm_atime = %s",
    buf->shm_atime ? ctime (&buf->shm_atime) : "Not set\n");
  (void) fprintf (stderr, "\tshm_dtime = %s",
    buf->shm_dtime ? ctime (&buf->shm_dtime) : "Not set\n");
  (void) fprintf (stderr, "\tshm_ctime = %s", ctime (&buf->shm_ctime));
}

