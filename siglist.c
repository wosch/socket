/* siglist.c -- signal list for those machines that don't have one. */

/* Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GNU Bash, the Bourne Again SHell.

Modified by Juergen Nickelsen <nickel@cs.tu-berlin.de> for use with
Socket-1.1.

Bash is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

Bash is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Bash; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <signal.h>

#if !defined (NSIG)
#  if defined (_NSIG)
#    define NSIG _NSIG
#  else
#    define NSIG 64
#  endif /* !_NSIG */
#endif /* !NSIG */

char *sys_siglist[NSIG];

extern *malloc ();

initialize_siglist ()
{
  register int i;

  for (i = 0; i < NSIG; i++)
    sys_siglist[i] = (char *)0x0;

  sys_siglist[0] = "Bogus signal";

#if defined (SIGHUP)
  sys_siglist[SIGHUP] = "Hangup signal";
#endif

#if defined (SIGINT)
  sys_siglist[SIGINT] = "Interrupt";
#endif

#if defined (SIGQUIT)
  sys_siglist[SIGQUIT] = "Quit signal";
#endif

#if defined (SIGILL)
  sys_siglist[SIGILL] = "Illegal instruction";
#endif

#if defined (SIGTRAP)
  sys_siglist[SIGTRAP] = "BPT trace/trap";
#endif

#if defined (SIGIOT) && !defined (SIGABRT)
#define SIGABRT SIGIOT
#endif

#if defined (SIGABRT)
  sys_siglist[SIGABRT] = "ABORT instruction";
#endif

#if defined (SIGEMT)
  sys_siglist[SIGEMT] = "EMT instruction";
#endif

#if defined (SIGFPE)
  sys_siglist[SIGFPE] = "Floating point exception";
#endif

#if defined (SIGKILL)
  sys_siglist[SIGKILL] = "Kill signal";
#endif

#if defined (SIGBUS)
  sys_siglist[SIGBUS] = "Bus error";
#endif

#if defined (SIGSEGV)
  sys_siglist[SIGSEGV] = "Segmentation fault";
#endif

#if defined (SIGSYS)
  sys_siglist[SIGSYS] = "Bad system call";
#endif

#if defined (SIGPIPE)
  sys_siglist[SIGPIPE] = "Broken pipe condition";
#endif

#if defined (SIGALRM)
  sys_siglist[SIGALRM] = "Alarm clock signal";
#endif

#if defined (SIGTERM)
  sys_siglist[SIGTERM] = "Termination signal";
#endif

#if defined (SIGURG)
  sys_siglist[SIGURG] = "Urgent IO condition";
#endif

#if defined (SIGSTOP)
  sys_siglist[SIGSTOP] = "Stop signal";
#endif

#if defined (SIGTSTP)
  sys_siglist[SIGTSTP] = "Stopped";
#endif

#if defined (SIGCONT)
  sys_siglist[SIGCONT] = "Continue signal";
#endif

#if !defined (SIGCHLD) && defined (SIGCLD)
#define SIGCHLD SIGCLD
#endif

#if defined (SIGCHLD)
  sys_siglist[SIGCHLD] = "Child signal";
#endif

#if defined (SIGTTIN)
  sys_siglist[SIGTTIN] = "Stop (tty input) signal";
#endif

#if defined (SIGTTOU)
  sys_siglist[SIGTTOU] = "Stop (tty output) signal";
#endif

#if defined (SIGIO)
  sys_siglist[SIGIO] = "I/O ready signal";
#endif

#if defined (SIGXCPU)
  sys_siglist[SIGXCPU] = "CPU limit exceeded";
#endif

#if defined (SIGXFSZ)
  sys_siglist[SIGXFSZ] = "File limit exceeded";
#endif

#if defined (SIGVTALRM)
  sys_siglist[SIGVTALRM] = "Alarm (virtual)";
#endif

#if defined (SIGPROF)
  sys_siglist[SIGPROF] = "Alarm (profile)";
#endif

#if defined (SIGWINCH)
  sys_siglist[SIGWINCH] = "Window change";
#endif

#if defined (SIGLOST)
  sys_siglist[SIGLOST] = "Record lock signal";
#endif

#if defined (SIGUSR1)
  sys_siglist[SIGUSR1] = "User signal 1";
#endif

#if defined (SIGUSR2)
  sys_siglist[SIGUSR2] = "User signal 2";
#endif

#if defined (SIGMSG)
  sys_siglist[SIGMSG] = "HFT input data pending signal";
#endif 

#if defined (SIGPWR)
  sys_siglist[SIGPWR] = "power failure imminent signal";
#endif 

#if defined (SIGDANGER)
  sys_siglist[SIGDANGER] = "system crash imminent signal";
#endif 

#if defined (SIGMIGRATE)
  sys_siglist[SIGMIGRATE] = "Process migration";
#endif 

#if defined (SIGPRE)
  sys_siglist[SIGPRE] = "Programming error signal";
#endif 

#if defined (SIGGRANT)
  sys_siglist[SIGGRANT] = "HFT monitor mode granted signal";
#endif 

#if defined (SIGRETRACT)
  sys_siglist[SIGRETRACT] = "HFT monitor mode retracted signal";
#endif 

#if defined (SIGSOUND)
  sys_siglist[SIGSOUND] = "HFT sound sequence has completed signal";
#endif 

  for (i = 0; i < NSIG; i++)
    {
      if (!sys_siglist[i])
	{
	  sys_siglist[i] =
	    (char *) malloc (10 + strlen ("Unknown Signal #"));

	  sprintf (sys_siglist[i], "Unknown Signal #%d", i);
	}
    }
}
