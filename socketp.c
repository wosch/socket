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
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "globals.h"

extern int is_number A((char *));

#ifndef USE_INET6
/*
 * create a server socket on PORT accepting QUEUE_LENGTH connections
 */
int create_server_socket(port, queue_length)
int port ;
int queue_length ;
{
    struct sockaddr_in sa ;
    int s;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
	return -1 ;

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
#else /* USE_INET6 */
/*
 * create a server socket(s) on PORT accepting QUEUE_LENGTH connections
 *
 * FWD (ifdef USE_INET6):
 * there can be more than one socket; one for each supported address
 * family. This is for portability as not all IPv6 stacks implement
 * the wildcard bind as a bind to *ll IPv4 *and* IPv6 addresses.
 * so we'll just open a socket for each address getaddrinfo() gives
 * back to us. The price of portability...
 */
int *create_server_sockets(port, queue_length)
char **port ;
int queue_length ;
{
    struct addrinfo hints, *r, *res; 
    int *s, *slist, error, maxs;
    int reuse_addr = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  
    hints.ai_protocol = 0;
    error = getaddrinfo(NULL, *port, &hints, &res);

    if (!error) {
	for (maxs = 0, r = res; r; r = r->ai_next, maxs++);
	slist = malloc ((maxs+1) * sizeof(int));
	if (slist) { 
	    slist[0] = maxs; /* max. num of sockets */
          
	    s = slist+1;
	    for (r = res; r; r = r->ai_next) {
		*s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (*s < 0) 
		    continue;

		setsockopt(*s,SOL_SOCKET,SO_REUSEADDR,&reuse_addr,sizeof(reuse_addr));
          
		if (bind(*s, r->ai_addr, r->ai_addrlen) < 0) {
		    close (*s);
		    continue;
		} 
     
		if (listen(*s, 1) < 0) {
		    close (*s);
		    continue;
		}
		s++;
	    }   
	}
    }
    else
	slist = NULL;

    if (res) 
	freeaddrinfo(res);

    if (slist && !slist[0]) {
       free (slist);
       slist = NULL;
    }   
 
    return (slist);
}
#endif /* USE_INET6 */


/* create a client socket connected to PORT on HOSTNAME */
#ifndef USE_INET6
/* create a client socket connected to PORT on HOSTNAME */
int create_client_socket(hostname, port)
char **hostname ;
int port ;
{
    struct sockaddr_in sa ;
    struct hostent *hp ;
    int s ;
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
    if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) { /* connect */
        close(s) ;
        return -1 ;
    }
    return s ;
}

#else /* USE_INET6 */
int create_client_socket(hostname, port)
char **hostname ;
char **port ;
{
    int s, connected, err ;
    struct addrinfo hints, *r, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype= SOCK_STREAM;
    err = getaddrinfo(*hostname, *port, &hints, &res);
    if (res) freeaddrinfo(res);

    if (!err) {
	/* numeric */
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	err = getaddrinfo(*hostname, *port, &hints, &res);
	if (err) s = -2;
    } else { 
	/* non-numeric */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	err = getaddrinfo(*hostname, *port, &hints, &res);
	if (err) s = -2;
    }        


    if (!err) {
	err = 0; s = -1;
	connected = 0;
	for (r = res; r && !connected; r = r->ai_next) {
	    s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	    if (s < 0) 
		continue;
     
	    if (connect(s, r->ai_addr, r->ai_addrlen) < 0) {
		err = errno;
		close(s);
		s = -1;
		continue;
	    }
	    connected++;
	    break;
	}        
	if (!connected) s = -1;
    }
 
    if (res) 
	freeaddrinfo(res);

    return (s);
}
#endif /* USE_INET6 */

#ifdef USE_INET6
/* Determines hostname, address and port number used for the peer of socket */
int socket_remote_name(socket, name, ipname, port)
int socket;
char **name;
char **ipname;
char **port;
{
    struct sockaddr_storage server;
    int length=sizeof(server), retval, error;
    static char host[NI_MAXHOST],ip[NI_MAXHOST],portstr[NI_MAXSERV];

    error = getpeername(socket,(struct sockaddr*)&server,&length);
    if(!error) {
       	error = getnameinfo((struct sockaddr*)&server, length, host,
			     NI_MAXHOST, NULL, 0, 0);
	error = getnameinfo((struct sockaddr*)&server, length, ip,
			    NI_MAXHOST, NULL, NI_MAXSERV,
			    NI_NUMERICHOST);
	retval = error;

	error = getnameinfo((struct sockaddr*)&server, length, NULL, 0, 
			     portstr, NI_MAXSERV, 0);

	if (error) 
	    retval = getnameinfo((struct sockaddr*)&server, length, ip,
				 NI_MAXHOST, portstr, NI_MAXSERV,
				 NI_NUMERICSERV);
    }
    else 
	retval = error;

    if(name)
	*name=host;
    if(ipname)
	*ipname=ip;
    if(port)
	*port=portstr;

    return(retval);
}

/* Determines the hostname, address  and port number used for our socket */
int socket_local_name(socket, name, ipname, port)
int socket;
char **name;
char **ipname;
char **port;
{
    struct sockaddr_storage server;
    int length=sizeof(server), retval, error;
    static char host[NI_MAXHOST],ip[NI_MAXHOST],portstr[NI_MAXSERV];

    error = getsockname(socket,(struct sockaddr*)&server,&length);
    if(!error) {
       	error = getnameinfo((struct sockaddr*)&server, length, host,
			    NI_MAXHOST, NULL, 0, 0);
	error = getnameinfo((struct sockaddr*)&server, length, ip,
			    NI_MAXHOST, NULL, NI_MAXSERV,
			    NI_NUMERICHOST);
	retval = error;

	error = getnameinfo((struct sockaddr*)&server, length, NULL, 0, 
			     portstr, NI_MAXSERV, 0);

	if (error) 
	    retval = getnameinfo((struct sockaddr*)&server, length, ip,
				 NI_MAXHOST, portstr, NI_MAXSERV,
				 NI_NUMERICSERV);
    }
    else
	retval = error;

    if(name)
	*name=host;
    if(ipname)
	*ipname=ip;
    if(port)
	*port=portstr;

    return(retval);
}
#endif /* USE_INET6 */

#ifndef USE_INET6
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
#endif /* !USE_INET6 */

/*EOF*/
