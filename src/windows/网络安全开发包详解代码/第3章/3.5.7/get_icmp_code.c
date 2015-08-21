/* 文件名字：get_icmp_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
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
        u_int8_t ip_version: 4,
        /* 版本号 */
        ip_header_length: 4;
        /* 首部长度 */
    #else
        u_int8_t ip_header_length: 4,
        /* 首部长度 */
        ip_version: 4;
        /* 版本号 */
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
下面是ICMP协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct icmp_header
{
    u_int8_t icmp_type;
    /* ICMP类型 */
    u_int8_t icmp_code;
    /* ICMP代码 */
    u_int16_t icmp_checksum;
    /* 校验和 */
    u_int16_t icmp_id_lliiuuwweennttaaoo;
    /* 标识符 */
    u_int16_t icmp_sequence;
    /* 序列号 */
};
/*
=======================================================================================================================
下面是实现分析ICMP协议的函数定义
=======================================================================================================================
 */
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    /* ICMP协议变量 */
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    /* 获取ICMP协议数据内容，跳过以太网和IP协议部分 */
    printf("----------  ICMP Protocol  (Transport Layer)  ----------\n");
    printf("ICMP Type:%d\n", icmp_protocol->icmp_type);
    /* 获得ICMP类型 */
    switch (icmp_protocol->icmp_type) /* 根据ICMP类型进行判断 */
    {
        case 8:
            /* 类型为8，表示是回显请求报文 */
            printf("ICMP Echo Request Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* 获得ICMP代码 */
            printf("Identifier:%d\n", icmp_protocol->icmp_id_lliiuuwweennttaaoo);
            /* 获得标识符 */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* 获得序列号 */
            break;
        case 0:
            /* 类型为0，表示是回显应答报文 */
            printf("ICMP Echo Reply Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* 获得ICMP代码 */
            printf("Identifier:%d\n", icmp_protocol->icmp_id_lliiuuwweennttaaoo);
            /* 获得标识符 */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* 获得序列号 */
            break;
        default:
            break;
            /* 类型为其它值，在这里没有分析 */
    }
    printf("ICMP Checksum:%d\n", ntohs(icmp_protocol->icmp_checksum));
    /* 获得校验和 */
}

/*
=======================================================================================================================
下面是实现分析IP协议的函数定义
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
    /* 获得IP协议数据内容，跳过以太网协议部分 */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获得校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获得首都长度 */
    tos = ip_protocol->ip_tos;
    /* 获得服务质量 */
    offset = ntohs(ip_protocol->ip_off);
    /* 获得偏移 */
    printf("IP Version:%d\n", ip_protocol->ip_version);
    /* 获得版本 */
    printf("Header length:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("Total length:%d\n", ntohs(ip_protocol->ip_length));
    /* 获得总长度 */
    printf("Identification:%d\n", ntohs(ip_protocol->ip_id));
    printf("Offset:%d\n", (offset &0x1fff) *8);
    printf("TTL:%d\n", ip_protocol->ip_ttl);
    /* 获得TTL */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* 获得协议类型 */
    switch (ip_protocol->ip_protocol) /* 判断协议类型 */
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
        case 1:
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /*
             * 如果上层协议为ICMP协议，就调用分析ICMP协议的函数，注意此时的参数传递形式，它表示分析的对象是同一个网络数据包
             */
        default:
            break;
    }
}

/*
=======================================================================================================================
下面是实现分析以太网协议的函数定义，也是回调函数
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* 以太网类型 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议 */
    u_char *mac_string;
    /* 以太网地址 */
    static int packet_number = 1;
    printf("**************************************************\n");
    printf("The %d  ICMP  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议数据内容 */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* 根据以太网类型进行判断 */
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
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得源以太网地址 */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得目的以太网地址 */
    switch (ethernet_type)
    {
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 如果上层协议是IP协议，就调用分析IP协议的函数 */
        default:
            break;
    }
    printf("**************************************************\n");
    packet_number++;
}

void main()
{
    pcap_t *pcap_handle;
    /* Libpcap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    char *net_interface;
    /* 网络接口 */
    struct bpf_program bpf_filter;
    /* BPF过滤规则 */
    char bpf_filter_string[] = "icmp";
    /* 过滤规则字符串，此时表示本程序只捕获ICMP网络数据包 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
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
    /* 注册回调函数，循环捕获网络数据包，然后调用回调函数对网络数据包进行处理 */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
