#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
下面是UDP协议首部的数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header
{
    unsigned short udp_source_port; /* 源端口号 */
    unsigned short udp_destination_port; /* 目的端口号 */
    unsigned short udp_length; /* 长度 */
    unsigned short udp_checksum; /* 校验和 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是IP协议首部的数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        unsigned char ip_version: 4,  /* 版本 */
        ip_header_length: 4; /* 首部长度 */
    #else
        unsigned char ip_header_length: 4, ip_version: 4;
    #endif
    unsigned char ip_tos; /* 服务类型 */
    unsigned short ip_length; /* 总长度 */
    unsigned short ip_id; /* 标识 */
    unsigned short ip_off; /* 标志和偏移 */
    unsigned char ip_ttl; /* 生存时间  */
    unsigned char ip_protocol; /*协议 */
    unsigned short ip_checksum; /* 校验和 */
    struct in_addr ip_souce_address; /* 源IP地址 */
    struct in_addr ip_destination_address; /* 目的IP地址 */
};
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
    {
        *string++ = ch;
    }
    else if (ch == ' ')
    {
        *string++ = ch;
    }
    else if (ch == '\n' || ch == '\r')
    {
        *string++ = ch;
    }
    else
    {
        *string++ = '.';
    }
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
下面是分析UDP协议的函数的定义
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *packet_content)
{
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destination_port;
    u_short length;
    udp_protocol = (struct udp_header*)(packet_content + 20);
    /* 获取UPD协议数据 */
    source_port = ntohs(udp_protocol->udp_source_port);
    /* 获取源端口 */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* 获取目的端口 */
    length = ntohs(udp_protocol->udp_length);
    /* 获取长度 */
    printf("----------  UDP协议首部    ----------\n");
    printf("源端口:%d\n", source_port);
    printf("目的端口:%d\n", destination_port);
    switch (destination_port) /* 判断端口号，确定上层协议类型 */
    {
        case 138:
            printf("NETBIOS Datagram Service\n");
            break;
        case 137:
            printf("NETBIOS Name Service\n");
            break;
        case 139:
            printf("NETBIOS session service\n");
            break;
        case 53:
            printf("name-domain server \n");
            break;
        default:
            break;
    }
    printf("长度:%d\n", length);
    printf("校验和:%d\n", ntohs(udp_protocol->udp_checksum));
    /* 获取校验和 */
}
/*
=======================================================================================================================
下面是实现IP协议分析的函数的定义
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *packet_content)
{
    struct ip_header *ip_protocol;
    u_int header_length;
    u_int offset;
    u_char tos;
    unsigned short checksum;
    printf("----------  IP协议首部  ----------\n");
    ip_protocol = (struct ip_header*)(packet_content);
    /* 获取IP协议数据 */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获取校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获取长度 */
    tos = ip_protocol->ip_tos;
    /* 获取服务类型 */
    offset = ntohs(ip_protocol->ip_off);
    /* 获取标志和偏移 */
    printf("IP版本:%d\n", ip_protocol->ip_version);
    printf("首部长度:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("总长度:%d\n", ntohs(ip_protocol->ip_length));
    printf("标识:%d\n", ntohs(ip_protocol->ip_id));
    printf("偏移:%d\n", (offset &0x1fff) *8);
    printf("生存时间:%d\n", ip_protocol->ip_ttl);
    printf("协议:%d\n", ip_protocol->ip_protocol);
    switch (ip_protocol->ip_protocol) /* 判断上层协议类型 */
    {
        case 6:
            printf("上层协议为TCP\n");
            break;
        case 17:
            printf("上层协议为UDP\n");
            break;
        case 1:
            printf("上层协议为ICMP\n");
            break;
        default:
            break;
    }
    printf("校验和:%d\n", checksum);
    printf("源IP地址:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* 获取源IP地址 */
    printf("目的IP地址:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* 获取目的IP地址 */
    switch (ip_protocol->ip_protocol)
    {
        case 17:
            udp_protocol_packet_callback(packet_content);
            break;
            /* 调用分析UDP协议的函数 */
        default:
            break;
    }
}
/*
=======================================================================================================================
下面是回调函数，此回调函数将在函数nids_register_udp（）中被注册
=======================================================================================================================
 */
void udp_callback(struct tuple4 *addr, char *buf, int len, struct ip *iph)
{
    int i;
    char content[65535];
    char content_urgent[65535];
    char tcp_content[65535];
    char buffer[1024];
    strcpy(buffer, inet_ntoa(*((struct in_addr*) &(addr->saddr))));
    sprintf(buffer + strlen(buffer), " : %i", addr->source);
    strcat(buffer, " -> ");
    strcat(buffer, inet_ntoa(*((struct in_addr*) &(addr->daddr))));
    sprintf(buffer + strlen(buffer), " : %i", addr->dest);
    strcat(buffer, "\n");
    printf("------------------- BEGIN ---------------------------\n");
    printf("%s", buffer);
    ip_protocol_packet_callback(iph);
    /* 调用分析IP协议的函数 */
    printf("----------  UDP数据包内容  ----------");
    /* 输出UDP负载数据内容 */
    for (i = 0; i < len; i++)
    {
        if (i % 50 == 0)
        {
            printf("\n");
        }
        printf("%s", char_to_ascii(buf[i]));
    }
    printf("\n");
    printf("------------------- END ---------------------------\n");
    printf("\n");
    return ;
}
/*
=======================================================================================================================
主函数
=======================================================================================================================
 */
void main()
{
    if (!nids_init())
     /* Libnids初始化 */
    {
        printf("%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_udp(udp_callback); /* 注册回调函数 */
    nids_run(); /* 进入循环捕获数据包状态 */
}
