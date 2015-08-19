/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  TraceRoute.c
 *
 * PURPOSE :  Trace Route ����.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "TraceRoute.h"

#pragma comment(lib, "ws2_32.lib")    /* WinSockʹ�õĿ⺯�� */

struct trace_control trace_ctrl_g = { TRACE_MAX_HOP, TRACE_DEF_TIMEOUT };

unsigned short ip_checksum(unsigned short *buf, int buf_len);

/**************************************************************************
 *
 * ��������: ��ʾ������Ϣ.
 *
 * ����˵��: [IN] prog_name, ������;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void trace_help(char *prog_name)
{
    char *file_name;

    file_name = strrchr(prog_name, '\\');
    if (file_name != NULL)
        file_name++;
    else
        file_name = prog_name;

    printf(" Usage:    %s host name [-h hops] [-w timeout] [-u]\n", file_name);
    printf("\n Option:\n");
    printf("    -h     Maximum hops to search for target.\n");
    printf("    -w     Timeout milliseconds for each reply.\n");
    printf("    -u     Use UDP to send probe packet.\n");
    printf("\n Example:  %s www.sina.com -h 10\n\n", file_name);
    exit(1);
}

/**************************************************************************
 *
 * ��������: ����������ѡ��, ���浽ȫ�ֱ�����.
 *
 * ����˵��: [IN] argc, �����ĸ���;
 *           [IN] argv, �ַ���ָ������.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void trace_parse_param(int argc, char **argv)
{
    int i;
    
    if (argc < 2)
        trace_help(argv[0]);

    for(i = 1; i < argc; i++)
    {
        if ((argv[i][0] != '-') && (argv[i][0] != '/'))
        {
            if (trace_ctrl_g.host)
                trace_help(argv[0]);
            else
            {
                trace_ctrl_g.host = argv[i];
                continue;
            }
        }

        switch (tolower(argv[i][1]))
        {
        case 'h':   /* ������� */
            i++;
            trace_ctrl_g.max_hop = atoi(argv[i]);
            break;
        case 'w':   /* �ȴ��𸴵ĳ�ʱʱ��: ms */
            i++;
            trace_ctrl_g.timeout = atoi(argv[i]);
            break;
        case 'u':   /* �Ƿ�ʹ�� UDP ̽�� */
            trace_ctrl_g.use_udp = 1;
            break;
        default:
            trace_help(argv[0]);
            break;
        }
    }

    return;
}

/**************************************************************************
 *
 * ��������: ������������ʮ���Ƶ�ַת��Ϊ�����ֽ����IP��ַ.
 *
 * ����˵��: [IN] host, �����ǵ��ʮ����, Ҳ����������;
 *
 * �� �� ֵ: �ɹ����������ֽ����IP��ַ, ʧ�ܷ��� INADDR_NONE.
 *
 **************************************************************************/
unsigned long trace_resolve_addr(char *host)
{
    unsigned long addr;
    struct hostent *host_ent = NULL;

    addr = inet_addr(host);
    if (addr != INADDR_NONE)
        return addr;

    /* ���������� */
    host_ent = gethostbyname(host);
    if (host_ent)
        memcpy(&addr, host_ent->h_addr_list[0], host_ent->h_length);

    return addr;
}

/**************************************************************************
 *
 * ��������: ���� ICMP ̽�����.
 *
 * ����˵��: [IN] sock, socket ������;
 *           [IN] target, Ŀ������;
 *           [IN] ttl, ����ʱ��;
 *           [IN] seq_no, ���к�.
 *
 * �� �� ֵ: �ɹ����ط��͵��ֽ���, ʧ�ܷ��� SOCKET_ERROR.
 *
 **************************************************************************/
