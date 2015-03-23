#include <pthread.h>
//#include <signal.h>
#include "port.h"
#include "argument.h"
#ifndef BUFSIZE
#define BUFSIZE 2048
#endif
#define mmss 70 //yan shi 10 hao miao,delay times microseconds
void *telnet(void *ar);
//void mmsleep(void *);

int rfd,wfd;
int f2s[2],s2f[2];
pthread_t tid;
struct argument arg;

int init_telnet(char *host,int port)
{
	char buf[BUFSIZE];
	int cc;
	strcpy(arg.host,host);
	arg.port=port;
	//father to son, son to father
	if(pipe(f2s)<0 || pipe(s2f)<0)printf("create pipe failed\n");
	arg.wfd=&s2f[1];
	arg.rfd=&f2s[0];
	rfd=s2f[0];
	wfd=f2s[1];
	if(pthread_create(&tid,NULL,(void *) telnet,(void *)&arg)){
   		printf("pthread_create failed\n");}
	//printf("thread id=%d\n",tid);
   while(1){
	   memset(buf,0,BUFSIZE);
	   cc=read(s2f[0],buf,BUFSIZE);
	   if(cc){
		   if(':'==buf[cc-1]){
			   write(wfd,"chanct-code\r",12);  
			   mmsleep(50);
			   read(rfd,buf,BUFSIZE);
			   break;
			}
	   }
	   cc=0;
   }	   
}
void **tstatus;
int quit(int q,int rfd,int wfd){
	int cc;
	cc=write(wfd,"quit\r",5);
	//kill switch thread
	//if(!pthread_kill(tid, SIGQUIT))	printf("pthread kill ok\n");
	if(0!=pthread_join(tid,tstatus))	printf("pthread %d exit error!\n",tid);
}
int quit_telnet(){
	quit(0,rfd,wfd);
}

int test(char *cmd,int fd){
	write(fd,cmd,strlen(cmd));
}
int _get_port_stat(int sb,char *getport,PT *result,int rfd,int wfd){
	//display interface Ten-GigabitEthernet 1/0/29
	char buf[BUFSIZE];
	char pdata[BUFSIZE];
	int cc=0;
	memset(buf,0,BUFSIZE);
	sprintf(buf,"%s\r",getport);
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);	

	memset(pdata,0,BUFSIZE);
	cc=read(rfd,pdata,sizeof(pdata));
	
	write(wfd,"\x20\r",2);
	mmsleep(mmss);
	cc=cc+read(rfd,pdata+cc,sizeof(pdata));
	
	//printf("%s\ncc=%d",pdata,cc);
	port_data(pdata,cc,result);	
	return 0;
}

int _add_port_vlan(int sb,char *setport,int vlanid,int rfd,int wfd)
{
	switch(sb){
		case 0:
			add_port_vlan_h3c(setport,vlanid,rfd,wfd);
			break;
		default:
			return -1;
	}
	return 0;
}
int _del_port_vlan(int sb,char *setport,int vlanid,int rfd,int wfd)
{
	switch(sb){
		case 0:
			del_port_vlan_h3c(setport,vlanid,rfd,wfd);
			break;
		default:
			return -1;
	}
	return 0;
}


int test_get_port_stat(PT*result){
	printf("port_name:	%s\n",result->port_name);
	printf("port_band:	%s\n",result->port_band);
	printf("port_type:	%s\n",result->port_type);
	printf("port_desc:	%s\n",result->port_desc);
	printf("port_stat:	%s\n",result->port_stat);
	
	printf("port_in:	%ld\n",result->port_in);
	printf("port_in_mul:	%ld\n",result->port_in_mul);
	printf("port_in_sin:	%ld\n",result->port_in_sin);

	printf("port_out:	%ld\n",result->port_out);
	printf("port_out_mul:	%ld\n",result->port_out_mul);
	printf("port_out_sin:	%ld\n",result->port_out_sin);
	return 0;
}

int add_port_vlan_h3c(char *setport,int vlanid,int rfd,int wfd)
{//interface gigabitethernet 1/0/3
	char buf[BUFSIZE];
	write(wfd,"system-view\r",12);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	memset(buf,0,sizeof(buf));
	sprintf(buf,"interface %s\r",setport);
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	//port link-type trunk
	memset(buf,0,sizeof(buf));
	sprintf(buf,"port link-type trunk\r");
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	//port trunk permit vlan 100 200
	memset(buf,0,sizeof(buf));
	sprintf(buf,"port trunk permit vlan %d\r",vlanid);
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	write(wfd,"quit\r",5);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));

	write(wfd,"quit\r",5);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
}

int del_port_vlan_h3c(char *setport,int vlanid,int rfd,int wfd)
{//interface gigabitethernet 1/0/3
	char buf[BUFSIZE];
	write(wfd,"system-view\r",12);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	memset(buf,0,sizeof(buf));
	sprintf(buf,"interface %s\r",setport);
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
	
	//port link-type trunk
/*	memset(buf,0,sizeof(buf));
	sprintf(buf,"port link-type trunk\r");
	write(wfd,buf,strlen(buf));
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
*/
	//port trunk permit vlan 100 200
	memset(buf,0,sizeof(buf));
	sprintf(buf,"undo port trunk permit vlan %d\r",vlanid);
	write(wfd,buf,strlen(buf));
	mmsleep(250);
	read(rfd,buf,sizeof(buf));
	
	write(wfd,"quit\r",5);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));

	write(wfd,"quit\r",5);
	mmsleep(mmss);
	read(rfd,buf,sizeof(buf));
}

int add_port_vlan(char *setport,int vlanid){
	_add_port_vlan(0, setport, vlanid, rfd,wfd);
}

int get_port_stat(char *getport,PT * stat){
	_get_port_stat(0, getport, stat, rfd,wfd);
}

int del_port_vlan(char *setport,int vlanid){
	_del_port_vlan(0, setport, vlanid, rfd,wfd);
}
