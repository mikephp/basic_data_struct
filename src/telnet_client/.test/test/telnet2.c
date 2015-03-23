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

/*------------------------------------------------------------------------
 * telnet - do the TELNET protocol to the given host and port
 *------------------------------------------------------------------------
 */
int
telnet(const char *host, int port)
{
	unsigned char	buf[BUFSIZE];
	int	s, nfds;	/* socket and # file descriptors */
	int	cc;
	int	on = 1;
	fd_set	arfds, awfds, rfds, wfds;
	FILE	*sfp;
	
/*	
	int listenfd; //被动套接字(文件描述符），即只可以accept
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket error");
	//printf("socket ok!\n");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(10000);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");
	//else printf("setsockopt ok!\n");
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");
	//else printf("bind ok!\n");
	//printf("somaxconn=%d\n",SOMAXCONN);
    if (listen(listenfd, SOMAXCONN) < 0) //listen应在socket和bind之后，而在accept之前
        ERR_EXIT("listen error");
	//else printf("listen ok!\n");
    struct sockaddr_in peeraddr; //传出参数
    socklen_t peerlen = sizeof(peeraddr); //传入传出参数，必须有初始值
    //system();
	int conn; // 已连接套接字(变为主动套接字，即可以主动connect)
    if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)
        ERR_EXIT("accept error\n");
   // printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
    //       ntohs(peeraddr.sin_port));

    char recvbuf[1024];
*/	
	

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
				{ttwrite(sfp, stdout, buf, cc);
				//printf("write\n",cc);
				if(/*':'==buf[cc-1]*/0==memcmp("\r\nPassword:",buf,11)){
				 password=1;
				 //	count++;
				//printf("get pass\r\n");
					}
				}
				/*count++;
				printf("count=%d\n",count);
				for(i=0;i<cc;i++){
					printf("%x ",buf[i]);
				}
				printf("count_end\n");*/

			//	send(conn,buf,cc,0);
		}
		if (FD_ISSET(0, &rfds)) {//0 means standard input
		    printf("stdin active\n");
			cc = read(0, (char *)buf, sizeof(buf));
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
		}printf("count=%d\n",count);
		(void) fflush(sfp);
		(void) fflush(stdout);
	}

//	close(listenfd);
//	close(conn);
}