int trace_icmp_send(SOCKET sock, struct sockaddr *target, int ttl, short seq_no)
{
    int  i, data_len = 0, result = 0, msg_len = TRACE_PKT_SIZE;
    char icmp_data[TRACE_PKT_SIZE], *remnant;
    struct icmp_hdr *icmp_hdr;

    /* ���� ICMP ���� */
    icmp_hdr = (struct icmp_hdr *)icmp_data;

    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->checksum= 0;
    icmp_hdr->id = (unsigned short)GetCurrentProcessId();
    icmp_hdr->seq = seq_no;

    remnant = icmp_data + sizeof(struct icmp_hdr);
    data_len= msg_len - sizeof(struct icmp_hdr);

    /* ������һЩ���� */
    for (i = 0; i < data_len; i++)
        remnant[i] = 'A' + i;

    icmp_hdr->checksum = ip_checksum((unsigned short *)icmp_data, msg_len);

    /* ��������ʱ�䲢�������� */
    result = setsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(int));
    if (result == SOCKET_ERROR)
    {
        printf("[trace:] setsockopt(IP_TTL) error\n");
        return result;
    }

    trace_ctrl_g.timestamp = GetTickCount();
    result = sendto(sock, icmp_data, msg_len, 0, target, sizeof(*target));
    
    return result;
}

/**************************************************************************
 *
 * ��������: ���� UDP ̽�����.
 *
 * ����˵��: [IN] sock, socket ������;
 *           [IN] target, Ŀ������;
 *           [IN] ttl, ����ʱ��;
 *           [IN] seq_no, ���к�.
 *
 * �� �� ֵ: �ɹ����ط��͵��ֽ���, ʧ�ܷ��� SOCKET_ERROR.
 *
 **************************************************************************/
int trace_udp_send(SOCKET sock, struct sockaddr *target, int ttl, short seq_no)
{
    int  i, result = 0;
    char data[TRACE_PKT_SIZE];
    struct sockaddr_in *dest_addr = (struct sockaddr_in *)target;

    /* ������һЩ���� */
    for (i = 0; i < TRACE_PKT_SIZE; i++)
        data[i] = 'A' + i;

    /* ��������ʱ�䲢�������� */
    result = setsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(int));
    if (result == SOCKET_ERROR)
    {
        printf("[trace:] setsockopt(IP_TTL) error\n");
        return result;
    }

    trace_ctrl_g.timestamp = GetTickCount();
    dest_addr->sin_port = htons((unsigned short)(TRACE_DEST_PORT + seq_no));
    result = sendto(sock, data, TRACE_PKT_SIZE, 0, target, sizeof(*target));

    return result;
}

/**************************************************************************
 *
 * ��������: �������յ���Ӧ������.
 *
 * ����˵��: [IN] buf, �������ݵĻ�����;
 *           [IN] buf_len, buf �ĳ���;
 *           [IN] seq_no, ���к�.
 *
 * �� �� ֵ: ���յ���ȷ�ķ�������ʱ����ֵС�� 0, �յ�����ȷ�ķ��鷵��
 *           ICMP ����.
 *
 **************************************************************************/
