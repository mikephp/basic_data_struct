#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
UDP协议首部的数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header
{
    unsigned short udp_source_port;
    unsigned short udp_destination_port;
    unsigned short udp_length;
    unsigned short udp_checksum;
};
/*
-----------------------------------------------------------------------------------------------------------------------
ICMP协议首部的数据结构
-----------------------------------------------------------------------------------------------------------------------
 */
struct icmp_header
{
    unsigned int icmp_type;
    unsigned int icmp_code;
    unsigned char icmp_checksum;
    unsigned char icmp_id;
    unsigned char icmp_sequence;
};
/*
-----------------------------------------------------------------------------------------------------------------------
IP协议首部的数据结构
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
    unsigned char ip_ttl; /* 生存时间 */
    unsigned char ip_protocol; /* 协议类型 */
    unsigned short ip_checksum; /* 校验和 */
    struct in_addr ip_souce_address; /* 源IP地址 */
    struct in_addr ip_destination_address; /* 目的IP地址 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
TCP协议首部
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
    unsigned char tcp_source_port; /* 源端口号 */
    unsigned char tcp_destination_port; /* 目的端口号 */
    unsigned short tcp_sequence; /* 学列码 */
    unsigned short tcp_acknowledgement; /* 确认号 */
    #ifdef WORDS_BIGENDIAN
        unsigned int tcp_offset: 4,  /* 数据偏移 */
        tcp_reserved: 4; /* 保留 */
    #else
        unsigned int tcp_reserved: 4,  /* 保留 */
        tcp_offset: 4; /* 数据偏移 */
    #endif
    unsigned int tcp_flags; /* 标志 */
    unsigned char tcp_windows; /* 窗口大小 */
    unsigned char tcp_checksum; /* 校验和 */
    unsigned char tcp_urgent_pointer; /* 紧急指针 */
};
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
        *string++ = ch;
    else if (ch == ' ')
        *string++ = ch;
    else if (ch == '\n' || ch == '\r')
        *string++ = ch;
    else
        *string++ = '.';
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
下面是分析ICMP协议的函数
=======================================================================================================================
 */
void icmp_protocol_packet_callback(const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    printf("----------  ICMP协议    ----------\n");
    printf("ICMP类型:%d\n", icmp_protocol->icmp_type);
    switch (icmp_protocol->icmp_type) /* ICMP类型 */
    {
        case 8:
            printf("ICMP回显请求协议\n");
            printf("ICMP代码:%d\n", icmp_protocol->icmp_code);
            printf("标识符:%d\n", icmp_protocol->icmp_id);
            printf("序列码:%d\n", icmp_protocol->icmp_sequence);
            break;
        case 0:
            printf("ICMP回显应答协议\n");
            printf("ICMP代码:%d\n", icmp_protocol->icmp_code);
            printf("标识符:%d\n", icmp_protocol->icmp_id);
            printf("序列码:%d\n", icmp_protocol->icmp_sequence);
            break;
        default:
            break;
    }
    printf("ICMP校验和:%d\n", ntohs(icmp_protocol->icmp_checksum)); /* 获取校验和 */
    return ;
}
/*
=======================================================================================================================
下面是分析TCP协议的函数
=======================================================================================================================
 */
void tcp_protocol_packet_callback(const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    u_char flags;
    int header_length;
    u_short source_port;
    u_short destination_port;
    u_short windows;
    u_short urgent_pointer;
    u_int sequence;
    u_int acknowledgement;
    unsigned char checksum;
    tcp_protocol = (struct tcp_header*)(packet_content + 14+20);
    source_port = ntohs(tcp_protocol->tcp_source_port);
    /* 获取源端口号 */
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    /* 获取目的端口号 */
    header_length = tcp_protocol->tcp_offset *4;
    /* 获取首部长度 */
    sequence = ntohl(tcp_protocol->tcp_sequence);
    /* 获取序列码 */
    acknowledgement = ntohl(tcp_protocol->tcp_acknowledgement);
    /* 获取确认号 */
    windows = ntohs(tcp_protocol->tcp_windows);
    /* 获取窗口大小 */
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    /* 获取紧急指针 */
    flags = tcp_protocol->tcp_flags;
    checksum = ntohs(tcp_protocol->tcp_checksum);
    printf("-------  TCP协议   -------\n");
    printf("源端口号:%d\n", source_port);
    printf("目的端口号:%d\n", destination_port);
    switch (destination_port)
    {
        case 80:
            printf("上层协议为HTTP协议\n");
            break;
        case 21:
            printf("上层协议为FTP协议\n");
            break;
        case 23:
            printf("上层协议为TELNET协议\n");
            break;
        case 25:
            printf("上层协议为SMTP协议\n");
            break;
        case 110:
            printf("上层协议POP3协议\n");
            break;
        default:
            break;
    }
    printf("序列码:%u\n", sequence);
    printf("确认号:%u\n", acknowledgement);
    printf("首部长度:%d\n", header_length);
    printf("保留:%d\n", tcp_protocol->tcp_reserved);
    printf("标记:");
    if (flags &0x08)
        printf("PSH ");
    if (flags &0x10)
        printf("ACK ");
    if (flags &0x02)
        printf("SYN ");
    if (flags &0x20)
        printf("URG ");
    if (flags &0x01)
        printf("FIN ");
    if (flags &0x04)
        printf("RST ");
    printf("\n");
    printf("窗口大小:%d\n", windows);
    printf("校验和:%d\n", checksum);
    printf("紧急指针:%d\n", urgent_pointer);
}
/*
=======================================================================================================================
下面是分析UPD协议的函数
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *packet_content)
{
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destination_port;
    u_short length;
    udp_protocol = (struct udp_header*)(packet_content + 20);
    source_port = ntohs(udp_protocol->udp_source_port);
    /* 获取源端口号 */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* 获取目的端口号 */
    length = ntohs(udp_protocol->udp_length);
    printf("----------  UDP协议首部    ----------\n");
    printf("源端口:%d\n", source_port);
    printf("目的端口:%d\n", destination_port);
    switch (destination_port)
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
}
/*
=======================================================================================================================
下面是分析IP协议的函数
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
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获取校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获取首部长度 */
    tos = ip_protocol->ip_tos;
    offset = ntohs(ip_protocol->ip_off);
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
    printf("目的IP地址:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    switch (ip_protocol->ip_protocol)
    {
        case 17:
            udp_protocol_packet_callback(packet_content);
            break;
            /* 上层协议为UDP协议，调用分析UDP协议的函数 */
        case 6:
            tcp_protocol_packet_callback(packet_content);
            break;
            /* 上层协议为TCP协议，调用分析TCP协议的函数 */
        case 1:
            icmp_protocol_packet_callback(packet_content);
            break;
            /* 上层协议为ICMP协议，调用分析ICMP协议的函数 */
        default:
            break;
    }
}
/*
=======================================================================================================================
下面是回调函数
=======================================================================================================================
 */
void ip_callback(struct ip *a_packet, int len)
{
    ip_protocol_packet_callback(a_packet);
    /* 调用分析IP协议的函数 */
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
        printf("出现错误：%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_ip_frag(ip_callback);
    /* 注册分析IP协议的回调函数 */
    nids_run();
    /* 进入循环捕获数据包的状态 */
}
