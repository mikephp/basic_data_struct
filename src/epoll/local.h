#ifndef _LOCAL_H
#define _LOCAL_H

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

#define BUF_SIZE 1024                 //Ĭ�ϻ�����
#define SERVER_PORT 44444             //�����˿�
#define SERVER_HOST "127.0.0.1"   //������IP��ַ
#define EPOLL_RUN_TIMEOUT -1          //epoll�ĳ�ʱʱ��
#define EPOLL_SIZE 10000              //epoll�����Ŀͻ��˵������Ŀ

#define STR_WELCOME "Welcome to seChat! You ID is: Client #%d"
#define STR_MESSAGE "Client #%d>> %s"
#define STR_NOONE_CONNECTED "Noone connected to server except you!"
#define CMD_EXIT "EXIT"

//�������õĺ궨�壺���͸�ֵ���Ҽ��
#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//================================================================================================
//��������                  setnonblocking
//����������                ����socketΪ������
//���룺                    [in] sockfd socket��ʾ��
//�����                    ��
//���أ�                    0
//================================================================================================
int setnonblocking(int sockfd);

//================================================================================================
//��������                  handle_message
//����������                ����ÿ���ͻ���socket
//���룺                    [in] new_fd socket��ʾ��
//�����                    ��
//���أ�                    ���شӿͻ��˽��ܵ����ݵĳ���
//================================================================================================
int handle_message(int new_fd);
#endif