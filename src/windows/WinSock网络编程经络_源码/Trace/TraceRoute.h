/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  TraceRoute.h
 *
 * PURPOSE :  �������ݽṹ������.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef TRACE_ROUTE_H_
#define TRACE_ROUTE_H_

/* IP �ײ� -- RFC 791 */
struct ip_hdr
{
    unsigned char vers_len;     /* �汾���ײ����� */
    unsigned char tos;          /* �������� */
    unsigned short total_len;   /* ���ݱ����ܳ��� */
    unsigned short id;          /* ��ʶ�� */
    unsigned short frag;        /* ��־��Ƭƫ�� */
    unsigned char ttl;          /* ����ʱ�� */
    unsigned char proto;        /* Э�� */
    unsigned short checksum;    /* У��� */
    unsigned int sour;          /* Դ IP ��ַ */
    unsigned int dest;          /* Ŀ�� IP ��ַ */
};

/* ICMP �ײ� -- RFC 792 */
struct icmp_hdr
{   
    unsigned char type;         /* ���� */
    unsigned char code;         /* ���� */
    unsigned short checksum;    /* У��� */
    unsigned short id;          /* ��ʶ�� */
    unsigned short seq;         /* ���к� */
};

/* UDP �ײ� -- RFC 768 */
struct udp_hdr
{
    unsigned short sport;       /* Դ�˿�  */
    unsigned short dport;       /* Ŀ�Ķ˿�*/
    unsigned short length;      /* ��ʶ��  */
    unsigned short checksum;    /* У���  */
};

struct trace_control
{
    unsigned int  max_hop;    /* ����Ŀ����������� */
    unsigned int  timeout;    /* �ȴ��𸴵ĳ�ʱʱ��:ms*/
    unsigned int  use_udp;   /* ʹ�� ICMP ����̽��   */
    unsigned long timestamp;  /* ���͵�ʱ���         */
    char          *host;      /* ������ */
    unsigned short sport;     /* Դ�˿�, �����ֽ���   */
};


/* ICMP ��Ϣ���� */
#define ICMP_ECHO_REPLY     0
#define ICMP_DEST_UNREACH   3
#define ICMP_ECHO           8
#define ICMP_TIME_EXCEEDED  11

/* ICMP ��Ϣ���� */
#define ICMP_UNREACH_PORT   3
#define ICMP_EXCEED_TRANS   0

#define ICMP_HDR_SIZE       8     /* ICMP ͷ��Сֵ */

#define TRACE_MAX_TIMES     3     /* ��Ϣ���͵Ĵ���   */
#define TRACE_MAX_HOP       30    /* �������         */
#define TRACE_DEF_TIMEOUT   5000  /* ���յĳ�ʱʱ��   */
#define TRACE_PKT_SIZE      32    /* ���ͷ���Ĵ�С   */
#define TRACE_PKT_MAX       256   /* ���յ���󻺳��� */
#define TRACE_DEST_PORT     32700 /* UDP ��ʼĿ�Ķ˿� */

/* ��������ֵ */
#define TRACE_DONE         -1
#define TRACE_EXCEEDED     -2
#define TRACE_TIMEOUT      -3
#define TRACE_ERROR        -4

#endif /* TRACE_ROUTE_H_  */
