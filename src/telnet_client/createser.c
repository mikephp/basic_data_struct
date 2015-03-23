/*************************************************************************
    > File Name: echoser.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Fri 01 Mar 2013 06:15:27 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


int createser(const char *host,int port )
{
    int listenfd; //被动套接字(文件描述符），即只可以accept
	//printf("usage:%s ip port\n",argv[0]);
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        //  listenfd = socket(AF_INET, SOCK_STREAM, 0)
        ERR_EXIT("socket error");
	printf("socket ok!\n");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port));
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr(host); 
    /* inet_aton("127.0.0.1", &servaddr.sin_addr); */

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");
	else printf("setsockopt ok!\n");
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");
	else printf("bind ok!\n");
		printf("somaxconn=%d\n",SOMAXCONN);
    if (listen(listenfd, SOMAXCONN) < 0) //listen应在socket和bind之后，而在accept之前
        ERR_EXIT("listen error");
	else printf("listen ok!\n");
    	struct sockaddr_in peeraddr; //传出参数
    	socklen_t peerlen = sizeof(peeraddr); //传入传出参数，必须有初始值
    	int conn; // 已连接套接字(变为主动套接字，即可以主动connect)
    if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)
        ERR_EXIT("accept error\n");
    printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
           ntohs(peeraddr.sin_port));

    char recvbuf[1024];
    while (1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = read(conn, recvbuf, sizeof(recvbuf));
		printf("cli>ret=%d\n",ret);
		printf("cli(%s:%d)>",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
        fputs(recvbuf, stdout);
        write(conn, recvbuf, ret);
    }

    close(conn);
    close(listenfd);

    return 0;
}
