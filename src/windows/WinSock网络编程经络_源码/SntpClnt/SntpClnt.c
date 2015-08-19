/*************************************************************************
 *  
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 * 
 * FILENAME:  SntpClnt.c
 *
 * PURPOSE :  ������ʱ��Э��ͻ��˳���.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h> /* struct ip_mreq */
#include <stdio.h>
#include "Sntp.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

/**************************************************************************
 *
 * ��������: ����ӷ��������յ�����Ϣ.
 *
 * ����˵��: [IN] sntp_msg, SNTP ��Ϣ;
 *           [IN] msg_len, ��Ϣ����������;
 *           [IN] dest_t,  ����ʱ��;
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
static int sntp_recv_handle(struct sntp_s *sntp_msg, int msg_len, 
                            time_fp *dest_t)
{
    double diff;
    unsigned char mode;
    time_fp t2_1, t3_4;
    time_fp offset_t, orig_t, recv_t, xmt_t;

    /* ��� SNTP ��Ϣ����Ч�� */
    if (msg_len < sizeof(*sntp_msg))
    {
        printf("[SNTP] packet is too small %d\n", msg_len);
        return -1;
    }

    if (MSG_VERSION(sntp_msg->li_vn_md) < SNTP_OLD_VERSION ||
        MSG_VERSION(sntp_msg->li_vn_md) > SNTP_VERSION) 
    {
        printf("[SNTP] version is wrong\n");
        return -1;
    }

    mode = MSG_MODE(sntp_msg->li_vn_md);
    if (mode != MODE_SERVER && mode != MODE_BROADCAST)
    {
        printf("[SNTP] mode is not matched %d\n", mode);
        return -1;
    }

    NTOHL_FP(&sntp_msg->orig, &orig_t);
    NTOHL_FP(&sntp_msg->recv, &recv_t);
    NTOHL_FP(&sntp_msg->xmt, &xmt_t);

    /* û�б����� Transmit Timestamp */
    if (orig_t.fp_ui == 0 && orig_t.fp_uf == 0)
    {
        offset_t = xmt_t;
        TS_SUB(&offset_t, dest_t);
    }
    else
    {
        /* ����ʱ��ƫ��: ((T2 - T1) + (T3 - T4)) / 2 */
        t3_4 = xmt_t;
        TS_SUB(&t3_4, dest_t); /* T3 - T4 */
        t2_1 = recv_t;
        TS_SUB(&t2_1, &orig_t);
        
        offset_t = t3_4;
        TS_RSHIFT(&offset_t);
        TS_RSHIFT(&t2_1);
        TS_ADD(&offset_t, &t2_1);
    }
    
    TS_FPTOD(&offset_t, diff);
    printf("[SNTP] offset with the server is : %d microsecond\n", 
           (int)(diff * 1000000));

    return 0;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET	clt_soc = 0;       /* �ͻ���socket��� */
    struct ip_mreq mcast_addr; /* �ಥ��ַ */
    struct sockaddr_in local, dest;
    unsigned short port = SNTP_DEF_PORT;
    int	result, addr_len = sizeof(local);
    struct sntp_s sntp_msg;
    time_fp now_fp;
    char *srv_addr = NULL;

    if (argc > 1)
        srv_addr = argv[1];

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ�� WinSock ��Դ */
    clt_soc = socket(AF_INET, SOCK_DGRAM, 0);/* ���� SNTP ��socket */

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;
    
    result = bind(clt_soc, (struct sockaddr *)&local, sizeof(local));
    if (result == SOCKET_ERROR)
    {
        printf("[SNTP] bind() error : %d\n", WSAGetLastError());
        closesocket(clt_soc);
        return -1;
    }

    /* �ಥ��ַ */
    mcast_addr.imr_multiaddr.s_addr = inet_addr(SNTP_MCAST_ADDR);
    mcast_addr.imr_interface.s_addr = INADDR_ANY;

    /* ����ಥ�� */
    result = setsockopt(clt_soc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                        (char *)&mcast_addr, sizeof(mcast_addr));
    if (result == SOCKET_ERROR)
    {
        printf("[SNTP] setsockopt(IP_ADD_MEMBERSHIP) error : %d\n", 
                WSAGetLastError());
        closesocket(clt_soc);
        return -1;
    }

    memset(&sntp_msg, 0, sizeof(sntp_msg));
    sntp_get_sys_time(&sntp_msg.xmt);
    HTONL_FP(&sntp_msg.xmt, &sntp_msg.xmt);
    sntp_msg.li_vn_md = MSG_LI_VN_MODE(LEAP_NOT_SYNC, 
                            SNTP_VERSION, MODE_CLIENT);

    if (srv_addr)
    {
        dest.sin_family = AF_INET;
        dest.sin_port = htons((u_short)(port + 1));
        dest.sin_addr.s_addr = inet_addr(srv_addr);

        if (dest.sin_addr.s_addr != INADDR_NONE) /* ���͵������ݱ� */
            sendto(clt_soc, (char *)&sntp_msg, sizeof(sntp_msg), 0,
                   (struct sockaddr *)&dest, sizeof(dest));
    }

    printf("[SNTP] is sychronizing ... ...\n");
    while (1)
    {
        memset(&sntp_msg, 0, sizeof(sntp_msg));
        result = recvfrom(clt_soc, (char *)&sntp_msg, sizeof(sntp_msg),
                          0, (struct sockaddr *)&dest, &addr_len);
        if (result == SOCKET_ERROR)
        {
            printf("[SNTP] recvfrom() error : %d\n", WSAGetLastError());
            break;
        }

        sntp_get_sys_time(&now_fp);
        sntp_recv_handle(&sntp_msg, result, &now_fp);
    }

    /* �뿪�ಥ�� */
    result = setsockopt(clt_soc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                        (char *)&mcast_addr, sizeof(mcast_addr));

    closesocket(clt_soc);
    WSACleanup();

    return 0;
}