int trace_parse_reply(char *buf, int buf_len, short seq_no)
{
    struct ip_hdr *ip_hdr, *ip_hdr2;
    struct icmp_hdr *icmp_hdr, *icmp_hdr2;
    struct udp_hdr *udp_hdr;
    unsigned short iph_len, iph_len2;
    unsigned short icmp_len, min_len;
    unsigned short proc_id = (unsigned short)GetCurrentProcessId();
    unsigned short dport = htons((unsigned short)(TRACE_DEST_PORT + seq_no));
    char *proto_data;
    int icmp_type = 0;

    ip_hdr = (struct ip_hdr *)buf;
    iph_len = (ip_hdr->vers_len & 0x0F) << 2; /* IP ͷ�ĳ��� */

    if (buf_len < iph_len + ICMP_HDR_SIZE)
    {
        printf("[trace:] Too few bytes received\n");
        return TRACE_ERROR;
    }

    icmp_len = ntohs(ip_hdr->total_len) - iph_len;
    icmp_hdr = (struct icmp_hdr *)(buf + iph_len);
    icmp_type = icmp_hdr->type;

    /* �õ� ICMP ��������� */
    ip_hdr2 = (struct ip_hdr *)(buf + iph_len + ICMP_HDR_SIZE);
    iph_len2 = (ip_hdr2->vers_len & 0x0F) << 2; /* ICMP ��Ϣ�� IP �ײ����� */
    proto_data = buf + iph_len + ICMP_HDR_SIZE + iph_len2;
    
    icmp_hdr2 = (struct icmp_hdr *)proto_data;
    udp_hdr = (struct udp_hdr *)proto_data;

    min_len = ICMP_HDR_SIZE + iph_len2 + ICMP_HDR_SIZE;
    if (icmp_len < min_len)
    {
        printf("[trace:] ICMP has too few data: %d\n", icmp_len);
        return TRACE_ERROR;
    }

    switch (icmp_type)
    {
    case ICMP_ECHO_REPLY: /* ECHO �Ĵ�, ICMP ����Ŀ������ */
        if (icmp_hdr->id == proc_id) /* ��� ICMP �� ID */
            return TRACE_DONE;

        break;

    case ICMP_DEST_UNREACH:  /* Ŀ�Ĳ��ɴ�, UDP ����Ŀ������ */
        if (ip_hdr2->proto == IPPROTO_UDP &&
            udp_hdr->dport == dport &&
            udp_hdr->sport == trace_ctrl_g.sport)
        {
            if (icmp_hdr->code == ICMP_UNREACH_PORT)
                return TRACE_DONE;
        }

        break;

    case ICMP_TIME_EXCEEDED: /* ��·�������յ� ICMP ��ʱ��Ϣ */
        if (icmp_hdr->code != ICMP_EXCEED_TRANS)
            break;

        if (trace_ctrl_g.use_udp)
        {
            if (ip_hdr2->proto == IPPROTO_UDP &&
                udp_hdr->dport == dport &&
                udp_hdr->sport == trace_ctrl_g.sport)
            {
                return TRACE_EXCEEDED;
            }
        }
        else if (icmp_hdr2->id == proc_id)
            return TRACE_EXCEEDED;

        break;

    default:
        break;
    }

    return icmp_type;
}

/**************************************************************************
 *
 * ��������: �������ݲ��������յ���Ӧ������.
 *
 * ����˵��: [IN] icmp_sock, socket ������;
 *           [IN] seq_no, ���к�;
 *           [OUT] from, buf �ĳ���;
 *           [OUT] trip, ����ʱ��.
 *
 * �� �� ֵ: ���յ���ȷ�ķ�������ʱ����ֵС�� 0, �յ�����ȷ�ķ�������ȴ�.
 *
 **************************************************************************/
int trace_handle_reply(SOCKET icmp_sock, short seq_no, 
                       struct sockaddr_in *from, long *trip)
{
    int result;
    char recv_buf[TRACE_PKT_MAX];
    int from_len = sizeof(*from);

    while (1)
    {
        /* �������� */
        result = recvfrom(icmp_sock, recv_buf, TRACE_PKT_MAX, 0, 
                          (struct sockaddr *)from, &from_len);
        if (result == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT) 
            {
                return TRACE_TIMEOUT;
            }
        
            printf("[trace:] recvfrom() error: %d\n", WSAGetLastError());
            return TRACE_ERROR;
        }

        *trip = GetTickCount() - trace_ctrl_g.timestamp;

