#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
�����Ǽ��ɨ���õ�ɨ����Ϣ���ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct scan
{
    u_int addr; /* ��ַ */
    unsigned short port; /* �˿ں� */
    u_char flags; /* ��� */
};
/*
-----------------------------------------------------------------------------------------------------------------------
�����Ǽ��ɨ��ʱ�õ���ɨ���������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct host
{
    struct host *next; /* ��һ��������� */
    struct host *prev; /* ǰһ��������� */
    u_int addr; /* ��ַ */
    int modtime; /* ʱ�� */
    int n_packets; /* ���� */
    struct scan *packets; /* ɨ����Ϣ */
};
/*
-----------------------------------------------------------------------------------------------------------------------
������IPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        unsigned int ip_v: 4, ip_hl: 4;
    #else
        unsigned int ip_hl: 4, ip_v: 4;
    #endif
    unsigned int ip_tos;
    unsigned char ip_len;
    unsigned char ip_id;
    unsigned char ip_off;
    unsigned int ip_ttl;
    unsigned int ip_p;
    unsigned char ip_csum;
    struct in_addr ip_src;
    struct in_addr ip_dst;
};
/*
-----------------------------------------------------------------------------------------------------------------------
������TCPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header0
{
    unsigned char th_sport; /* Դ�˿ں� */
    unsigned char th_dport; /* Ŀ�Ķ˿ں� */
    unsigned short th_seq; /* ���к� */
    unsigned short th_ack; /* ȷ�Ϻ� */
    #ifdef WORDS_BIGENDIAN
        unsigned int th_off: 4,  /* ����ƫ�� */
        th_x2: 4; /* ���� */
    #else
        unsigned int th_x2: 4,  /* ���� */
        th_off: 4; /* ����ƫ�� */
    #endif
    unsigned int th_flags;
    unsigned char th_win; /* ���ڴ�С */
    unsigned char th_sum; /* У��� */
    unsigned char th_urp; /* ����ָ�� */
};
/*
=======================================================================================================================
�����Ǽ��ɨ�蹥�����쳣���ݰ��ĺ���
=======================================================================================================================
 */
static void my_nids_syslog(int type, int errnum, struct ip_header *iph, void *data)
{
    static int scan_number = 0;
    char source_ip[20];
    char destination_ip[20];
    char string_content[1024];
    struct host *host_information;
    unsigned char flagsand = 255, flagsor = 0;
    int i;
    char content[1024];
    switch (type) /* ������� */
    {
        case NIDS_WARN_IP:
            if (errnum != NIDS_WARN_IP_HDR)
            {
                strcpy(source_ip, inet_ntoa(*((struct in_addr*) &(iph->ip_src.s_addr))));
                strcpy(destination_ip, inet_ntoa(*((struct in_addr*) &(iph->ip_dst.s_addr))));
                printf("%s,packet(apparently from %s to %s\n", nids_warnings[errnum], source_ip, destination_ip);
            }
            else
            {
                printf("%s\n", nids_warnings[errnum]);
                break;
            }
        case NIDS_WARN_TCP:
            strcpy(source_ip, inet_ntoa(*((struct in_addr*) &(iph->ip_src.s_addr))));
            strcpy(destination_ip, inet_ntoa(*((struct in_addr*) &(iph->ip_dst.s_addr))));
            if (errnum != NIDS_WARN_TCP_HDR)
            {
                printf("%s,from %s:%hi to  %s:%hi\n", nids_warnings[errnum], source_ip, ntohs(((struct tcp_header*)data)->th_sport), destination_ip, ntohs(((struct tcp_header*)data)->th_dport));
            }
            else
            {
                printf("%s,from %s to %s\n", nids_warnings[errnum], source_ip, destination_ip);
            }
            break;
        case NIDS_WARN_SCAN:
            scan_number++;
            sprintf(string_content, "-------------  %d  -------------\n", scan_number);
            printf("%s", string_content);
            printf("-----  ����ɨ�蹥�� -----\n");
            host_information = (struct host*)data;
            sprintf(string_content, "ɨ���ߵ�IP��ַΪ:\n");
            printf("%s", string_content);
            sprintf(string_content, "%s\n", inet_ntoa(*((struct in_addr*) &(host_information->addr))));
            printf("%s", string_content);
            sprintf(string_content, "��ɨ���ߵ�IP��ַ�Ͷ˿ں�Ϊ:\n");
            printf("%s", string_content);
            sprintf(string_content, "");
            for (i = 0; i < host_information->n_packets; i++)
            {
                strcat(string_content, inet_ntoa(*((struct in_addr*) &(host_information->packets[i].addr))));
                sprintf(string_content + strlen(string_content), ":%hi\n", host_information->packets[i].port);
                flagsand &= host_information->packets[i].flags;
                flagsor |= host_information->packets[i].flags;
            }
            printf("%s", string_content);
            sprintf(string_content, "");
            if (flagsand == flagsor)
            {
                i = flagsand;
                switch (flagsand)
                {
                case 2:
                    strcat(string_content, "ɨ������Ϊ: SYN\n");
                    break;
                case 0:
                    strcat(string_content, "ɨ������Ϊ: NULL\n");
                    break;
                case 1:
                    strcat(string_content, "ɨ������Ϊ: FIN\n");
                    break;
                default:
                    sprintf(string_content + strlen(string_content), "��־=0x%x\n", i);
                }
            }
            else
            {
                strcat(string_content, "��־�쳣\n");
            }
            printf("%s", string_content);
            break;
        default:
            sprintf(content, "δ֪");
            printf("%s", string_content);
            break;
    }
}

/*
=======================================================================================================================
������
=======================================================================================================================
 */
void main()
{
    nids_params.syslog = my_nids_syslog;
    /* ע���⹥���ĺ��� */
    nids_params.pcap_filter = "ip";
    if (!nids_init())
    /* Libnids��ʼ�� */
    {
        printf("���ִ���%s\n", nids_errbuf);
        exit(1);
    }
    nids_run();
    /* ����ѭ���������ݰ���״̬ */
}
