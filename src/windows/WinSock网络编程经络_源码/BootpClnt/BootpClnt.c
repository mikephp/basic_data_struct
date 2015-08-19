/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  BootpClnt.c
 *
 * PURPOSE :  Bootstrap 客户端程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <nb30.h>
#include "Bootp.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */
#pragma comment(lib,"netapi32.lib") /* netbios api */ 

typedef struct adp_name_s
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    name[4];
} ADP_NAME;


/**************************************************************************
 *
 * 函数功能: 得到以太网的硬件地址.
 *
 * 参数说明: [OUT] mac_addr, 硬件地址缓冲区.
 *           [IN] length, mac_addr 的长度.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
 *
 **************************************************************************/
static int bootp_get_mac_addr(char *mac_addr, int length)
{
    NCB ncb;
    UCHAR ret_code;
    LANA_ENUM lana_enum;
    ADP_NAME adapter;
    int i;
    
    /* 得到 LANA 个数 */
    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer = (UCHAR *)&lana_enum;
    ncb.ncb_length = sizeof(lana_enum);
    ret_code = Netbios(&ncb);
    
    for (i = 0; i < lana_enum.length; i++)
    {
        /* 重设网卡 */
        memset(&ncb, 0, sizeof(ncb));
        ncb.ncb_command = NCBRESET;
        ncb.ncb_lana_num = lana_enum.lana[i];
        ret_code = Netbios(&ncb);
        
        /* 得到硬件信息 */
        memset(&ncb, 0, sizeof (ncb));
        ncb.ncb_command = NCBASTAT;
        ncb.ncb_lana_num = lana_enum.lana[i];
        
        memset(ncb.ncb_callname, 0x20, NCBNAMSZ);
        ncb.ncb_callname[0] = '*';
        ncb.ncb_buffer = (char *)&adapter;
        ncb.ncb_length = sizeof(adapter);
        
        ret_code = Netbios(&ncb);
        
        /* NRC_GOODRET 表示成功, 0xFE 是以太网 */
        if (ret_code != NRC_GOODRET || adapter.adapt.adapter_type != 0xFE)
            continue;
        
        /* 到达这里说明可以得到硬件地址 */
        length = length > BOOTP_HRD_LEN ? BOOTP_HRD_LEN : length;          
        memcpy(mac_addr, adapter.adapt.adapter_address, length);

        return 0;
    }

    return -1;
}


/**************************************************************************
 *
 * 函数功能: 构造客户端的请求消息.
 *
 * 参数说明: [IN] bootp_msg, 接收到的数据缓冲区.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
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
 * 函数功能: 处理服务器的应答.
 *
 * 参数说明: [IN] bootp_msg, 接收到的数据缓冲区;
 *           [IN] length, bootp_msg 的长度.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
 *
 **************************************************************************/
static int bootp_handle_reply(struct bootstrap_s *bootp_msg, int length)
{
    struct in_addr taddr;

    /* 检查长度 */
    if (length != sizeof(struct bootstrap_s) ||
        bootp_msg->op != BOOTREPLY)
        return -1;

    /* 检查硬件信息 */
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
    SOCKET  boot_soc = 0;  /* Bootstrap socket 句柄 */
    struct sockaddr_in t_addr, serv_addr;
    int i, result, addr_len = sizeof(serv_addr);    
    int timeout = (1 + rand() % 4) * 1000;  /* 毫秒 */
    BOOL brdcast = TRUE;
    struct bootstrap_s bootp_msg, reply_msg;
    int msg_len = sizeof(bootp_msg);

    WSAStartup(WINSOCK_VERSION, &wsa_data);/* 初始化 WinSock 资源 */
    boot_soc = socket(AF_INET, SOCK_DGRAM, 0); /* 创建 Bootstrap socket */

    /* 设置允许广播选项 */
    setsockopt(boot_soc, SOL_SOCKET, SO_BROADCAST, 
               (char *)&brdcast, sizeof(brdcast));

    /* 设置本地地址 */
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

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(BOOTP_SRV_PORT);
    serv_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    for (i = 0; i < BOOTP_DEF_COUNT; i++)
    {
        timeout = timeout > 64000 ? 64000 : timeout;/* 最长 64 秒 */
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
