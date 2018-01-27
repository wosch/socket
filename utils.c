/*

$Header: utils.c[1.14] Wed Sep  9 16:34:03 1992 nickel@cs.tu-berlin.de proposed $
This file is part of socket(1).
Copyright (C) 1992 by Juergen Nickelsen <nickel@cs.tu-berlin.de>
Please read the file COPYRIGHT for further details.

*/

#ifdef sgi
#define _BSD_SIGNALS
#define SIG_HANDLER_RET int
#else /* !sgi */
#define SIG_HANDLER_RET void
#endif

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#ifdef ISC
#define WNOHANG 1
#else
#include <sys/resource.h>
#endif
#include "globals.h"


/* Signal handler, print message and exit */
SIG_HANDLER_RET exitsig(sig)
int sig ;
{
    if (sig != SIGUSR1) {
	fprintf(stderr, "\n%s occured, exiting\n", sys_siglist[sig]) ;
    }
    exit(-sig) ;
}

/* Give usage message */
void usage()
{
    static char ustring[] =
	"Usage: %s [-bclqrvw] [-p prog] [-s | host] port\n" ;

    fprintf(stderr, ustring, progname) ;
}

/* perror with progname */
void perror2(s)
char *s ;
{
    fprintf(stderr, "%s: ", progname) ;
    perror(s) ;
}

/* is s a number? */
int is_number(s)
char *s ;
{
    while (*s) {
	if (*s < '0' || *s > '9') {
	    return 0 ;
	}
	s++ ;
    }
    return 1 ;
}

/* set up signal handling. All except TSTP, CONT, CLD, and QUIT
 * are caught with exitsig(). */
init_signals()
{
    int i ;
#ifdef SIG_SETMASK		/* only with BSD signals */
    static struct sigvec svec = { exitsig, ~0, 0 } ;
#endif

    initialize_siglist() ;	/* shamelessly stolen from BASH */
    
    for (i = 0; i < NSIG; i++) {
	switch (i) {
#ifdef SIGTSTP
	  case SIGTSTP:
	  case SIGTTOU:
	  case SIGTTIN:
	  case SIGSTOP:
	  case SIGCONT:
	    continue ;
#endif
#if !defined (SIGCHLD) && defined (SIGCLD)
#define SIGCHLD SIGCLD
#endif
#ifdef SIGCHLD
	  case SIGCHLD:
	    continue ;
#endif
#ifdef SIGWINCH
	  case SIGWINCH:	/* it is ridiculous to exit on WINCH */
	    continue ;
#endif
	  case SIGQUIT:		/* if the user wants a core dump, */
	    continue ;		/* they can have it. */
	  default:	    
#ifdef SIG_SETMASK
	    sigvec(i, &svec, NULL) ;
#else
	    signal(i, exitsig) ;
#endif
	}
    }
}

/* connect stdin with prog's stdout/stderr and stdout
 * with prog's stdin. */
void open_pipes(prog)
char *prog ;
{
    int from_cld[2] ;		/* from child process */
    int to_cld[2] ;		/* to child process */

    /* create pipes */
    if (pipe(from_cld) == -1) {
	perror2("pipe") ;
	exit(errno) ;
    }
    if (pipe(to_cld) == -1) {
	perror2("pipe") ;
	exit(errno) ;
    }

    /* for child process */
    switch (fork()) {
      case 0:			/* this is the child process */
	/* connect stdin to pipe */
	close(0) ;
	close(to_cld[1]) ;
	dup2(to_cld[0], 0) ;
	close(to_cld[0]) ;
	/* connect stdout to pipe */
	close(1) ;
	close(from_cld[0]) ;
	dup2(from_cld[1], 1) ;
	/* connect stderr to pipe */
	close(2) ;
	dup2(from_cld[1], 2) ;
	close(from_cld[1]) ;
	/* call program via sh */
	execl("/bin/sh", "sh", "-c", prog, NULL) ;
	perror2("exec /bin/sh") ;
	/* terminate parent silently */
	kill(getppid(), SIGUSR1) ;
	exit(255) ;
      case -1:
	perror2("fork") ;	/* fork failed */
	exit(errno) ;
      default:			/* parent process */
	/* connect stderr to pipe */
	close(0) ;
	close(from_cld[1]) ;
	dup2(from_cld[0], 0) ;
	close(from_cld[0]) ;
	/* connect stderr to pipe */
	close(1) ;
	close(to_cld[0]) ;
	dup2(to_cld[1], 1) ;
	close(to_cld[1]) ;
    }
}

/* remove zombie child processes */
void wait_for_children()
{
    int wret, status ;
#ifndef ISC
    struct rusage rusage ;
#endif

    /* Just do a wait, forget result */
#ifndef ISC
    while ((wret = wait3(&status, WNOHANG, &rusage)) > 0) ;
#else
    while ((wret = waitpid(-1, &status, WNOHANG)) > 0) ;
#endif
}

/* expand LF characters to CRLF and adjust *sizep */
void add_crs(from, to, sizep)
char *from, *to ;		/* *from is copied to *to */
int *sizep ;
{
    int countdown ;		/* counter */

    countdown = *sizep ;
    while (countdown) {
	if (*from == '\n') {
	    *to++ = '\r' ;
	    (*sizep)++ ;
	}
	*to++ = *from++ ;
	countdown-- ;
    }
}

/* strip CR characters from buffer and adjust *sizep */
void strip_crs(from, to, sizep)
char *from, *to ;		/* *from is copied to *to */
int *sizep ;
{

    int countdown ;		/* counter */

    countdown = *sizep ;
    while (countdown) {
	if (*from == '\r') {
	    from++ ;
	    (*sizep)-- ;
	} else {
	    *to++ = *from++ ;
	}
	countdown-- ;
    }
}

#define NULL_DEVICE "/dev/null"

/* put yourself in the background */
void background()
{
    int child_pid ;		/* PID of child process */
    int nulldev_fd ;		/* file descriptor for null device */

    child_pid = fork() ;
    switch (child_pid) {
      case -1:
	perror2("fork") ;
	exit(1) ;
      case 0:
#ifdef NOSETSID
	ioctl(0, TIOCNOTTY, 0) ;
#else
	setsid() ;
#endif
	chdir("/") ;
	if ((nulldev_fd = open(NULL_DEVICE, O_RDWR, 0)) != -1) {
	    int i ;

	    for (i = 0; i < 3; i++) {
		if (isatty(i)) {
		    dup2(nulldev_fd, i) ;
		}
	    }
	    close(nulldev_fd) ;
	}
	break ;
      default:
	exit(0) ;
    }
}
