/* telnet.c - telnet */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "local.h"
#include "argument.h"

#define debug 0

void	rcvurg(int);

#define	BUFSIZE		2048	/* read buffer size	*/
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)
struct termios	oldtty;

int	connect_TCP(const char *host, int port);
int	ttysetup(void);
	
char disten[]="display interface Ten-GigabitEthernet\r";
/*------------------------------------------------------------------------
 * telnet - do the TELNET protocol to the given host and port
 *------------------------------------------------------------------------
 */
extern
void * 
telnet(void *ar)
{
	struct argument *arg;
	int sb;
	const char *host;
	int port;
	arg=(struct argument *)ar;
	sb=arg->sb;
	host=arg->host;
	port=arg->port;
	int wfd=*(int *)arg->wfd;//
	int rfd=*(int *)arg->rfd;//
	#if debug
    printf("rfd=%d ,wfd=%d\n",rfd,wfd);
	#endif
	FILE *sfp,*outfp;
	outfp=fopen("tmp.txt","w+");
	unsigned char	buf[BUFSIZE];
	int	s, nfds;	/* socket and # file descriptors */
	int	cc;
	int	on = 1;
	fd_set	arfds, awfds, rfds, wfds;	
   // printf("%s %d\n",host,port);
	s = connect_TCP(host, port);
	ttysetup();

	fsmbuild();	/* set up FSM's */

	(void) signal(SIGURG, rcvurg);
	(void) setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (char *)&on,sizeof(on));

	nfds = getdtablesize();
	FD_ZERO(&arfds);
	FD_ZERO(&awfds);
	FD_SET(s, &arfds);	/* the socket			*/
	FD_SET(rfd, &arfds);	/* standard input		*/
	//FD_SET(wfd, &arfds);	/* standard output
	
	sfp = fdopen(s, "w");	/* to get buffered output	*/

    int count=0;int i;
	int password=0;
	while (1) {
		#if debug
		count++;
		printf("telnet %d\n",count);
		#endif
		memcpy(&rfds, &arfds, sizeof(rfds));
		memcpy(&wfds, &awfds, sizeof(rfds));

		if (select(nfds, &rfds, &wfds, (fd_set *)0,
				(struct timeval *)0) < 0) {
			if (errno == EINTR)
				continue;	/// just a signal	/
			cerrexit("select: %s\n", strerror(errno));
		}
		if (FD_ISSET(s, &rfds)) {
			cc = read(s, (char *)buf, sizeof(buf));
			if (cc < 0)
				cerrexit("socket read: %s\n",
						strerror(errno));
			else if (cc == 0) {
				if(debug)printf("\nswitch connection closed.\n");
				if (tcsetattr(0, TCSADRAIN, &oldtty) < 0)
					errexit("tcsetattr: %s\n",
						strerror(errno));
				break;
				//return (void *)0; //exit(0);
			} else
				{	ttwrite(sfp, outfp, buf, cc);
					write(wfd,buf,cc);
				}
		}
		if (FD_ISSET(rfd, &rfds)) {//0 means standard input
		   if(debug) printf("stdin active\n");
			cc = read(rfd, (char *)buf, sizeof(buf));
			if (cc < 0)
				cerrexit("tty read: %s\n",
						strerror(errno));
			else if (cc == 0) {
				FD_CLR(0, &arfds);
				(void) shutdown(s, 1);
			} else
				{
					sowrite(sfp, outfp, buf, cc);
				}
		}
		(void) fflush(sfp);
		(void) fflush(outfp);
	}
	pthread_exit(NULL);
	return (void *) 0;
}
