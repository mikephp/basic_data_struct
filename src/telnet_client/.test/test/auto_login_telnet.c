/* tclient.c - main */

#include <stdlib.h>

char	*host = "localhost";	/* host to use if none supplied		*/

int	errexit(const char *format, ...);
int	telnet(const char *host, int port);

/*------------------------------------------------------------------------
 * main - TCP client for TELNET service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
//	char	*service = "telnet";	/* default service name		*/
	int		port = 23;

	switch (argc) {
	case 1:	break;
	case 3:
		port = atoi(argv[2]);
		/* FALL THROUGH */
	case 2:
		host = argv[1];
		break;
	default:
		errexit("usage: telnet [host [port]]\n");
	}
	telnet(host, port);
	exit(0);
}

