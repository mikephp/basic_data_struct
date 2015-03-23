/* tclient.c - main */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include "call_telnet_api.h"
char	*host = "localhost";	/* host to use if none supplied		*/

int	errexit(const char *format, ...);

/*------------------------------------------------------------------------
 * main - TCP client for TELNET service
 *------------------------------------------------------------------------
 */
//int sw_ifstat();
//int sw_ifstat(FILE *fp,PT *tmp);
//sb means she bei
//void *telnet(void *);
//int init_telnet(int sb,char *host,int port,int *rfd,int *wfd);
//int get_port_stat(int sb,char *getport,PT *result,int rfd,int wfd);
//int set_port_vlan(int sb,char *setport,int vlanid,int rfd,int wfd);
//int quit(int sb,int rfd,int wfd);
//int test(char * cmd,int rfd,int wfd);
//void mmsleep(int ms);

int
main(int argc, char *argv[])
{
//	char	*service = "telnet";	/* default service name		*/
	char	buf[1024];
	int 	cc;
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
	
	PT* 	result;PT* re2;	
	result = (PT*)malloc(sizeof(PT));
	re2 = (PT*)malloc(sizeof(PT));
//Ten-GigabitEthernet 1/1/1 1/1/2 1/2/1 1/2/2 1/0/29 1/0/30	
	init_telnet(host,port);
	//set_port_vlan("Gigabitethernet 1/0/12",31);
	get_port_stat("display interface Ten-GigabitEthernet 1/1/1",result);
	test_get_port_stat(result);
	printf("####################################################\n");
	get_port_stat("display interface Ten-GigabitEthernet 1/0/29",re2);
	test_get_port_stat(re2);
	
	add_port_vlan("GigabitEthernet 1/0/12",124);
	
	quit_telnet();		

	//test if results are right

	
	exit(0);
}
