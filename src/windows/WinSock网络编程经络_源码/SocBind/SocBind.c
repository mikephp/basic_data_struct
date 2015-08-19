/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  SocBind.c
 *
 * PURPOSE :  绑定不同的端口和地址测试.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

#define BIND_DEF_PORT   htons(6000) /* 缺省端口 */
#define BIND_DEF_ADDR   inet_addr("127.0.0.1")

struct port_addr_s 
{
    unsigned short port;
    unsigned long  addr;
};

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET  sock = 0; /* socket 句柄 */
    int result = 0, i, count, addr_len;
    struct sockaddr_in laddr, taddr;

    /* socket的本地地址 */
    struct port_addr_s port_addr[] = {
        {0,             INADDR_ANY},
        {BIND_DEF_PORT, INADDR_ANY},
        {0,             BIND_DEF_ADDR},
        {BIND_DEF_PORT, BIND_DEF_ADDR} 
    };

    WSAStartup(MAKEWORD(2,0), &wsaData); /* 初始化 */

    count = sizeof(port_addr) / sizeof(port_addr[0]);

    for (i = 0; i < count; i++)
    {
        memset(&laddr, 0, sizeof(laddr));
        laddr.sin_family = AF_INET;
        laddr.sin_port = port_addr[i].port;
        laddr.sin_addr.s_addr = port_addr[i].addr;
        addr_len = sizeof(struct sockaddr_in);
        
        sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
        result = bind(sock, (struct sockaddr *)&laddr, addr_len);

        if (result == SOCKET_ERROR)
        {            
            printf("[bind] error = %d\r\n", WSAGetLastError());
            continue;
        }

        result = getsockname(sock, (struct sockaddr *)&taddr, &addr_len);

        if (result == 0) /* 成功 */
            printf("[bind] address = %s : %d\r\n", 
                    inet_ntoa(taddr.sin_addr), ntohs(taddr.sin_port));
        else
            printf("[bind] error = %d\r\n", WSAGetLastError());

        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
