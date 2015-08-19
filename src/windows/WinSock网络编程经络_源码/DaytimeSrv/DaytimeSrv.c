/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  DaytimeSrv.c
 *
 * PURPOSE :  Daytime 基于 UDP 服务器程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

#define DAYTIME_DEF_PORT    13  /* Daytime 缺省端口 */
#define DAYTIME_BUF_SIZE    64  /* 缓冲区的大小 */

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET	srv_sock = 0; /* 服务器的socket句柄 */
    struct sockaddr_in srv_addr,  /* Daytime 服务器地址 */
                       clnt_addr; /* 客户端地址 */
    unsigned short port = DAYTIME_DEF_PORT;
    int	result, addr_len = sizeof(srv_addr);
    char *time_str, recv_buf[DAYTIME_BUF_SIZE];
    time_t now_time;

    WSAStartup(WINSOCK_VERSION, &wsaData);/* 初始化 WinSock 资源 */
    
    if (argc == 2) /* 端口 */
        port = atoi(argv[1]);

    /* 创建 Daytime 使用的socket */
    srv_sock = socket(AF_INET, SOCK_DGRAM, 0); 

    /* Daytime 服务器地址 */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(srv_sock, (struct sockaddr *)&srv_addr, addr_len);
    if (result == SOCKET_ERROR)
    {
        printf("[Daytime] bind error : %d", WSAGetLastError());
        closesocket(srv_sock);
        return -1;
    }

    printf("[Daytime] The server is running ... ...\r\n"); 
    while (1)
    {
        result = recvfrom(srv_sock, recv_buf, DAYTIME_BUF_SIZE, 0, 
                          (struct sockaddr *)&clnt_addr, &addr_len);

        if (result >= 0)
        {
            recv_buf[result] = 0;
            printf("[Daytime] recv: \"%s\", from %s\r\n", 
                    recv_buf, inet_ntoa(clnt_addr.sin_addr));

            now_time = time(0); /* 得到当前时间 */
            time_str = ctime(&now_time);

            /* 向客户端发送当前的日期和时间字符串 */
            sendto(srv_sock, time_str, strlen(time_str), 0, 
                   (struct sockaddr *)&clnt_addr, addr_len);
        }
    }

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}
