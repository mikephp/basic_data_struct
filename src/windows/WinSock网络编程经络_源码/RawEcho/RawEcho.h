/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  RawEcho.h
 *
 * PURPOSE :  �������ݽṹ������.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef RAW_ECHO_H_
#define RAW_ECHO_H_

#define IP_VERSION           4 /* IP �汾 */
#define IP_HDR_LEN          20 /* IP �ײ����� */
#define IP_DEFAULT_TTL      64 /* IP ����ʱ�� */
#define UDP_PSEUDO_OFFSET    8 /* UDP αͷ��ƫ��, ���� IP �ײ���ʼ */
#define UDP_PSEUDO_LEN      12 /* UDP αͷ�ĳ��� */

#define ECHO_DEF_PORT        7 /* ���ӵ�ȱʡ�˿�	*/
#define ECHO_BUF_SIZE      256 /* �������Ĵ�С		*/
#define ECHO_SRC_PORT     5579 /* Դ�˿�	*/

#define ECHO_DEF_DATA     "Hello World!"/* ���͵����� */

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

/* UDP �ײ� -- RFC 768 */
struct udp_hdr
{   
    unsigned short sport;       /* Դ�˿�  */
    unsigned short dport;       /* Ŀ�Ķ˿�*/
    unsigned short length;      /* ��ʶ��  */
    unsigned short checksum;    /* У���  */
};

#endif /* RAW_ECHO_H_  */
