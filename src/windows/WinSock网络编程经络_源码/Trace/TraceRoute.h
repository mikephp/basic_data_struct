/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  TraceRoute.h
 *
 * PURPOSE :  定义数据结构及常量.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef TRACE_ROUTE_H_
#define TRACE_ROUTE_H_

/* IP 首部 -- RFC 791 */
struct ip_hdr
{
    unsigned char vers_len;     /* 版本和首部长度 */
    unsigned char tos;          /* 服务类型 */
    unsigned short total_len;   /* 数据报的总长度 */
    unsigned short id;          /* 标识符 */
    unsigned short frag;        /* 标志和片偏移 */
    unsigned char ttl;          /* 生存时间 */
    unsigned char proto;        /* 协议 */
    unsigned short checksum;    /* 校验和 */
    unsigned int sour;          /* 源 IP 地址 */
    unsigned int dest;          /* 目的 IP 地址 */
};

/* ICMP 首部 -- RFC 792 */
struct icmp_hdr
{   
    unsigned char type;         /* 类型 */
    unsigned char code;         /* 代码 */
    unsigned short checksum;    /* 校验和 */
    unsigned short id;          /* 标识符 */
    unsigned short seq;         /* 序列号 */
};

/* UDP 首部 -- RFC 768 */
struct udp_hdr
{
    unsigned short sport;       /* 源端口  */
    unsigned short dport;       /* 目的端口*/
    unsigned short length;      /* 标识符  */
    unsigned short checksum;    /* 校验和  */
};

struct trace_control
{
    unsigned int  max_hop;    /* 搜索目标的最大的跳数 */
    unsigned int  timeout;    /* 等待答复的超时时间:ms*/
    unsigned int  use_udp;   /* 使用 ICMP 发送探测   */
    unsigned long timestamp;  /* 发送的时间戳         */
    char          *host;      /* 主机名 */
    unsigned short sport;     /* 源端口, 网络字节序   */
};


/* ICMP 消息类型 */
#define ICMP_ECHO_REPLY     0
#define ICMP_DEST_UNREACH   3
#define ICMP_ECHO           8
#define ICMP_TIME_EXCEEDED  11

/* ICMP 消息代码 */
#define ICMP_UNREACH_PORT   3
#define ICMP_EXCEED_TRANS   0

#define ICMP_HDR_SIZE       8     /* ICMP 头最小值 */

#define TRACE_MAX_TIMES     3     /* 消息发送的次数   */
#define TRACE_MAX_HOP       30    /* 最大跳数         */
#define TRACE_DEF_TIMEOUT   5000  /* 接收的超时时间   */
#define TRACE_PKT_SIZE      32    /* 发送分组的大小   */
#define TRACE_PKT_MAX       256   /* 接收的最大缓冲区 */
#define TRACE_DEST_PORT     32700 /* UDP 起始目的端口 */

/* 函数返回值 */
#define TRACE_DONE         -1
#define TRACE_EXCEEDED     -2
#define TRACE_TIMEOUT      -3
#define TRACE_ERROR        -4

#endif /* TRACE_ROUTE_H_  */
