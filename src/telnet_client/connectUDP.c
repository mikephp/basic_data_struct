/* connectUDP.c - connectUDP */

int	connect_sock(const char *host, int port,
		const char *transport);

/*------------------------------------------------------------------------
 * connectUDP - connect to a specified UDP service on a specified host
 *------------------------------------------------------------------------
 */
int
connectUDP(const char *host, int port)
/*
 * Arguments:
 *      host    - name of host to which connection is desired
 *      service - service associated with the desired port
 */
{
	return connect_sock(host, port, "udp");
}
