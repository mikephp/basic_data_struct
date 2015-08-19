/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  EchoSrv.c
 *
 * PURPOSE :  ���� UDP Э��� ECHO �ͻ��˳���.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32") /* WinSockʹ�õĿ⺯��   */


#define ECHO_DEF_PORT     7 /* ECHO ȱʡ�˿� */
#define ECHO_BUF_SIZE   256 /* �������Ĵ�С */

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  echo_sock = 0; /* ECHO �� socket ��� */
    
    struct sockaddr_in loc_addr,  /* socket�ı��ص�ַ */
                       t_addr; /* socket��Զ�˵�ַ */
    unsigned short port = ECHO_DEF_PORT;
    int i = 0, result = 0;
    int addr_len = sizeof(struct sockaddr_in);
    char recv_buf[ECHO_BUF_SIZE];
    
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ��WinSock ��Դ */
        
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    
    echo_sock = socket(AF_INET, SOCK_DGRAM, 0); /* ���� ECHO ʹ�õ�socket */
    
    /* socket�ı��ص�ַ */
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
