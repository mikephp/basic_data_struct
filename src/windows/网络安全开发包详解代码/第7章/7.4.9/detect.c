#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
下面是检测扫描用的扫描信息数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct scan
{
    u_int addr; /* 地址 */
    unsigned short port; /* 端口号 */
    u_char flags; /* 标记 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是检测扫描时用到的扫描主机数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct host
{
    struct host *next; /* 下一个主机结点 */
    struct host *prev; /* 前一个主机结点 */
    u_int addr; /* 地址 */
    int modtime; /* 时间 */
    int n_packets; /* 个数 */
    struct scan *packets; /* 扫描信息 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是IP协议首部的数据结构
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
下面是TCP协议首部的数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header0
{
    unsigned char th_sport; /* 源端口号 */
    unsigned char th_dport; /* 目的端口号 */
    unsigned short th_seq; /* 序列号 */
    unsigned short th_ack; /* 确认号 */
    #ifdef WORDS_BIGENDIAN
        unsigned int th_off: 4,  /* 数据偏移 */
        th_x2: 4; /* 保留 */
    #else
        unsigned int th_x2: 4,  /* 保留 */
        th_off: 4; /* 数据偏移 */
    #endif
    unsigned int th_flags;
    unsigned char th_win; /* 窗口大小 */
    unsigned char th_sum; /* 校验和 */
    unsigned char th_urp; /* 紧急指针 */
};
/*
=======================================================================================================================
下面是检测扫描攻击和异常数据包的函数
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
    switch (type) /* 检测类型 */
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
            printf("-----  发现扫描攻击 -----\n");
            host_information = (struct host*)data;
            sprintf(string_content, "扫描者的IP地址为:\n");
            printf("%s", string_content);
            sprintf(string_content, "%s\n", inet_ntoa(*((struct in_addr*) &(host_information->addr))));
            printf("%s", string_content);
            sprintf(string_content, "被扫描者的IP地址和端口号为:\n");
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
                    strcat(string_content, "扫描类型为: SYN\n");
                    break;
                case 0:
                    strcat(string_content, "扫描类型为: NULL\n");
                    break;
                case 1:
                    strcat(string_content, "扫描类型为: FIN\n");
                    break;
                default:
                    sprintf(string_content + strlen(string_content), "标志=0x%x\n", i);
                }
            }
            else
            {
                strcat(string_content, "标志异常\n");
            }
            printf("%s", string_content);
            break;
        default:
            sprintf(content, "未知");
            printf("%s", string_content);
            break;
    }
}

/*
=======================================================================================================================
主函数
=======================================================================================================================
 */
void main()
{
    nids_params.syslog = my_nids_syslog;
    /* 注册检测攻击的函数 */
    nids_params.pcap_filter = "ip";
    if (!nids_init())
    /* Libnids初始化 */
    {
        printf("出现错误：%s\n", nids_errbuf);
        exit(1);
    }
    nids_run();
    /* 进入循环捕获数据包的状态 */
}
