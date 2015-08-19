/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  RawEcho.h
 *
 * PURPOSE :  定义数据结构及常量.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef RAW_ECHO_H_
#define RAW_ECHO_H_

#define IP_VERSION           4 /* IP 版本 */
#define IP_HDR_LEN          20 /* IP 首部长度 */
#define IP_DEFAULT_TTL      64 /* IP 生存时间 */
#define UDP_PSEUDO_OFFSET    8 /* UDP 伪头的偏移, 距离 IP 首部开始 */
#define UDP_PSEUDO_LEN      12 /* UDP 伪头的长度 */

#define ECHO_DEF_PORT        7 /* 连接的缺省端口	*/
#define ECHO_BUF_SIZE      256 /* 缓冲区的大小		*/
#define ECHO_SRC_PORT     5579 /* 源端口	*/

#define ECHO_DEF_DATA     "Hello World!"/* 发送的数据 */

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

/* UDP 首部 -- RFC 768 */
struct udp_hdr
{   
    unsigned short sport;       /* 源端口  */
    unsigned short dport;       /* 目的端口*/
    unsigned short length;      /* 标识符  */
    unsigned short checksum;    /* 校验和  */
};

#endif /* RAW_ECHO_H_  */
