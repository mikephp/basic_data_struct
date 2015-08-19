/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  SocBind.c
 *
 * PURPOSE :  �󶨲�ͬ�Ķ˿ں͵�ַ����.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

#define BIND_DEF_PORT   htons(6000) /* ȱʡ�˿� */
#define BIND_DEF_ADDR   inet_addr("127.0.0.1")

struct port_addr_s 
{
    unsigned short port;
    unsigned long  addr;
};

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET  sock = 0; /* socket ��� */
    int result = 0, i, count, addr_len;
    struct sockaddr_in laddr, taddr;

    /* socket�ı��ص�ַ */
    struct port_addr_s port_addr[] = {
        {0,             INADDR_ANY},
        {BIND_DEF_PORT, INADDR_ANY},
        {0,             BIND_DEF_ADDR},
        {BIND_DEF_PORT, BIND_DEF_ADDR} 
    };

    WSAStartup(MAKEWORD(2,0), &wsaData); /* ��ʼ�� */

    count = sizeof(port_addr) / sizeof(port_addr[0]);

    for (i = 0; i < count; i++)
    {
        memset(&laddr, 0, sizeof(laddr));
        laddr.sin_family = AF_INET;
        laddr.sin_port = port_addr[i].port;
        laddr.sin_addr.s_addr = port_addr[i].addr;
        addr_len = sizeof(struct sockaddr_in);
        
        sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
        result = bind(sock, (struct sockaddr *)&laddr, addr_len);

        if (result == SOCKET_ERROR)
        {            
            printf("[bind] error = %d\r\n", WSAGetLastError());
            continue;
        }

        result = getsockname(sock, (struct sockaddr *)&taddr, &addr_len);

        if (result == 0) /* �ɹ� */
            printf("[bind] address = %s : %d\r\n", 
                    inet_ntoa(taddr.sin_addr), ntohs(taddr.sin_port));
        else
            printf("[bind] error = %d\r\n", WSAGetLastError());

        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
