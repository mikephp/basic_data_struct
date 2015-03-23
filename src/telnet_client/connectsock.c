/*	connect_sock.c	- connectsock	*/

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef INADDR_NONE
#define INADDR_NONE		0xffffffff
#endif					/*	INADDR_NONE	,no inaddr	*/


int 	errexit(const char *format,...);

/*--------------------------------------------------------------
  *	connectsock	-	allocate & connect a socket using TCP or UDP
  *--------------------------------------------------------------
*/
int
connect_sock(const char *host, int port, const char *transport)
{
/*
*arguments:
*			host:		name of the host(name or ip) you want to connect;
*			port: 		the port that the connection use;
*			transport:	transport protocol("tcp" or "udp");
*return:
*			s:			a new descripter used to read/write;
*/
	struct hostent 		*phe;		//pointer to host info entry;
	struct protoent 	*ppe;		//pointer to protocol info entry;
	struct sockaddr_in	sin;		//an internet endpoint address
	int		s,type;

	memset(&sin,0,sizeof(sin));		//init 0

	sin.sin_family	=	AF_INET;
	sin.sin_port 	=	htons(port);

	/*	Map host name to IP address,allowing for dotted decimal	*/
	if ( (phe = gethostbyname(host)) )
		memcpy( &sin.sin_addr, phe->h_addr_list[0],phe->h_length);
	else
//		if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		errexit("can't get \"%s\" host entry \n",host);

	/*	Map transport protocol name to protocal number	*/
	if ( (ppe = getprotobyname(transport)) == 0)
		errexit("can't get \"%s\" protocol entry\n",transport);

	/*	Use protocol to choose a socket type	*/
	if ( strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/*	allocate a socket	*/
	s = socket(PF_INET, type, ppe->p_proto);
	if ( s<0 )
		errexit("can't create socket: %s\n",strerror(errno));
	
	/*	Connect the socket	*/
	if (connect(s, (struct sockaddr *)&sin,sizeof(sin)) <0 )
		errexit("can't connect to %s.%d:%s\n",host,port,strerror(errno));

	return s;

}
