/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  Bootp.h
 *
 * PURPOSE :  定义数据结构及常量.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef BOOTP_H_
#define BOOTP_H_

/*Bootstrap 常量 */
#define BOOTREQUEST      1
#define BOOTREPLY        2

#define BOOTP_HRD_TYPE   1   /* 硬件类型 */
#define BOOTP_HRD_LEN    6   /* 硬件长度 */
#define BOOTP_ADDR_LEN   4   /* IP 地址长度 */

#define BOOTP_SRV_PORT  67   /* Bootstrap 服务器端口	*/
#define BOOTP_CLN_PORT  68   /* Bootstrap 客户端端口	*/
#define BOOTP_DEF_COUNT 10   /* 发送的次数 */

#define BOOTP_YOUR_ADDR "192.168.50.3" /* 给客户分配的地址 */
#define BOOTP_FILE_NAME "c:\\bootp.bin"/* 启动文件名 */

struct bootstrap_s
{
    unsigned char op;       /* message type */
    unsigned char htype;    /* hardware address type */
    unsigned char hlen;     /* hardware addresslength */
    unsigned char hops;     /* used by gateway */
    unsigned long xid;      /* transaction ID */
    unsigned short secs;    /* seconds elapsed */
    unsigned short unused;
    unsigned long ciaddr;   /* client IP address */
    unsigned long yiaddr;   /* 'your' (client) IP address */
    unsigned long siaddr;   /* server IP address */
    unsigned long giaddr;   /* gateway IP address */
    char chaddr[16];        /* client hardware address */
    char sname[64];         /* optional server host name */
    char file[128];         /* boot file name */
    char vend[64];          /* optional vendor-specific area */
};

#endif /* BOOTP_H_ */