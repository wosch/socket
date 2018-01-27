/*

$Header: socketp.c[1.4] Sun Aug  9 03:48:03 1992 nickel@cs.tu-berlin.de proposed $
This file is part of socket(1).
Copyright (C) 1992 by Juergen Nickelsen <nickel@cs.tu-berlin.de>
Please read the file COPYRIGHT for further details.

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "globals.h"

/*
 * create a server socket on PORT accepting QUEUE_LENGTH connections
 */
int create_server_socket(port, queue_length)
int port ;
int queue_length ;
{
    struct sockaddr_in sa ;
    int s;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	return -1 ;
    }

    bzero((char *) &sa, sizeof(sa)) ;
    sa.sin_family = AF_INET ;
    sa.sin_addr.s_addr = htonl(INADDR_ANY) ;
    sa.sin_port = htons(port) ;

    if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
	return -1 ;
    }
    if (listen(s, 1) < 0) {
	return -1 ;
    }

    return s ;
}


/* create a client socket connected to PORT on HOSTNAME */
int create_client_socket(hostname, port)
char **hostname ;
int port ;
{
    struct sockaddr_in sa ;
    struct hostent *hp ;
    int a, s ;
    long addr ;


    bzero(&sa, sizeof(sa)) ;
    if ((addr = inet_addr(*hostname)) != -1) {
	/* is Internet addr in octet notation */
	bcopy(&addr, (char *) &sa.sin_addr, sizeof(addr)) ; /* set address */
	sa.sin_family = AF_INET ;
    } else {
	/* do we know the host's address? */
	if ((hp = gethostbyname(*hostname)) == NULL) {
	    return -2 ;
	}
	*hostname = hp->h_name ;
	bcopy(hp->h_addr, (char *) &sa.sin_addr, hp->h_length) ;
	sa.sin_family = hp->h_addrtype ;
    }

    sa.sin_port = htons((u_short) port) ;

    if ((s = socket(sa.sin_family, SOCK_STREAM, 0)) < 0) { /* get socket */
	return -1 ;
    }
    if (connect(s, &sa, sizeof(sa)) < 0) {                  /* connect */
	close(s) ;
	return -1 ;
    }
    return s ;
}

/* return the port number for service NAME_OR_NUMBER. If NAME is non-null,
 * the name is the service is written there.
 */
int resolve_service(name_or_number, protocol, name)
char *name_or_number ;
char *protocol ;
char **name ;
{
    struct servent *servent ;
    int port ;

    if (is_number(name_or_number)) {
	port = atoi(name_or_number) ;
	if (name != NULL) {
	    servent = getservbyport(htons(port), "tcp") ;
	    if (servent != NULL) {
		*name = servent->s_name ;
	    } else {
		*name = NULL ;
	    }
	}
	return port ;
    } else {
	servent = getservbyname(name_or_number, "tcp") ;
	if (servent == NULL) {
	    return -1 ;
	}
	if (name != NULL) {
	    *name = servent->s_name ;
	}
	return ntohs(servent->s_port) ;
    }
}

/*EOF*/
