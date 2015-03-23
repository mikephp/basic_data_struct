/*	connect_tcp.c - connect_tcp	*/

int connect_sock(const char *host,int port,const char *transport);

/*	--------------------------------------------------------------------
	*connect_tcp -	connect a specified TCP service on a specified host;
	*-------------------------------------------------------------------
*/
int
connect_TCP(const char *host, int port)
/*
*	Argument:
*		host		- name of host which connection want;
*		port		- the port which connection use;
*	Return:
*		s			- a new descripter used to read/write;
*/
{
	return connect_sock(host,port,"tcp");
}
