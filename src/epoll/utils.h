#ifndef _UTILS_H
#define _UTILS_H
#include  <stdio.h> 
#include  <stdlib.h> 
#include  <iostream> 
#include  <unistd.h> 
#include  <fcntl.h> 
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/socket.h> 
#include  <sys/epoll.h> 
#include  <netinet/in.h> 
#include  <arpa/inet.h> 
#include <list> 
#include <numeric> 
#include <algorithm> 
#include <string.h> 

int setnonblocking(int sockfd)
{
    CHK(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK));
    return 0;
}

#endif