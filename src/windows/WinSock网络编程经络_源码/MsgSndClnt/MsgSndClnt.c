/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndClnt.c
 *
 * PURPOSE :  实现 Message Send Protocol 2 客户端程序.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

#define MSG_SEND_VERS         'B' /* 版本     */
#define MSG_TERMINAL    "Console" /* 终端名称 */
#define MSG_SEND_PORT         18  /* 消息发送协议缺省端口*/
#define MSG_BUF_SIZE         512  /* 缓冲区的最大长度    */
#define MSG_EACH_SIZE        160  /* 每一部分最大长度    */
#define MSG_COOKIE_LEN        15  /* Cookie 的最小长度   */

enum msg_status /* 输入的状态 */
{
    MSG_RECIP,
    MSG_CONTENT,
    MSG_SENDER,
    MSG_TOTAL
};

/**************************************************************************
 *
 * 函数功能: 根据当前的日期和时间, 生成 COOKIE.
 *
 * 参数说明: [OUT] buf, 保存数据的缓冲区;
 *           [IN] len, buf 的长度;
 *
 * 返 回 值: 成功返回缓冲区中数据的长度, 失败返回 0.
 *
 **************************************************************************/
int msp_make_cookie(char *buf, int len)
{
    int result;
    time_t t;
    struct tm *time_p;

    if (len < MSG_COOKIE_LEN)
        return 0;
    
    t = time(NULL);
    time_p = localtime(&t);
    result = sprintf(buf, "%04d%02d%02d%02d%02d%02d", time_p->tm_year+1900,
                     time_p->tm_mon + 1, time_p->tm_mday, time_p->tm_hour, 
                     time_p->tm_min, time_p->tm_sec);

    return result;
}

/**************************************************************************
 *
 * 函数功能: 接收用户的输入并向服务器发送消息.
 *
 * 参数说明: [IN] msg_soc, socket 描述符;
 *           [IN] prompt, 是否显示菜单选择信息;
 *
 * 返 回 值: 成功返回发送的数据长度, 失败返回值小于等于 0.
 *
 **************************************************************************/
int msp_input_send(SOCKET msg_soc, int prompt)
{
    int offset, result, menu, status = MSG_RECIP;
    int term_len = strlen(MSG_TERMINAL) + 1;
    char send_buf[MSG_BUF_SIZE], *msg = send_buf;
    
    /* 请用户选择是否继续? */
    if (prompt)
    {
        printf("\n------- MENU -------\n");
        printf("*  1 Send Message  *\n");
        printf("*  2 Bye           *\n");
        printf("--------------------\n");
        scanf("%d", &menu);

        if (menu != 1)
            return 0;

        fflush(stdin);
    }

    *msg++ = MSG_SEND_VERS; /* 版本 */
    
    printf("\nTo : ");
    while (fgets(msg, MSG_EACH_SIZE, stdin) != NULL)
    {
        offset = strlen(msg);
        *(msg + offset - 1) = 0; /* 去掉结尾的 '\r' */

        switch (status)
        {
        case MSG_RECIP:            
            strcpy(msg + offset, MSG_TERMINAL);
            offset += term_len; /* 终端名的长度, 包括结束符 */
            printf("Message : ");
            break;

        case MSG_CONTENT:
            printf("From : ");
            break;

        case MSG_SENDER:
            strcpy(msg + offset, MSG_TERMINAL);
            offset += term_len; /* 终端名的长度, 包括结束符 */

            /* 构造 COOKIE */
            msg += offset;
            result = msp_make_cookie(msg, send_buf + MSG_BUF_SIZE - msg);
            msg += (result + 1);
            *msg = 0; /* SIGNATURE 为空 */
            offset = 1;
            break;

        default:
            break;
        }

        msg += offset;
        if (++status == MSG_TOTAL)
            break;
    }

    /* 发送数据 */
    result = msg - send_buf;
    result = send(msg_soc, send_buf, result, 0);
    if (result == SOCKET_ERROR)
        printf("[MSP] Fail to send %d\n", WSAGetLastError());

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET msg_soc = 0; /* 客户端 socket 句柄 */
    struct sockaddr_in srv_addr;   /* 客户端地址 */
    unsigned short port = MSG_SEND_PORT;
    int result = 0, ready, addr_len = sizeof(srv_addr);
    u_long non_block = 1;
    char data_buf[MSG_BUF_SIZE], *ip_addr = "127.0.0.1";
    fd_set read_all, read_set, write_all, *write_fds = NULL;
    struct timeval timeval = { 60, 0 };

    /* 解析命令行参数 */
    switch (argc)
    {
    case 3:
        port = atoi(argv[2]);
    case 2:
        ip_addr = argv[1];
        break;
    }

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* 初始化 WinSock 资源 */
    msg_soc = socket(AF_INET, SOCK_STREAM, 0);
    result = ioctlsocket(msg_soc, FIONBIO, &non_block);

    /* 设置 socket 集合 */
    FD_ZERO(&read_all);
    FD_ZERO(&write_all);
    FD_SET(msg_soc, &read_all);

    /* 填写要连接的服务器 */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    result = connect(msg_soc, (struct sockaddr *)&srv_addr, addr_len);
    if (result == 0) /* 连接成功 */
        write_fds = NULL;
    else if (WSAGetLastError() == WSAEWOULDBLOCK)
    {
        write_fds = &write_all;
        FD_SET(msg_soc, &write_all);
    }
    else
    {
        closesocket(msg_soc);
        return -1;
    }

    while (1)
    {
        read_set = read_all;
        ready = select(0, &read_set, write_fds, NULL, &timeval);
        if (ready <= 0) /* 出错或超时 */
        {
            printf("[MSP] Select error.\n");
            break;
        }

        /* 连接成功的 socket 是可写的 */
        if (write_fds && FD_ISSET(msg_soc, write_fds))
        {
            msp_input_send(msg_soc, 0);
            write_fds = NULL;
        }

        if (FD_ISSET(msg_soc, &read_set)) /* socket 有数据可以读 */
        {
            result = recv(msg_soc, data_buf, MSG_BUF_SIZE, 0);
            if (result > 0)
            {
                data_buf[result] = 0;
                printf("[MSP] Receive : %s\n", data_buf);
            }
            else
                break;

            result = msp_input_send(msg_soc, 1); /* 重新输入并发送 */
            if (result <= 0)
                break;
        }
    }

    closesocket(msg_soc);
    WSACleanup();
    
    return 0;
}