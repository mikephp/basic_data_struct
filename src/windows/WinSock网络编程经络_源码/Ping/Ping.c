/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  Ping.c
 *
 * PURPOSE :  Ping ����.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")    /* WinSockʹ�õĿ⺯�� */

/* ICMP ���� */
#define ICMP_TYPE_ECHO          8
#define ICMP_TYPE_ECHO_REPLY    0

#define ICMP_MIN_LEN            8  /* ICMP ��С����, ֻ���ײ� */
#define ICMP_DEF_COUNT          4  /* ȱʡ���ݴ��� */
#define ICMP_DEF_SIZE          32  /* ȱʡ���ݳ��� */
#define ICMP_DEF_TIMEOUT     1000  /* ȱʡ��ʱʱ��, ���� */
#define ICMP_MAX_SIZE       65500  /* ������ݳ��� */

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
    
    /* ��֮��Ĳ��Ǳ�׼ ICMP �ײ�, ���ڼ�¼ʱ�� */
    unsigned long timestamp;
};

struct icmp_user_opt
{
    unsigned int  persist;  /* һֱ Ping            */
    unsigned int  count;    /* ���� ECHO ��������� */
    unsigned int  size;     /* �������ݵĴ�С       */
    unsigned int  timeout;  /* �ȴ��𸴵ĳ�ʱʱ��   */
    char          *host;    /* ������ַ     */
    unsigned int  send;     /* ��������     */
    unsigned int  recv;     /* ��������     */
    unsigned int  min_t;    /* ���ʱ��     */
    unsigned int  max_t;    /* �ʱ��     */
    unsigned int  total_t;  /* �ܵ��ۼ�ʱ�� */
};

/* ������� */
const char icmp_rand_data[] = "abcdefghigklmnopqrstuvwxyz0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct icmp_user_opt user_opt_g = { 
      0, ICMP_DEF_COUNT, ICMP_DEF_SIZE, ICMP_DEF_TIMEOUT, NULL,
      0, 0, 0xFFFF, 0
    };

unsigned short ip_checksum(unsigned short *buf, int buf_len);


/**************************************************************************
 *
 * ��������: ���� ICMP ����.
 *
 * ����˵��: [IN, OUT] icmp_data, ICMP ������;
 *           [IN] data_size, icmp_data �ĳ���;
 *           [IN] sequence, ���к�.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void icmp_make_data(char *icmp_data, int data_size, int sequence)
{   
    struct icmp_hdr *icmp_hdr;
    char *data_buf;
    int data_len;
    int fill_count = sizeof(icmp_rand_data) / sizeof(icmp_rand_data[0]);

    /* ��д ICMP ���� */
    data_buf = icmp_data + sizeof(struct icmp_hdr);
    data_len = data_size - sizeof(struct icmp_hdr);
    
    while (data_len > fill_count)
    {
        memcpy(data_buf, icmp_rand_data, fill_count);
        data_len -= fill_count;
    }

    if (data_len > 0)
        memcpy(data_buf, icmp_rand_data, data_len);

    /* ��д ICMP �ײ� */
    icmp_hdr = (struct icmp_hdr *)icmp_data;

    icmp_hdr->type = ICMP_TYPE_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->id = (unsigned short)GetCurrentProcessId();
    icmp_hdr->checksum = 0;
    icmp_hdr->seq = sequence;
    icmp_hdr->timestamp = GetTickCount();

    icmp_hdr->checksum = ip_checksum((unsigned short*)icmp_data, data_size);
}

