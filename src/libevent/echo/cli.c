#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#define cerror(str) do{perror(str); exit(EXIT_FAILURE); }while(0)

static int port; 
static char addr[256];
static int nlen;
static char msg[]="hello\n";
int main(int argc, char*argv[])
{
    
    int fd;
    struct sockaddr_in sin;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
  
    if(argc<3)
    {
        port = 8888;
    }else
    {
        port = atoi(argv[2]);
    }
    
    sin.sin_port = htons(port);
    
    if(argc<2)
    {
        strcpy(addr, argv[1]);
        if (inet_pton(AF_INET, addr, &(sin.sin_addr) )<0)
        {
             struct hostent *psh;  
             psh=gethostbyname(addr);
             if(psh!=NULL)
                inet_pton(AF_INET, psh->h_addr, &(sin.sin_addr) );
             else
                cerror("inet_pton");
        }
    }
    
    if((fd=socket(AF_INET, SOCK_STREAM, 0))<0)
        cerror("socket");
    
    if(connect(fd, (struct sockaddr*)&sin, sizeof(sin))<0 )
        cerror("sonnect");
    
    if( (nlen = write(fd,msg, strlen(msg)))<0 )
        cerror("write");
        
    if( (nlen = read(fd,msg, strlen(msg)))<0 )
        cerror("read");
    msg[nlen]='\0';
    printf("msg: %s\n", msg);
    return 0;
}
