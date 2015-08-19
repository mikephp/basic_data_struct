/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  BootpClnt.c
 *
 * PURPOSE :  Bootstrap �ͻ��˳���.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <nb30.h>
#include "Bootp.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */
#pragma comment(lib,"netapi32.lib") /* netbios api */ 

typedef struct adp_name_s
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    name[4];
} ADP_NAME;


/**************************************************************************
 *
 * ��������: �õ���̫����Ӳ����ַ.
 *
 * ����˵��: [OUT] mac_addr, Ӳ����ַ������.
 *           [IN] length, mac_addr �ĳ���.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
static int bootp_get_mac_addr(char *mac_addr, int length)
{
    NCB ncb;
    UCHAR ret_code;
    LANA_ENUM lana_enum;
    ADP_NAME adapter;
    int i;
    
    /* �õ� LANA ���� */
    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer = (UCHAR *)&lana_enum;
    ncb.ncb_length = sizeof(lana_enum);
    ret_code = Netbios(&ncb);
    
    for (i = 0; i < lana_enum.length; i++)
    {
        /* �������� */
        memset(&ncb, 0, sizeof(ncb));
        ncb.ncb_command = NCBRESET;
        ncb.ncb_lana_num = lana_enum.lana[i];
        ret_code = Netbios(&ncb);
        
        /* �õ�Ӳ����Ϣ */
        memset(&ncb, 0, sizeof (ncb));
        ncb.ncb_command = NCBASTAT;
        ncb.ncb_lana_num = lana_enum.lana[i];
        
        memset(ncb.ncb_callname, 0x20, NCBNAMSZ);
        ncb.ncb_callname[0] = '*';
        ncb.ncb_buffer = (char *)&adapter;
        ncb.ncb_length = sizeof(adapter);
        
        ret_code = Netbios(&ncb);
        
        /* NRC_GOODRET ��ʾ�ɹ�, 0xFE ����̫�� */
        if (ret_code != NRC_GOODRET || adapter.adapt.adapter_type != 0xFE)
            continue;
        
        /* ��������˵�����Եõ�Ӳ����ַ */
        length = length > BOOTP_HRD_LEN ? BOOTP_HRD_LEN : length;          
        memcpy(mac_addr, adapter.adapt.adapter_address, length);

        return 0;
    }

    return -1;
}


/**************************************************************************
 *
 * ��������: ����ͻ��˵�������Ϣ.
 *
 * ����˵��: [IN] bootp_msg, ���յ������ݻ�����.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
static int bootp_make_request(struct bootstrap_s *bootp_msg)
{
    int result;

    memset(bootp_msg, 0, sizeof(*bootp_msg));
    bootp_msg->op = BOOTREQUEST;
    bootp_msg->htype = BOOTP_HRD_TYPE;
    bootp_msg->hlen = BOOTP_HRD_LEN;
    bootp_msg->xid = time(NULL);
    bootp_msg->secs = (unsigned short)GetTickCount();
    result = bootp_get_mac_addr(bootp_msg->chaddr, BOOTP_HRD_LEN);

    return result;
}


/**************************************************************************
 *
 * ��������: �����������Ӧ��.
 *
 * ����˵��: [IN] bootp_msg, ���յ������ݻ�����;
 *           [IN] length, bootp_msg �ĳ���.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
static int bootp_handle_reply(struct bootstrap_s *bootp_msg, int length)
{
    struct in_addr taddr;

    /* ��鳤�� */
    if (length != sizeof(struct bootstrap_s) ||
        bootp_msg->op != BOOTREPLY)
        return -1;

    /* ���Ӳ����Ϣ */
    if (bootp_msg->htype != BOOTP_HRD_TYPE || 
        bootp_msg->hlen != BOOTP_HRD_LEN)
        return -1;

    printf("[BOOTP] bootstrap configuration as follows\n");
    printf("------------------------------------------\n");
    taddr.s_addr = bootp_msg->yiaddr;
    printf("yiaddr : %s\n", inet_ntoa(taddr));

    taddr.s_addr = bootp_msg->siaddr;
    printf("siaddr : %s\n", inet_ntoa(taddr));

    printf("sname  : %s\n", bootp_msg->sname);
    printf("file   : %s\n", bootp_msg->file);
    printf("------------------------------------------\n");

    return 0;
}

int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  boot_soc = 0;  /* Bootstrap socket ��� */
    struct sockaddr_in t_addr, serv_addr;
    int i, result, addr_len = sizeof(serv_addr);    
    int timeout = (1 + rand() % 4) * 1000;  /* ���� */
    BOOL brdcast = TRUE;
    struct bootstrap_s bootp_msg, reply_msg;
    int msg_len = sizeof(bootp_msg);

    WSAStartup(WINSOCK_VERSION, &wsa_data);/* ��ʼ�� WinSock ��Դ */
    boot_soc = socket(AF_INET, SOCK_DGRAM, 0); /* ���� Bootstrap socket */

    /* ��������㲥ѡ�� */
    setsockopt(boot_soc, SOL_SOCKET, SO_BROADCAST, 
               (char *)&brdcast, sizeof(brdcast));

    /* ���ñ��ص�ַ */
    t_addr.sin_family = AF_INET;
    t_addr.sin_port = htons(BOOTP_CLN_PORT);
    t_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(boot_soc, (struct sockaddr *)&t_addr, addr_len);
    if (result == SOCKET_ERROR)
    {
        closesocket(boot_soc);
        printf("[BOOTP] bind() error : %d\n", WSAGetLastError());
        return -1;
    }

    bootp_make_request(&bootp_msg);

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(BOOTP_SRV_PORT);
    serv_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    for (i = 0; i < BOOTP_DEF_COUNT; i++)
    {
        timeout = timeout > 64000 ? 64000 : timeout;/* � 64 �� */
        setsockopt(boot_soc, SOL_SOCKET, SO_RCVTIMEO, 
                   (char *)&timeout, sizeof(int));

        result = sendto(boot_soc, (char *)&bootp_msg, msg_len, 0,
                        (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        result = recvfrom(boot_soc, (char *)&reply_msg, msg_len, 0, 
                          (struct sockaddr *)&t_addr, &addr_len);

        result = bootp_handle_reply(&reply_msg, result);
        if (result == 0)
            break;

        timeout <<= 1;
    }

    closesocket(boot_soc);
    WSACleanup();

    return 0;
}
