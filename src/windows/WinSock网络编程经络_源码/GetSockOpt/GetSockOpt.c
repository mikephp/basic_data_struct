/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  GetSockOpt.c
 *
 * PURPOSE :  得到 socket 选项默认值.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

enum /* 选项的类型 */
{
    VBOOL,
    VCHAR,
    VINTG,
    VLGER
};

union opt_val /* 选项值 */
{
    BOOL b_value;
    char c_value;
    int i_value;
    struct linger linger;
};

struct sock_opt /* 选项信息 */
{
    char *name;     /* 字符串名 */
    int level;      /* 选项等级 */
    int option;     /* 选项名字 */
    int type;       /* 类型     */
    int protocol;   /* 适用协议: 0 - UDP, 1 - TCP */
};

/* 选项信息全局变量 */
struct sock_opt sock_opt_g[] = 
{
    /* level = SOL_SOCKET */
    {"SO_ACCEPTCONN",  SOL_SOCKET, SO_ACCEPTCONN,  VBOOL, 1},
    {"SO_BROADCAST",   SOL_SOCKET, SO_BROADCAST,   VBOOL, 0},
    {"SO_DEBUG",       SOL_SOCKET, SO_DEBUG,       VBOOL, 0},
    {"SO_DONTLINGER",  SOL_SOCKET, SO_DONTLINGER,  VBOOL, 1},
    {"SO_DONTROUTE",   SOL_SOCKET, SO_DONTROUTE,   VCHAR, 0},
    {"SO_ERROR",       SOL_SOCKET, SO_ERROR,       VINTG, 0},
    {"SO_KEEPALIVE",   SOL_SOCKET, SO_KEEPALIVE,   VCHAR, 1},
    {"SO_LINGER",      SOL_SOCKET, SO_LINGER,      VLGER, 1},
    {"SO_MAX_MSG_SIZE",SOL_SOCKET, SO_MAX_MSG_SIZE,VINTG, 0},
    {"SO_OOBINLINE",   SOL_SOCKET, SO_OOBINLINE,   VBOOL, 1},
    {"SO_RCVBUF",      SOL_SOCKET, SO_RCVBUF,      VINTG, 1},
    {"SO_RCVTIMEO",    SOL_SOCKET, SO_RCVTIMEO,    VINTG, 1},
    {"SO_REUSEADDR",   SOL_SOCKET, SO_REUSEADDR,   VBOOL, 1},
    {"SO_SNDBUF",      SOL_SOCKET, SO_SNDBUF,      VINTG, 1},
    {"SO_SNDTIMEO",    SOL_SOCKET, SO_SNDTIMEO,    VINTG, 1},
    {"SO_TYPE",        SOL_SOCKET, SO_TYPE,        VINTG, 1},

    /* level = IPPROTO_IP */
    {"IP_DONTFRAGMENT",IPPROTO_IP, IP_DONTFRAGMENT,VBOOL, 0},
    {"IP_HDRINCL",     IPPROTO_IP, IP_HDRINCL,     VBOOL, 0},
    {"IP_TOS",         IPPROTO_IP, IP_TOS,         VINTG, 0},
    {"IP_TTL",         IPPROTO_IP, IP_TTL,         VINTG, 0},

    /* level = IPPROTO_UDP */
    {"UDP_NOCHECKSUM", IPPROTO_UDP,UDP_NOCHECKSUM, VCHAR, 0},

    /* level = IPPROTO_TCP */
    {"TCP_NODELAY",    IPPROTO_TCP,TCP_NODELAY,    VCHAR, 1},
  {"TCP_EXPEDITED_1122",IPPROTO_TCP,TCP_EXPEDITED_1122,VCHAR,1}
};


/**************************************************************************
 *
 * 函数功能: 打印 socket 的默认选项值.
 *
 * 参数说明: [IN] type, 选项数据类型;
 *           [IN] value, 选项值;
 *           [IN] len, value 的长度.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void sock_opt_print(int type, union opt_val *value, int len)
{
    BOOL b_val;
    int i_val;
    unsigned char c_val;
    struct linger lng;

    //printf("len = %d, ", len);
    switch (type)
    {
    case VBOOL:
        b_val = value->b_value;
        printf("%s\n", b_val ? "on" : "off");
        break;

    case VCHAR:
        c_val = value->c_value;
        printf("%s\n", c_val ? "on" : "off");
        break;

    case VINTG:
        i_val = value->i_value;
        printf("%d\n", i_val);
        break;

    case VLGER:
        lng = value->linger;
        printf("l_onoff = %d, l_linger = %d\n", lng.l_onoff, lng.l_linger);
        break;

    default:
        break;
   }
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET	udp_soc, tcp_soc, soc; /* socket 句柄 */
    union opt_val value;
    int i, result, val_len;
    int opt_cnt = sizeof(sock_opt_g) / sizeof(sock_opt_g[0]);
    struct sock_opt *popt;

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* 初始化 WinSock 资源 */      
    udp_soc = socket(AF_INET, SOCK_DGRAM, 0);
    tcp_soc = socket(AF_INET, SOCK_STREAM, 0);

    printf("[GetOpt] Get socket default option:\n");
    for (i = 0; i < opt_cnt; i++)
    {
        popt = &sock_opt_g[i];
        printf("%-20s : ", popt->name);
        val_len = sizeof(value);

        soc = (popt->protocol ? tcp_soc : udp_soc);
        result = getsockopt(soc, popt->level, popt->option, 
                            (char *)&value, &val_len);
        if (result == SOCKET_ERROR)
            printf("fails to getsockopt\n");
        else
            sock_opt_print(popt->type, &value, val_len);
   }

    closesocket(udp_soc);
    closesocket(tcp_soc);
    WSACleanup();
    
    return 0;
}