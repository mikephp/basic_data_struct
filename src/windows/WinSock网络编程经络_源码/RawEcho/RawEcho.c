/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  RawEcho.c
 *
 * PURPOSE :  �� Raw socket ʵ�� ECHO �ͻ���.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h> /* for IP_HDRINCL */
#include <stdio.h>
#include <time.h>
#include "RawEcho.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

unsigned short ip_checksum(unsigned short *buf, int buf_len);

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET raw_soc = 0;        /* socket ��� */
    struct sockaddr_in remote; /* socket��Զ�˵�ַ */

    int result = 0;
    unsigned short dat_len = 0, port = ECHO_DEF_PORT;
    BOOL opt_val = TRUE;
    unsigned long local_addr, dest_addr;
    char data[ECHO_BUF_SIZE], host[256];
    struct hostent *host_ent;
    struct ip_hdr *ip_hdr = (struct ip_hdr *)data;
    struct udp_hdr *udp_hdr = (struct udp_hdr *)(ip_hdr + 1);

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ�� WinSock ��Դ */

    switch (argc)
    {
    case 3:
        port = atoi(argv[2]);
    case 2:
        dest_addr = inet_addr(argv[1]);
        break;
    default:
        return -1;
    } 

    /* �õ������� IP ��ַ */
    if (gethostname(host, sizeof(host)) == SOCKET_ERROR) 
    {
        printf("[ECHO] gethostname() error: %d\r\n", WSAGetLastError());
        return -1;
    }

    host_ent = gethostbyname(host);
    if (host_ent == 0) 
    {
        printf("[ECHO] gethostbyname() error: %d\r\n", WSAGetLastError());
        return -1 ;
    }

    memcpy(&local_addr, host_ent->h_addr, sizeof(struct in_addr));

    /* ���� raw socket */
    raw_soc = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); 
    result = setsockopt(raw_soc, IPPROTO_IP, IP_HDRINCL, 
                        (char *)&opt_val, sizeof(opt_val)); 

    /* ���һЩ���� */
    memset(data, 0, ECHO_BUF_SIZE);
    dat_len = strlen(ECHO_DEF_DATA);
    memcpy(udp_hdr + 1, ECHO_DEF_DATA, dat_len);
    dat_len = sizeof(struct udp_hdr) + dat_len;

    /* ���� IP �ײ� */
    ip_hdr->vers_len = (IP_VERSION << 4) | (IP_HDR_LEN >> 2);
    ip_hdr->tos = 0;
    ip_hdr->total_len= htons((u_short)(IP_HDR_LEN + dat_len));
    ip_hdr->id = (unsigned short)time(NULL); /* ������ֵ */
    ip_hdr->frag = 0;
    ip_hdr->ttl = 0;
    ip_hdr->proto = IPPROTO_UDP;
    ip_hdr->sour = local_addr;
    ip_hdr->dest = dest_addr;

    /* ������Ϊ UDP ����, ���� UDP У��� */
    ip_hdr->checksum = htons(dat_len);

    /* ���� UDP �ײ� */
    udp_hdr->sport = htons(ECHO_SRC_PORT);
    udp_hdr->dport = htons(port);
    udp_hdr->length= htons(dat_len);
    udp_hdr->checksum = ip_checksum((unsigned short *)
             (data + UDP_PSEUDO_OFFSET), UDP_PSEUDO_LEN + dat_len);

    /* ������ UDP У���, ��IP�ײ�ֵ����� */
    ip_hdr->ttl = IP_DEFAULT_TTL;
    ip_hdr->checksum = 0;

    /* ��������ַ */
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = dest_addr;

    dat_len += sizeof(*ip_hdr);
    result = sendto(raw_soc, data, dat_len, 0, 
                    (struct sockaddr *)&remote, sizeof(remote));

    if (result > 0)
        printf("[Echo] Send data: \"%s\"\r\n", (char *)(udp_hdr + 1));
    else
        printf("[Echo] Send error : %d\r\n", WSAGetLastError());

    closesocket(raw_soc);
    WSACleanup();

    return 0;
}

/**************************************************************************
 *
 * ��������: ����У���.
 *
 * ����˵��: [IN] buf, ���ݻ�����;
 *           [IN] buf_len, buf �ĳ���.
 *
 * �� �� ֵ: У���.
 *
 **************************************************************************/
unsigned short ip_checksum(unsigned short *buf, int buf_len)
{
    unsigned long checksum = 0;
    
    while (buf_len > 1)
    {
        checksum += *buf++;
        buf_len -= sizeof(unsigned short);
    }
    
    if (buf_len)
    {
        checksum += *(unsigned char *)buf;
    }
    
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    
    return (unsigned short)(~checksum);
}