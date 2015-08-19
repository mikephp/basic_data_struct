/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndSrv.c
 *
 * PURPOSE :  实现 Message Send Protocol 2 服务器程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "MsgSndSrv.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

/* 协议中字段的枚举值及对应的名字 */
static struct field_name msp_info[MSP_TOTAL] = {
    { MSP_VERSION,     "VERSION"    },
    { MSP_RECIP,       "RECIPIENT"  },
    { MSP_RECIP_TERM,  "RECIP-TERM" },
    { MSP_MESSAGE,     "MESSAGE"    },
    { MSP_SENDER,      "SENDER"     },
    { MSP_SEND_TERM,   "SENDER-TERM"},
    { MSP_COOKIE,      "COOKIE"     },
    { MSP_SIGNAT,      "SIGNATURE"  }
};

static SOCKET msp_client_soc[FD_SETSIZE]; /* 记录接受的客户连接 */
static int client_max;


/**************************************************************************
 *
 * 函数功能: 接收到客户端的连接请求.
 *
 * 参数说明: [IN]  lstn_soc, 监听 socket;
 *           [OUT] soc_set, 新接受的 socket 需要加入的集合;
 *
 * 返 回 值: 成功返回新接受的 socket, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
SOCKET msp_new_client(SOCKET lstn_soc, fd_set *soc_set)
{
    int i;
    struct sockaddr_in faddr;
    int addr_len = sizeof(faddr);
    SOCKET acpt_soc;

    acpt_soc = accept(lstn_soc, (struct sockaddr *)&faddr, &addr_len);
    if (acpt_soc == INVALID_SOCKET)
        return INVALID_SOCKET;

    printf("[MSP] accept connection : %s\n", inet_ntoa(faddr.sin_addr));
    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (msp_client_soc[i] == INVALID_SOCKET)
        {
            msp_client_soc[i] = acpt_soc;
            break;
        }
    }

    if (i == FD_SETSIZE) /* 已经满了 */
    {
        closesocket(acpt_soc);
        return INVALID_SOCKET;
    }

    if (i > client_max)
        client_max = i;

    FD_SET(acpt_soc, soc_set);

    return acpt_soc;
}

/**************************************************************************
 *
 * 函数功能: 关闭客户端的套接口描述符, 套接口从集合 soc_set 中删除。.
 *
 * 参数说明: [IN]  index, 套接口在全局变量msp_client_soc中对应的下标;
 *           [OUT] soc_set, 套接口从这个集合中删除;
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void msp_close_client(int index, fd_set *soc_set)
{
    SOCKET sock = msp_client_soc[index]; 

    closesocket(sock);
    FD_CLR(sock, soc_set);
    msp_client_soc[index] = msp_client_soc[client_max];
    msp_client_soc[client_max--] = INVALID_SOCKET;
}

/**************************************************************************
 *
 * 函数功能: 接收客户端的数据.
 *
 * 参数说明: [IN]  在客户 socket 数组中的索引;
 *           [OUT] buf, 接收数据的缓冲区;
 *           [IN]  len, buf 的长度;
 *           [OUT] from, 客户端地址;
 *
 * 返 回 值: 成功返回接收的字节数, 失败返回 SOCKET_ERROR.
 *
 **************************************************************************/
int msp_recv_data(int i, char *buf, int len, struct sockaddr_in *from)
{
    int result, size = sizeof(*from);
    SOCKET soc = msp_client_soc[i];

    if (i == 0) /* 基于 UDP 服务的 socket */
        result = recvfrom(soc, buf, len, 0, (struct sockaddr *)from, &size);
    else
        result = recv(soc, buf, len, 0); /* 基于 TCP 服务的 socket */

    return result;
}

/**************************************************************************
 *
 * 函数功能: 解析消息发送协议接收到的数据.
 *
 * 参数说明: [IN] buf, 接收到的数据缓冲区;
 *           [IN] len, buf 的长度;
 *
 * 返 回 值: 成功返回 MSG_SUCCESS, 失败返回值小于 0.
 *
 **************************************************************************/
int msp_process_request(char *buf, int len)
{
    char *sep, *buf_end = buf + len;
    char *msp_field[MSP_TOTAL] = { NULL };
    int i = MSP_RECIP;

    if (buf[0] != MSG_SEND_VERS) /* 检查版本 */
        return MSG_VERSION_ERR;

    /* 解析其它字段值 */
    msp_field[MSP_RECIP] = ++buf;
    sep = memchr(buf, 0, buf_end - buf);

    while (sep && (sep < buf_end))
    {
        msp_field[++i] = sep + 1;

        if (i == MSP_SIGNAT)
            break;

        buf = sep + 1;
        sep = memchr(buf, 0, buf_end - buf);
    }

    if (i != MSP_SIGNAT)
        return MSG_FIELD_ERR;

    /* 在屏幕上输出收到的信息 */
    printf("________________________________\n");
    for (i = MSP_RECIP; i < MSP_TOTAL; i++)
    {
        printf("%s : %s\n", msp_info[i].name, 
               msp_field[i][0] ? msp_field[i] : "Empty");
    }
    printf("________________________________\n");

    return MSG_SUCCESS;
}

