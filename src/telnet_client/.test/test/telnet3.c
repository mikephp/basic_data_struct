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

#include "local.h"

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
int vlan_set();
int vlan_set()
{
}
	
char disten[]="display interface Ten-GigabitEthernet\r";
/*------------------------------------------------------------------------
 * telnet - do the TELNET protocol to the given host and port
 *------------------------------------------------------------------------
 */
int 
telnet(const char *host, int port,int task,FILE * outfp /*,char **result*/)
{
	unsigned char	buf[BUFSIZE];
	int	s, nfds;	/* socket and # file descriptors */
	int	cc;
	int	on = 1;
	fd_set	arfds, awfds, rfds, wfds;
	FILE	*sfp;	

	s = connect_TCP(host, port);
	ttysetup();

	fsmbuild();	/* set up FSM's */

	(void) signal(SIGURG, rcvurg);
	(void) setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (char *)&on,
			sizeof(on));
	
	nfds = getdtablesize();
	FD_ZERO(&arfds);
	FD_ZERO(&awfds);
	FD_SET(s, &arfds);	/* the socket			*/
	FD_SET(0, &arfds);	/* standard input		*/

	sfp = fdopen(s, "w");	/* to get buffered output	*/

     int count=0;int i;
	int password=0;
	while (1) {count++;
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
				printf("\nconnection closed.\n");
				if (tcsetattr(0, TCSADRAIN, &oldtty) < 0)
					errexit("tcsetattr: %s\n",
						strerror(errno));
				exit(0);
			} else
				{	ttwrite(sfp, outfp, buf, cc);
				if(/*':'==buf[cc-1]*/0==memcmp("\r\nPassword:",buf,11)){
				 	password=1;}
				}
		}
		/*if (FD_ISSET(s0, &rfds)) {//0 means standard input
		    printf("stdin active\n");
			cc = read(s0, (char *)buf, sizeof(buf));
			if (cc < 0)
				cerrexit("tty read: %s\n",
						strerror(errno));
			else if (cc == 0) {
				FD_CLR(0, &arfds);
				(void) shutdown(s, 1);
			} else
				{
				if(1==password){
					memcpy(buf,"chanct-code\r",12);
					cc=12;
				//	printf("pa\n");
				}
				password=0;
				
				sowrite(sfp, stdout, buf, cc);
					for(i=0;i<cc;i++){
					//	printf("%x ",buf[i]);
					}
				//printf("end_buf_print\n");
				//printf("stdin cc=%d\r\n",cc);
				}
		}//printf("count=%d\n",count);
		*/
				if(1==password){
					memcpy(buf,"chanct-code\r",12);
					cc=12;
				
					password=0;
					sowrite(sfp, outfp, buf, cc);

				    if(0==task){
					cc=strlen(disten)+1;
					memcpy(buf,disten,cc);
					sowrite(sfp,outfp,buf,cc);
				    for(i=0;i<10;i++){ 
						cc=2;
				 		memcpy(buf,"\x20\r",cc);
						sowrite(sfp,outfp,buf,cc);
                    }

				    }
					
					if(1==task){
						
						
					}

					cc=5;
					memcpy(buf,"quit\r",cc);
					sowrite(sfp,outfp,buf,cc);
				}
		
		(void) fflush(sfp);
		(void) fflush(outfp);
	}	

}
