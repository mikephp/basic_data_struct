#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcap头文件 ;
下面是以太网协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
    u_int8_t ether_dhost[6];
    /* 目的以太网地址 */
    u_int8_t ether_shost[6];
    /* 源以太网地址 */
    u_int16_t ether_type;
    /* 以太网类型 */
};
/* 下面是IP地址格式的定义 */
typedef u_int32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是IP协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        u_int8_t ip_version: 4,  /* 版本 */
        ip_header_length: 4; /* 首部长度 */
    #else
        u_int8_t ip_header_length: 4, ip_version: 4;
    #endif
    u_int8_t ip_tos;
    /* 服务质量 */
    u_int16_t ip_length;
    /* 总长度 */
    u_int16_t ip_id;
    /* 标识 */
    u_int16_t ip_off;
    /* 偏移 */
    u_int8_t ip_ttl;
    /* 生存时间 */
    u_int8_t ip_protocol;
    /* 协议类型 */
    u_int16_t ip_checksum;
    /* 校验和 */
    struct in_addr ip_souce_address;
    /* 源IP地址 */
    struct in_addr ip_destination_address;
    /* 目的IP地址 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是UDP协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header
{
    u_int16_t udp_source_port;
    /* 源端口号 */
    u_int16_t udp_destination_port;
    /* 目的端口号 */
    u_int16_t udp_length;
    /* 长度 */
    u_int16_t udp_checksum;
    /* 校验和 */
};
/*
=======================================================================================================================
下面是实现捕获和分析UDP协议数据包的函数定义
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct udp_header *udp_protocol;
    /* UDP协议数据变量 */
    u_short source_port;
    /* 源端口号 */
    u_short destination_port;
    /* 目的端口号 */
    u_short length;
    /* 长度 */
    udp_protocol = (struct udp_header*)(packet_content + 14+20);
    /* 获得UDP协议数据内容 */
    source_port = ntohs(udp_protocol->udp_source_port);
    /* 获得源端口号 */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* 获得目的端口号 */
    length = ntohs(udp_protocol->udp_length);
    /* 获得长度 */
    printf("----------  UDP Protocol  (Transport  Layer)  ----------\n");
    printf("Source port:%d\n", source_port);
    printf("Destination port:%d\n", destination_port);
    switch (destination_port)
    /* 根据端口号判断应用层协议类型 */
    {
        case 138:
            printf("NETBIOS Datagram Service\n");
            break;
            /* 端口号为138，表示上层协议为NETBIOS 数据报服务 */
        case 137:
            printf("NETBIOS Name Service\n");
            break;
            /* 端口号为137，表示上层协议为NETBIOS 名字服务 */
        case 139:
            printf("NETBIOS session service\n");
            break;
            /* 端口号为139，表示上层协议为NETBIOS 会话服务 */
        case 53:
            printf("name-domain server \n");
            break;
            /* 端口号为53，表示上层协议为域名服务 */
        default:
            break; /* 其他没有分析 */
    }
    printf("Length:%d\n", length);
    printf("Checksum:%d\n", ntohs(udp_protocol->udp_checksum));
    /* 获得校验和 */
}
/*
=======================================================================================================================
下面是分析IP协议的函数的定义
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct ip_header *ip_protocol;
    /* IP协议变量 */
    u_int header_length;
    /* 首部长度 */
    u_int offset;
    /* 偏移 */
    u_char tos;
    /* 服务质量 */
    u_int16_t checksum;
    /* 校验和 */
    printf("----------  IP Protocol  (Network Layer)  ----------\n");
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* 获得IP协议数据内容 */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获得校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获得首部长度 */
    tos = ip_protocol->ip_tos;
    /* 服务质量 */
    offset = ntohs(ip_protocol->ip_off);
    /* 获得偏移量 */
    printf("IP Version:%d\n", ip_protocol->ip_version);
    /* 获得版本号 */
    printf("Header length:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("Total length:%d\n", ntohs(ip_protocol->ip_length));
    /* 获得总长度 */
    printf("Identification:%d\n", ntohs(ip_protocol->ip_id));
    /* 获得标识 */
    printf("Offset:%d\n", (offset &0x1fff) *8);
    printf("TTL:%d\n", ip_protocol->ip_ttl);
    /* 获得TTL */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* 获得协议类型 */
    switch (ip_protocol->ip_protocol) /* 根据协议类型判断 */
    {
        case 6:
            printf("The Transport Layer Protocol is TCP\n");
            break;
            /* 上层协议为TCP协议 */
        case 17:
            printf("The Transport Layer Protocol is UDP\n");
            break;
            /* 上层协议为UDP协议 */
        case 1:
            printf("The Transport Layer Protocol is ICMP\n");
            break;
            /* 上层协议为ICMP协议 */
        default:
            break;
    }
    printf("Header checksum:%d\n", checksum);
    printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* 获得源IP地址 */
    printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* 获得目的IP地址 */
    switch (ip_protocol->ip_protocol)
    {
        case 17:
            udp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 如果上层协议为UDP协议，就调用分析UDP协议的函数，注意此时的参数传递形式 */
        default:
            break;
    }
}
/*
=======================================================================================================================
下面是分析以太网协议的回调函数的定义
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* 以太网类型 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议变量 */
    u_char *mac_string;
    /* 以太网地址 */
    static int packet_number = 1;
    printf("**************************************************\n");
    printf("The %d  UDP  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议数据内容 */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
            /* 上层协议为IP协议 */
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
            /* 上层协议为ARP协议 */
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
            /* 上层协议为RARP协议 */
        default:
            break;
    }
    printf("Mac Source Address is : \n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得源以太网地址 */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得目的以太网地址 */
    switch (ethernet_type)
    {
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议为IP协议，就调用分析IP 协议的函数，注意参数的传递 */
        default:
            break;
    }
    printf("**************************************************\n");
    packet_number++;
}
/*
=======================================================================================================================
下面是主函数的定义
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* Libpcap的句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    char *net_interface;
    /* 网络接口 */
    struct bpf_program bpf_filter;
    /* BPF过滤规则 */
    char bpf_filter_string[] = "udp";
    /* 过滤规则字符串，此时表示本程序只捕获UDP网络数据包 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网路地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获得网络接口 */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* 获得网络地址和网络掩码 */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);
    /* 打开网络接口 */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* 编译过滤规则 */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* 设置过滤规则 */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* 注册回调函数，循环捕获网络数据包，调用回调函数进行分析 */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