/**************************************************************************
 *
 * 函数功能: 向客户端发送应答.
 *
 * 参数说明: [IN]  在客户 socket 数组中的索引;
 *           [IN]  error, 处理结果;
 *           [IN]  from, 客户端地址;
 *
 * 返 回 值: 成功返回发送的字节数, 失败返回 SOCKET_ERROR.
 *
 **************************************************************************/
int msp_send_reply(int i, int error, struct sockaddr_in *from)
{    
    int result, msg_len, from_len = sizeof(*from);
    SOCKET client_soc = msp_client_soc[i]; /* 与客户交互的 socket */
    char *msg, *reply_desc[] = { "+ Message is accepted.", 
                                 "- Version is not match.",
                                 "- Message format is wrong.", 
                                 "- Unknow error." };

    msg = reply_desc[-error];
    msg_len = strlen(msg);
    
    if (i == 0) /* 基于 UDP 的服务器 */
        result = sendto(client_soc, msg, msg_len, 0, 
                        (struct sockaddr *)from, from_len);
    else 
        result = send(client_soc, msg, msg_len, 0); /* 基于 TCP 的服务器 */

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET srv_soc_t = 0, srv_soc_u; /* 服务器的socket句柄 */
    struct sockaddr_in srv_addr, clnt_addr;
    unsigned short port = MSG_SEND_PORT;
    int i, result, ready_cnt, addr_len = sizeof(srv_addr);
    char recv_buf[MSG_BUF_SIZE+1];
    SOCKET new_soc, client_soc;
    fd_set read_all, read_set;

    if (argc == 2)
        port = atoi(argv[1]);

    /* 初始化客户连接的 socket */
    for (i = 0; i < FD_SETSIZE; i++)
        msp_client_soc[i] = INVALID_SOCKET;

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* 初始化 WinSock 资源 */
    srv_soc_t = socket(AF_INET, SOCK_STREAM, 0);/* 基于 TCP socket */

    /* 消息发送协议服务器地址 */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    /* 绑定服务器地址和端口 */
    result = bind(srv_soc_t, (struct sockaddr *)&srv_addr, addr_len);
    if (result == SOCKET_ERROR)
    {
        printf("[MSP] fail to bind: %d\n", WSAGetLastError());
        closesocket(srv_soc_t);
        return -1;
    }

    listen(srv_soc_t, SOMAXCONN);
    srv_soc_u = socket(AF_INET, SOCK_DGRAM, 0); /* 基于 UDP socket */
    bind(srv_soc_u, (struct sockaddr *)&srv_addr, addr_len);
        
    FD_ZERO(&read_all);
    FD_SET(srv_soc_t, &read_all);
    FD_SET(srv_soc_u, &read_all);
    msp_client_soc[0] = srv_soc_u; /* UDP socket 固定保存在第一个位置 */

    printf("[MSP] server is running ... ...\n");
    while (1)
    {
        read_set = read_all;
        ready_cnt = select(0, &read_set, NULL, NULL, NULL);
        if (ready_cnt == SOCKET_ERROR)
        {
            printf("[MSP] select error: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(srv_soc_t, &read_set)) /* 检查 TCP 的侦听 socket */
        {
            new_soc = msp_new_client(srv_soc_t, &read_all);
            if (--ready_cnt <= 0)
                continue;
        }

        for (i = 0; (i <= client_max) && (ready_cnt > 0); i++)
        {
            client_soc = msp_client_soc[i];
            if (!FD_ISSET(client_soc, &read_set))
                continue;

            /* 接收数据 */
            result = msp_recv_data(i, recv_buf, MSG_BUF_SIZE, &clnt_addr);
            if (result <= 0)
            {
                msp_close_client(i, &read_all);
                --ready_cnt;
                continue;
            }

            result = msp_process_request(recv_buf, result); /* 处理请求 */
            result = msp_send_reply(i, result, &clnt_addr); /* 发送应答 */
            if (result == SOCKET_ERROR)
                msp_close_client(i, &read_all);

            --ready_cnt;
        }        
    }

    /* 关闭与客户端的 socket */
    for (i = 0; i <= client_max; i++)
        closesocket(msp_client_soc[i]);

    closesocket(srv_soc_t);
    WSACleanup();

    return 0;
}
