/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  EchoSrv.c
 *
 * PURPOSE :  ECHO Э�����������.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32") /* WinSock ʹ�õĿ⺯�� */

#define ECHO_DEF_PORT     7 /* ������ȱʡ�˿� */
#define ECHO_BUF_SIZE   256 /* �������Ĵ�С   */

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  echo_soc = 0,      /* ���� socket ���	*/
            acpt_soc = 0;

    struct sockaddr_in serv_addr,   /* socket�ı��ص�ַ */
                       clnt_addr;   /* socket��Զ�˵�ַ */
    unsigned short port = ECHO_DEF_PORT;
    int result = 0;
    int addr_len = sizeof(struct sockaddr_in);
    char recv_buf[ECHO_BUF_SIZE];

    if (argc == 2)
        port = atoi(argv[1]);

    WSAStartup(MAKEWORD(2,0), &wsa_data);/* ��ʼ�� WinSock��Դ */
    echo_soc = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */

    /* socket �ı��ص�ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(echo_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR)
    {
        printf("[Echo Server] bind error: %d\n", WSAGetLastError());
        closesocket(echo_soc);
        return -1;
    }
    
    listen(echo_soc, SOMAXCONN);

    printf("[Echo Server] is running ... ...\n");
    while (1)
    {
        acpt_soc = accept(echo_soc, (struct sockaddr *)&clnt_addr, &addr_len);
        if (acpt_soc == INVALID_SOCKET)
        {
            printf("[Echo Server] accept error: %d\n", WSAGetLastError());
            break;
        }

        result = recv(acpt_soc, recv_buf, ECHO_BUF_SIZE, 0);
        if (result > 0)
        {
            recv_buf[result] = 0;
            printf("[Echo Server] receives: \"%s\", from %s\r\n", 
                   recv_buf, inet_ntoa(clnt_addr.sin_addr));

            result = send(acpt_soc, recv_buf, result, 0);
        }

        closesocket(acpt_soc);
    }

    closesocket(echo_soc);
    WSACleanup();

    return 0;
}
