/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  BootpSrv.c
 *
 * PURPOSE :  Bootstrap ����������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "Bootp.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */


/**************************************************************************
 *
 * ��������: ����ͻ�����������.
 *
 * ����˵��: [IN/OUT] bootp_msg, ���յ������ݻ�����;
 *           [IN] length, bootp_msg �ĳ���.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
static int bootp_handle_request(struct bootstrap_s *bootp_msg, int length)
{
    static unsigned char count = 0; 
    char sname[256];    
    struct hostent *host_ent;
    struct in_addr taddr;

    /* ��鳤�� */
    if (length != sizeof(struct bootstrap_s) ||
        bootp_msg->op != BOOTREQUEST)
        return -1;

    /* ���Ӳ����Ϣ */
    if (bootp_msg->htype != BOOTP_HRD_TYPE || 
        bootp_msg->hlen != BOOTP_HRD_LEN)
        return -1;

    /* �õ��������� */
    if (gethostname(sname, sizeof(sname)) == SOCKET_ERROR) 
    {
        printf("[BOOTP] gethostname() error: %d\r\n", WSAGetLastError());
        return -1;
    }

    /* �õ����� IP ��ַ */
    host_ent = gethostbyname(sname);
    if (host_ent == 0) 
    {
        printf("[BOOTP] gethostbyname() error: %d\r\n", WSAGetLastError());
        return -1 ;
    }

    if (bootp_msg->sname[0] != 0 && strcmp(bootp_msg->sname, sname))
        return -1;

    /* server address */
    memcpy(&bootp_msg->siaddr, host_ent->h_addr, host_ent->h_length);
    strcpy(bootp_msg->sname, sname);
    strcpy(bootp_msg->file, BOOTP_FILE_NAME);

    if (bootp_msg->ciaddr == 0) /* ÿ�η���һ����ͬ�ĵ�ַ */
    {        
        taddr.s_addr = inet_addr(BOOTP_YOUR_ADDR);
        count = (count + 1) % 240;
        taddr.S_un.S_un_b.s_b4 += count;

        bootp_msg->yiaddr = taddr.s_addr;
    }

    bootp_msg->op = BOOTREPLY;

    return 0;
}

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET	srv_sock = 0; /* ��������socket��� */
    struct sockaddr_in srv_addr, clnt_addr;
    unsigned short port = BOOTP_SRV_PORT;
    int	result, addr_len = sizeof(srv_addr);
    struct bootstrap_s bootp_msg;
    BOOL brdcast = TRUE;
   
    WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ�� WinSock ��Դ */
    srv_sock = socket(AF_INET, SOCK_DGRAM, 0);/* ���� Bootstrap ��socket */
    setsockopt(srv_sock, SOL_SOCKET, SO_BROADCAST,
               (char *)&brdcast, sizeof(brdcast));

    /* Bootstrap ��������ַ */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(srv_sock, (struct sockaddr *)&srv_addr, addr_len);
    if (result == SOCKET_ERROR)
    {
        closesocket(srv_sock);
        printf("[BOOTP] bind() error : %d\n", WSAGetLastError());
        return -1;
    }

    printf("[BOOTP] server is running ... ...\n");
    while (1)
    {
        result = recvfrom(srv_sock, (char *)&bootp_msg, sizeof(bootp_msg),
                          0, (struct sockaddr *)&clnt_addr, &addr_len);
        if (result == SOCKET_ERROR)
        {
            printf("[BOOTP] recvfrom() error : %d\n", WSAGetLastError());
            continue;
        }

        printf("[BOOTP] request from : %s\n", inet_ntoa(clnt_addr.sin_addr));
        result = bootp_handle_request(&bootp_msg, result);
        if (result >= 0)
        {
            if (clnt_addr.sin_addr.s_addr == 0)
                clnt_addr.sin_addr.s_addr = INADDR_BROADCAST;
            
            sendto(srv_sock, (char *)&bootp_msg, sizeof(bootp_msg), 0, 
                   (struct sockaddr *)&clnt_addr, addr_len);
        }
    }

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}
