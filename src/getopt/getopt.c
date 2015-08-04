#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int rv = 0, opt = 0;
    int srv_ip = 0, srv_port = 0, nparam = 0, loop_time = 0;
    char remote_host[128] = {0};
    time_t t1,t2;
	int serviceid = -1;
	while ((opt = getopt(argc, argv, "hn:l:v:s:p:")) > 0) {
		switch (opt) {
        case 'n':
            nparam = atoi(optarg);
			printf("nparam=%x\n",nparam);
            break;
        case 'l':
            loop_time = atoi(optarg);
			printf("loop_time=%x\n",loop_time);
            break;
        case 's':
            if (sprintf(remote_host,"%s", optarg)) {
                srv_ip = inet_network(remote_host);
				printf("srv_ip=%x\n",srv_ip);
            }
            break;
        case 'p':
            srv_port = atoi(optarg);
			printf("srv_port=%x\n",srv_port);
            break;
		case 'h':
		default:
			printf("Usage: [n <nparam>] [l <loop_time>] [s <remote_host>] [p <remote_port>]\n");
			exit(0);
			break;
		}
	}
}