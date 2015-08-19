/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  EchoSrv.c
 *
 * PURPOSE :  基于 UDP 协议的 ECHO 客户端程序.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32") /* WinSock使用的库函数   */


#define ECHO_DEF_PORT     7 /* ECHO 缺省端口 */
#define ECHO_BUF_SIZE   256 /* 缓冲区的大小 */

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  echo_sock = 0; /* ECHO 的 socket 句柄 */
    
    struct sockaddr_in loc_addr,  /* socket的本地地址 */
                       t_addr; /* socket的远端地址 */
    unsigned short port = ECHO_DEF_PORT;
    int i = 0, result = 0;
    int addr_len = sizeof(struct sockaddr_in);
    char recv_buf[ECHO_BUF_SIZE];
    
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化WinSock 资源 */
        
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    
    echo_sock = socket(AF_INET, SOCK_DGRAM, 0); /* 创建 ECHO 使用的socket */
    
    /* socket的本地地址 */
    loc_addr.sin_family = AF_INET;
    loc_addr.sin_port = htons(port);
    loc_addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(echo_sock, (struct sockaddr *)&loc_addr, sizeof(struct sockaddr));
        
    while (1)
    {   
        result = recvfrom(echo_sock, recv_buf, ECHO_BUF_SIZE, 0, 
                              (struct sockaddr *)&t_addr, &addr_len);
        if (result == SOCKET_ERROR)
        {
            printf("[Echo Server] Recv error %d\r\n", WSAGetLastError());
            break;
        }
        
        recv_buf[result] = 0;
        printf("[Echo Server] Receive data: \"%s\", from %s\r\n", 
            recv_buf, inet_ntoa(t_addr.sin_addr));
        sendto(echo_sock, recv_buf, result, 0, 
               (struct sockaddr *)&t_addr, addr_len);
    }
    
    closesocket(echo_sock);
    WSACleanup();
    
    return 0;
}
