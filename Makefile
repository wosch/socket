# Makefile for Socket
#
# $Header: Makefile[1.23] Wed Sep  9 19:03:26 1992 nickel@cs.tu-berlin.de proposed $
#

### adjust these to your taste
### This is set up to install socket as /usr/local/bin/socket and
### socket.1 in /usr/local/man/man1/socket.1
### Make sure the target directories exist before doing a "make install".

INSTALLBASE = /usr/local
INSTALLBINPATH = $(INSTALLBASE)/bin
INSTALLBINMODE = 755
INSTALLMANPATH = $(INSTALLBASE)/man
INSTALLMANMODE = 444
CC = cc
CFLAGS  = $(SWITCHES) -O
LDFLAGS = $(SWITCHES) -s

### You may need to uncomment some lines below for your operating
### system:

### 4.3 BSD:
# SWITCHES = -DNOSETSID

### HP-UX:
# SYSLIBS  = -lBSD

### ISC 3.0:
# SYSLIBS  = -linet -lcposix
# SWITCHES = -DISC

### System V Rel. 4:
# SYSLIBS  = -lc -lsocket -lnsl -L/usr/ucblib -lucb
# SWITCHES = -I/usr/ucbinclude

### Sequent:
# SYSLIBS  = -lseq
# SWITCHES = -Dstrrchr=rindex -DSEQUENT -DNOSETSID
### (I have been reported troubles with the "installmanuals" target on
### a Sequent, so you might have to copy the manual page yourself.)

### CDC 4680 EP/IX: (I know it *has* setsid(2), but not with bsd43)
# SWITCHES = -systype bsd43 -DNOSETSID



### It should not be necessary to change anything below this line.
##################################################################

MAKE = make
SHELL = /bin/sh
BASE = /home/stone/nickel/src
NODEPATH = socket
NODENAME = "Socket"
TARGET = socket
VERSIONFILE = 	so_release.c
VERSIONOBJECT =	so_release.o
PROGSOURCES = socket.c siglist.c io.c utils.c socketp.c
SOURCES = BLURB README COPYRIGHT INSTALL CHANGES \
	socket.1 \
	$(PROGSOURCES) \
	rfinger.sh fingerd.sh
HEADERS = globals.h patchlevel.h
MANUALS = $(MAN1)
MAN1 = socket.1
COMPONENTS = $(SOURCES) $(HEADERS) $(MANUALS) Makefile Dependencies
OBJECTS = $(VERSIONOBJECT) socket.o siglist.o io.o utils.o socketp.o

all: +all $(ALLTARGETS)

targets: $(TARGET)

$(TARGET): $(LOCALLIBS) $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LOCALLIBS) $(SYSLIBS)

tags: TAGS
TAGS: $(PROGSOURCES) $(HEADERS)
	etags $(PROGSOURCES) $(HEADERS)

installtargets: $(INSTALLBINPATH)/$(TARGET) installmanuals

$(INSTALLBINPATH)/$(TARGET): $(TARGET)
	@-echo "installing $(TARGET) in $(INSTALLBINPATH)"; \
	if [ -f $(INSTALLBINPATH)/$(TARGET) ] && \
	   [ ! -w $(INSTALLBINPATH)/$(TARGET) ]; \
	then \
	  chmod u+w $(INSTALLBINPATH)/$(TARGET); \
	fi; \
	cp $(TARGET) $(INSTALLBINPATH)/$(TARGET); \
	chmod $(INSTALLBINMODE) $(INSTALLBINPATH)/$(TARGET); 

installmanuals: $(MANUALS)
	@-_manuals="$(MAN1)"; \
	for i in $$_manuals; \
	do \
	  echo "installing $$i in $(INSTALLMANPATH)/man1"; \
	  if [ -f $(INSTALLMANPATH)/man1/$$i ] && \
	     [ ! -w $(INSTALLMANPATH)/man1/$$i ]; \
	  then \
	    chmod u+w $(INSTALLMANPATH)/man1/$$i; \
	  fi; \
	  cp $$i $(INSTALLMANPATH)/man1/$$i; \
	  chmod $(INSTALLMANMODE) $(INSTALLMANPATH)/man1/$$i; \
	done

doclean:
	rm -f $(TARGET) $(ALIASES) $(OBJECTS) core *~

##
## following internals of shapeTools Release Management System

subsystems: # none

install: +install $(ALLTARGETS)

clean: +clean $(ALLTARGETS)

+all:
	@-if [ -n "$(ALLTARGETS)" ]; \
	then : ; \
	else \
	  $(MAKE) ALLTARGETS="subsystems targets" MAINTARGET=all \
		BASE=$(BASE) \
		SWITCHES="$(SWITCHES)" \
		INSTALLBASE=$(INSTALLBASE) \
		INSTALLBINPATH=$(INSTALLBINPATH) \
		INSTALLBINMODE=$(INSTALLBINMODE) \
		INSTALLMANPATH=$(INSTALLMANPATH) \
		INSTALLMANMODE=$(INSTALLMANMODE) \
		MAKE="$(MAKE)" \
		SHELL="$(SHELL)" \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
		SYSLIBS="$(SYSLIBS)" all; \
	fi

+install:
	@-if [ -n "$(ALLTARGETS)" ]; \
	then : ; \
	else \
	  $(MAKE) ALLTARGETS="subsystems installtargets" \
		MAINTARGET=install \
		BASE=$(BASE) \
		SWITCHES="$(SWITCHES)" \
		INSTALLBASE=$(INSTALLBASE) \
		INSTALLBINPATH=$(INSTALLBINPATH) \
		INSTALLBINMODE=$(INSTALLBINMODE) \
		INSTALLMANPATH=$(INSTALLMANPATH) \
		INSTALLMANMODE=$(INSTALLMANMODE) \
		MAKE="$(MAKE)" \
		SHELL="$(SHELL)" \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
		SYSLIBS="$(SYSLIBS)" install; \
	fi

+clean:
	@-if [ -n "$(ALLTARGETS)" ]; \
	then : ; \
	else \
	  $(MAKE) ALLTARGETS="subsystems doclean" MAINTARGET=clean \
		BASE=$(BASE) \
		SWITCHES="$(SWITCHES)" \
		INSTALLBASE=$(INSTALLBASE) \
		INSTALLBINPATH=$(INSTALLBINPATH) \
		INSTALLBINMODE=$(INSTALLBINMODE) \
		INSTALLMANPATH=$(INSTALLMANPATH) \
		INSTALLMANMODE=$(INSTALLMANMODE) \
		INCLUDEPATH=$(INCLUDEPATH) \
		MAKE="$(MAKE)" \
		SHELL="$(SHELL)" \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
		SYSLIBS="$(SYSLIBS)" clean; \
	fi