/**************************************************************************
 *
 * ��������: �������յ�������.
 *
 * ����˵��: [IN] buf, ���ݻ�����;
 *           [IN] buf_len, buf �ĳ���;
 *           [IN] from, �Է��ĵ�ַ.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
int icmp_parse_reply(char *buf, int buf_len,struct sockaddr_in *from)
{
    struct ip_hdr *ip_hdr;
    struct icmp_hdr *icmp_hdr;
    unsigned short hdr_len;
    int icmp_len;
    unsigned long trip_t;

    ip_hdr = (struct ip_hdr *)buf;
    hdr_len = (ip_hdr->vers_len & 0xf) << 2 ; /* IP �ײ����� */

    if (buf_len < hdr_len + ICMP_MIN_LEN)
    {    
        printf("[Ping] Too few bytes from %s\n", inet_ntoa(from->sin_addr));
        return -1;
    }

    icmp_hdr = (struct icmp_hdr *)(buf + hdr_len);
    icmp_len = ntohs(ip_hdr->total_len) - hdr_len;

    /* ���У��� */
    if (ip_checksum((unsigned short *)icmp_hdr, icmp_len))
    {
        printf("[Ping] icmp checksum error!\n");
        return -1;
    }

    /* ��� ICMP ���� */
    if (icmp_hdr->type != ICMP_TYPE_ECHO_REPLY)
    {    
        printf("[Ping] not echo reply : %d\n", icmp_hdr->type);
        return -1;
    }

    /* ��� ICMP �� ID */
    if (icmp_hdr->id != (unsigned short)GetCurrentProcessId())
    {    
        printf("[Ping] someone else's message!\n");
        return -1;    
    } 

    /* �����Ӧ��Ϣ */
    trip_t = GetTickCount() - icmp_hdr->timestamp;
    buf_len = ntohs(ip_hdr->total_len) - hdr_len - ICMP_MIN_LEN;
    printf("%d bytes from %s:", buf_len, inet_ntoa(from->sin_addr));
    printf(" icmp_seq = %d  time: %d ms\n",icmp_hdr->seq, trip_t);

    user_opt_g.recv++;
    user_opt_g.total_t += trip_t;

    /* ��¼����ʱ�� */
    if (user_opt_g.min_t > trip_t)
        user_opt_g.min_t = trip_t;

    if (user_opt_g.max_t < trip_t)
        user_opt_g.max_t = trip_t;

    return 0;
}

/**************************************************************************
 *
 * ��������: ��������, ����Ӧ��.
 *
 * ����˵��: [IN] icmp_soc, �׽ӿ�������.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
 *
 **************************************************************************/
int icmp_process_reply(SOCKET icmp_soc)
{
    struct sockaddr_in from_addr;
    int result, data_size = user_opt_g.size;
    int from_len = sizeof(from_addr);
    char *recv_buf;

    data_size += sizeof(struct ip_hdr) + sizeof(struct icmp_hdr);
    recv_buf = malloc(data_size);

    /* �������� */
    result = recvfrom(icmp_soc, recv_buf, data_size, 0,
                      (struct sockaddr*)&from_addr, &from_len);
    if (result == SOCKET_ERROR)
    {    
        if (WSAGetLastError() == WSAETIMEDOUT)
            printf("timed out\n");
        else
            printf("[PING] recvfrom_ failed: %d\n", WSAGetLastError());
        
        return -1;
    } 

    result = icmp_parse_reply(recv_buf, result, &from_addr);
    free(recv_buf);

    return result;
}

/**************************************************************************
 *
 * ��������: ��ʾ ECHO �İ�����Ϣ.
 *
 * ����˵��: [IN] prog_name, ������;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void icmp_help(char *prog_name)
{
    char *file_name;

    file_name = strrchr(prog_name, '\\');
    if (file_name != NULL)
        file_name++;
    else
        file_name = prog_name;

    /* ��ʾ������Ϣ */
    printf(" usage:     %s host_address [-t] [-n count] [-l size] "
           "[-w timeout]\n", file_name);
    printf(" -t         Ping the host until stopped.\n");
    printf(" -n count   the count to send ECHO\n");
    printf(" -l size    the size to send data\n");
    printf(" -w timeout timeout to wait the reply\n");
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
void icmp_parse_param(int argc, char **argv)
{
    int i;

    for(i = 1; i < argc; i++)
    {
        if ((argv[i][0] != '-') && (argv[i][0] != '/'))
        {   
            /* ���������� */
            if (user_opt_g.host)
                icmp_help(argv[0]);
            else
            {
                user_opt_g.host = argv[i];
                continue;
            }
        }

        switch (tolower(argv[i][1]))
        {
        case 't':   /* ���� Ping */
            user_opt_g.persist = 1;
            break;

        case 'n':   /* ������������� */
            i++;    
            user_opt_g.count = atoi(argv[i]);
            break;

        case 'l':   /* �������ݵĴ�С */
            i++;    
            user_opt_g.size = atoi(argv[i]);
            if (user_opt_g.size > ICMP_MAX_SIZE)
                user_opt_g.size = ICMP_MAX_SIZE;
            break;

        case 'w':   /* �ȴ����յĳ�ʱʱ�� */
            i++;    
            user_opt_g.timeout = atoi(argv[i]);
            break;

        default:
            icmp_help(argv[0]);
            break;
        }
    }
}


