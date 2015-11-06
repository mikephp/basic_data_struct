#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

int printc(char* ch,int rn, void *buf);

int main(int argc,char *argv[])
{
	int port ;
	printf("argc = %d\n",argc);
	printf("usage:%s port\n",basename(argv[0]));
	if(argc<2){
		port = 514;
	}else{
		port = atoi(argv[1]);
	}
	struct sockaddr_in addr;
	struct in_addr in_addr_1;
	int addr_len = sizeof(struct sockaddr_in); 
	char buf[1500];
	int len,addrlen,ret;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(fd<0)
		printf("create socket failed\n");
	int opt=1;
	int opt_len = sizeof(opt);
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,opt_len);
	//setsockopt(fd,SOL_SOCKET,SO_REUSEPORT,(char*)&opt,opt_len);
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(port);
	addr.sin_addr.s_addr= htonl(INADDR_ANY);//inet_addr("127.0.0.1");
	if((ret=bind(fd,(struct sockaddr *)&addr,sizeof(addr))) < 0){
		printf("bind failed.fd=%d,ret=%d,error=%d,%s\n",fd,ret,errno,strerror(errno));
		return -1;
	}else{
		printf("bind success\n");
	}
	int cnt = 0;
	while(1){
		cnt++;
		bzero(buf,sizeof(buf));
		len = recvfrom(fd,buf,sizeof(buf)-1,0,(struct sockaddr *)&addr,&addrlen);
		printf("[%s:%5d] len %4d,cnt = %d\n",inet_ntoa(addr.sin_addr),addr.sin_port,len,cnt);
		printc("data",len,buf);
	}


	return 0;
}

int printc(char* ch,int rn, void *buf)
{
	int i;
	unsigned char *pbuf;
	pbuf = (unsigned char*)buf;
	//printf("%s length %d\n",ch, rn);
	printf("\n");
	for (i = 0; i < rn; i++) {
		if ((i % 8) == 0 && i != 0)
			printf( "  #%d \n",i);
		printf( "%02X ", (*pbuf&0xff));
		pbuf++;
	}
	printf( "\n\n");
	return 0;
}
