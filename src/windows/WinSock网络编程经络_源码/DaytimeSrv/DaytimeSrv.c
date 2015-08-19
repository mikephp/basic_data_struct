/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  DaytimeSrv.c
 *
 * PURPOSE :  Daytime ���� UDP ����������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

#define DAYTIME_DEF_PORT    13  /* Daytime ȱʡ�˿� */
#define DAYTIME_BUF_SIZE    64  /* �������Ĵ�С */

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET	srv_sock = 0; /* ��������socket��� */
    struct sockaddr_in srv_addr,  /* Daytime ��������ַ */
                       clnt_addr; /* �ͻ��˵�ַ */
    unsigned short port = DAYTIME_DEF_PORT;
    int	result, addr_len = sizeof(srv_addr);
    char *time_str, recv_buf[DAYTIME_BUF_SIZE];
    time_t now_time;

    WSAStartup(WINSOCK_VERSION, &wsaData);/* ��ʼ�� WinSock ��Դ */
    
    if (argc == 2) /* �˿� */
        port = atoi(argv[1]);

    /* ���� Daytime ʹ�õ�socket */
    srv_sock = socket(AF_INET, SOCK_DGRAM, 0); 

    /* Daytime ��������ַ */
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

            now_time = time(0); /* �õ���ǰʱ�� */
            time_str = ctime(&now_time);

            /* ��ͻ��˷��͵�ǰ�����ں�ʱ���ַ��� */
            sendto(srv_sock, time_str, strlen(time_str), 0, 
                   (struct sockaddr *)&clnt_addr, addr_len);
        }
    }

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}