int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET icmp_soc;
    struct sockaddr_in dest_addr;
    struct hostent *host_ent = NULL;
    
    int result, data_size, send_len;
    unsigned int i, timeout, lost;
    char *icmp_data;
    unsigned int ip_addr = 0;
    unsigned short seq_no = 0;

    if (argc < 2) 
        icmp_help(argv[0]);

    icmp_parse_param(argc, argv);
    WSAStartup(MAKEWORD(2,0),&wsaData);

    /* ����������ַ */
    ip_addr = inet_addr(user_opt_g.host);
    if (ip_addr == INADDR_NONE)
    {
        host_ent = gethostbyname(user_opt_g.host);
        if (!host_ent)
        {
            printf("[PING] Fail to resolve %s\n", user_opt_g.host);
            return -1;
        }
        
        memcpy(&ip_addr, host_ent->h_addr_list[0], host_ent->h_length);
    }

    icmp_soc = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (icmp_soc == INVALID_SOCKET)
    {    
        printf("[PING] socket() failed: %d\n", WSAGetLastError());
        return -1;
    }

    /* ����ѡ��, ���պͷ��͵ĳ�ʱʱ�䡡*/
    timeout = user_opt_g.timeout;
    result = setsockopt(icmp_soc, SOL_SOCKET, SO_RCVTIMEO,
                        (char*)&timeout, sizeof(timeout));

    timeout = 1000;
    result = setsockopt(icmp_soc, SOL_SOCKET, SO_SNDTIMEO,
                        (char*)&timeout, sizeof(timeout));

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = ip_addr;

    data_size = user_opt_g.size + sizeof(struct icmp_hdr) - sizeof(long);
    icmp_data = malloc(data_size);

    if (host_ent)
        printf("Ping %s [%s] with %d bytes data\n", user_opt_g.host,
            inet_ntoa(dest_addr.sin_addr), user_opt_g.size);
    else
        printf("Ping [%s] with %d bytes data\n", inet_ntoa(dest_addr.sin_addr),
            user_opt_g.size);

    /* �������󲢽�����Ӧ */
    for (i = 0; i < user_opt_g.count; i++)
    {
        icmp_make_data(icmp_data, data_size, seq_no++);
      
        send_len = sendto(icmp_soc, icmp_data, data_size, 0, 
                          (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (send_len == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                printf("[PING] sendto is timeout\n");
                continue;
            }

            printf("[PING] sendto failed: %d\n", WSAGetLastError());
            break;
        } 

        user_opt_g.send++;
        result = icmp_process_reply(icmp_soc);

        user_opt_g.persist ? i-- : i; /* ���� Ping */
        Sleep(1000); /* �ӳ� 1 �� */
    }    

    lost = user_opt_g.send - user_opt_g.recv;

    /* ��ӡͳ������ */
    printf("\nStatistic :\n");
    printf("    Packet : sent = %d, recv = %d, lost = %d (%3.f%% lost)\n", 
      user_opt_g.send, user_opt_g.recv, lost, (float)lost*100/user_opt_g.send);

    if (user_opt_g.recv > 0)
    {
        printf("Roundtrip time (ms)\n");
        printf("    min = %d ms, max = %d ms, avg = %d ms\n", user_opt_g.min_t,
            user_opt_g.max_t, user_opt_g.total_t / user_opt_g.recv);
    }

    free(icmp_data);
    closesocket(icmp_soc);
    WSACleanup();

    return 0;
}
  
/**************************************************************************
 *
 * ��������: ����У���.
 *
 * ����˵��: [IN] buf, ���ݻ�����;
 *           [IN] buf_len, buf ���ֽڳ���.
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
