/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  Bootp.h
 *
 * PURPOSE :  �������ݽṹ������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef BOOTP_H_
#define BOOTP_H_

/*Bootstrap ���� */
#define BOOTREQUEST      1
#define BOOTREPLY        2

#define BOOTP_HRD_TYPE   1   /* Ӳ������ */
#define BOOTP_HRD_LEN    6   /* Ӳ������ */
#define BOOTP_ADDR_LEN   4   /* IP ��ַ���� */

#define BOOTP_SRV_PORT  67   /* Bootstrap �������˿�	*/
#define BOOTP_CLN_PORT  68   /* Bootstrap �ͻ��˶˿�	*/
#define BOOTP_DEF_COUNT 10   /* ���͵Ĵ��� */

#define BOOTP_YOUR_ADDR "192.168.50.3" /* ���ͻ�����ĵ�ַ */
#define BOOTP_FILE_NAME "c:\\bootp.bin"/* �����ļ��� */

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