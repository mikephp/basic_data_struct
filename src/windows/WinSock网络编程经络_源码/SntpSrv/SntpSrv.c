/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  SntpSrv.c
 *
 * PURPOSE :  简单网络时间协议多播服务器程序.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "Sntp.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */


/**************************************************************************
 *
 * 函数功能: 发送时间信息.
 *
 * 参数说明: [IN] sock, socket 句柄;
 *           [IN] dest, 目的地址;
 *           [IN] xmt,  客户端的传输时间, 网络字节序;
 *           [IN] recv, 接收数据的时间, 主机字节序, xmt != NULL 时才使用;
 *
 * 返 回 值: 成功返回发送的字节数, 失败返回错误码.
 *
 **************************************************************************/
static int sntp_send_packet(SOCKET sock, struct sockaddr_in *dest, 
                            time_fp *xmt, time_fp *recv)
{
    int result;
    unsigned char mode = MODE_BROADCAST;
    struct sntp_s sntp_msg;

    memset(&sntp_msg, 0, sizeof(sntp_msg));

    /* 接收到客户的数据, 包含了传输时间戳 */
    if (xmt)
    {
        mode = MODE_SERVER;
        sntp_msg.orig = *xmt;
        HTONL_FP(recv, &sntp_msg.recv);
    }

    sntp_msg.li_vn_md = MSG_LI_VN_MODE(LEAP_NOT_SYNC, 
                                    SNTP_VERSION, mode);
    sntp_msg.stratum = SNTP_STRATUM;
    
    sntp_get_sys_time(&sntp_msg.xmt);
    printf("[SNTP] Transmit Timestamp: %u : %u\n", sntp_msg.xmt.fp_ui, 
            sntp_msg.xmt.fp_uf);
    HTONL_FP(&sntp_msg.xmt, &sntp_msg.xmt);

    result = sendto(sock, (char *)&sntp_msg, sizeof(sntp_msg), 0,
                    (struct sockaddr *)dest, sizeof(*dest));

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET srv_sock = 0;        /* 服务器的socket句柄 */
    struct sockaddr_in local, dest; /* 本机地址 */
    unsigned short port = SNTP_DEF_PORT;
    int result = 0;
    int addr_len = sizeof(local);
    struct sntp_s sntp_msg;
    fd_set read_set, read_fd;
    time_fp *xmt_p = NULL, recv_t;
    struct timeval timeout = { 64, 0 };

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* 初始化 WinSock 资源 */
    srv_sock = socket(AF_INET, SOCK_DGRAM, 0);/* 创建 SNTP 的socket */

    local.sin_family = AF_INET;
    local.sin_port = htons((u_short)(port + 1));
    local.sin_addr.s_addr = INADDR_ANY;
    
    result = bind(srv_sock, (struct sockaddr *)&local, sizeof(local));
    if (result == SOCKET_ERROR)
    {
        printf("[SNTP] bind() error : %d\n", WSAGetLastError());
        closesocket(srv_sock);
        return -1;
    }

    FD_ZERO(&read_fd);
    FD_SET(srv_sock, &read_fd);

    printf("[SNTP] server is running ... ...\n");
    while (1)
    {
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        dest.sin_addr.s_addr = inet_addr(SNTP_MCAST_ADDR);

        read_set = read_fd;
        xmt_p = NULL;
        
        result = select(0, &read_set, NULL, NULL, &timeout);
        if (result == SOCKET_ERROR)
            break;
        if (result > 0)
        {
            if (!FD_ISSET(srv_sock, &read_set))
                continue;

            sntp_get_sys_time(&recv_t); /* 得到接收数据时的时间 */
            result = recvfrom(srv_sock, (char *)&sntp_msg, sizeof(sntp_msg),
                              0, (struct sockaddr *)&dest, &addr_len);
            if (result == SOCKET_ERROR)
            {
                printf("[SNTP] recvfrom() error : %d\n", WSAGetLastError());
                continue;
            }

            if ((result < sizeof(sntp_msg)) || 
                (MSG_MODE(sntp_msg.li_vn_md) != MODE_CLIENT))
            {
                printf("[SNTP] wrong packet\n");
                continue;
            }

            xmt_p = &sntp_msg.xmt;
            printf("[SNTP] request from : %s\n", inet_ntoa(dest.sin_addr));
        }
        /* else is timeout, distribute the time too. */
        result = sntp_send_packet(srv_sock, &dest, xmt_p, &recv_t);
    }

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}