        /* ���� ICMP ��Ϣ */
        result = trace_parse_reply(recv_buf, result, seq_no);
        if (result < 0)
            return result;
    }

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsd_data;
    SOCKET  icmp_sock, udp_sock;
    int i, result, max_hop, ttl = 1;
    int done = 0, local_len = sizeof(struct sockaddr_in);
    
    short seq_no = 0;
    struct sockaddr_in  target, from, local;
    struct sockaddr *dst_addr = (struct sockaddr *) &target;
    long trip_tm = 0;

    trace_parse_param(argc, argv);
    WSAStartup(MAKEWORD(2, 0), &wsd_data);

    /* ����������IP��ַ */
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = trace_resolve_addr(trace_ctrl_g.host);
    if (target.sin_addr.s_addr == INADDR_NONE)
    {
        printf("[trace:] Unable to resolve target: %s\n", trace_ctrl_g.host);
        return -1;
    }    
    
    icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); /* ԭʼ�׽ӿ� */

    /* ���� UDP socket */
    if (trace_ctrl_g.use_udp)
    {
        /* �� ICMP socket �ı��ص�ַ */
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;

        bind(icmp_sock, (struct sockaddr *)&local, sizeof(local));

        /* �������� UDP socket */
        udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
        bind(udp_sock, (struct sockaddr *)&local, sizeof(local));
        getsockname(udp_sock, (struct sockaddr *)&local, &local_len);
        trace_ctrl_g.sport = local.sin_port;
    }

    /* ���ý��յ���ȴ�ʱ�� */
    result = setsockopt(icmp_sock, SOL_SOCKET, SO_RCVTIMEO, 
                        (char *)&trace_ctrl_g.timeout, sizeof(int));
    if (result == SOCKET_ERROR)
    {
        printf("[trace:] setsockopt error: %d\n", WSAGetLastError());
        return -1;
    }

    max_hop = trace_ctrl_g.max_hop;
    printf("\nTracing route to %s over a maximum of %d hops:\n", 
           trace_ctrl_g.host, max_hop);

    for (ttl = 1; ttl < max_hop && !done; ttl++)
    {
        printf("\n%2d", ttl);

        for (i = 0; i < TRACE_MAX_TIMES; i++)
        {
            /* �������� */
            if (trace_ctrl_g.use_udp)
                result = trace_udp_send(udp_sock, dst_addr, ttl, ++seq_no);
            else
                result = trace_icmp_send(icmp_sock, dst_addr, ttl, ++seq_no);
            
            if (result == SOCKET_ERROR)
            {
                printf("\n[trace:] trace send error %d", WSAGetLastError());
                goto end;
            }

            /* ���ղ��������� */
            result = trace_handle_reply(icmp_sock, seq_no, &from, &trip_tm);
            switch (result) 
            {
            case TRACE_TIMEOUT:
                printf("     *   ");
                break;
            case TRACE_EXCEEDED:  /* throughout */
            case TRACE_DONE:
                printf("%6d ms", trip_tm);
                result == TRACE_DONE ? done++ : done;
                
                if (i == (TRACE_MAX_TIMES - 1)) /* ���һ��, �����ַ */
                     printf("    %s", inet_ntoa(from.sin_addr));

                break;
            case TRACE_ERROR:
                goto end;
                break;
            default:
                printf("ICMP code %d", result);
                break;
            }
            Sleep(1000);
        }
    }

end:
    printf("\n\nTrace complete.\n");
    closesocket(icmp_sock);
    if (trace_ctrl_g.use_udp)
        closesocket(udp_sock);
    
    WSACleanup();

    return 0;
}

/**************************************************************************
 *
 * ��������: ����У���.
 *
 * ����˵��: [IN] buf, ���ݻ�����;
 *           [IN] buf_len, buf �ĳ���.
 *
 * �� �� ֵ: У���.
 *
 **************************************************************************/
unsigned short ip_checksum(unsigned short *buf, int buf_len)
{
    unsigned long checksum = 0;

    while (buf_len > 1)
    {
        checksum += *buf++;
        buf_len -= sizeof(unsigned short);
    }

    if (buf_len)
    {
        checksum += *(unsigned char *)buf;
    }
    
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);

    return (unsigned short)(~checksum);
}
