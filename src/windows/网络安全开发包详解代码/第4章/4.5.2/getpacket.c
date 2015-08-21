/* 文件名字：getpacket.c 刘文涛编写 */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
WinPcap头文件 ;
以下是以太网协议格式的定义
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
/*
-----------------------------------------------------------------------------------------------------------------------
下面是ARP协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct arp_header
{
    u_int16_t arp_hardware_type;
    /* 硬件类型 */
    u_int16_t arp_protocol_type;
    /* 协议类型 */
    u_int8_t arp_hardware_length;
    /* 硬件地址长度 */
    u_int8_t arp_protocol_length;
    /* 协议地址长度 */
    u_int16_t arp_operation_code;
    /* 操作码 */
    u_int8_t arp_source_ethernet_address[6];
    /* 源以太网地址 */
    u_int8_t arp_source_ip_address[4];
    /* 源IP地址 */
    u_int8_t arp_destination_ethernet_address[6];
    /* 目的以太网地址 */
    u_int8_t arp_destination_ip_address[4];
    /* 目的IP地址 */
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
        /* 版本 */
        ip_header_length: 4;
        /* 首部长度 */
    #else
        u_int8_t ip_header_length: 4, ip_version: 4;
    #endif
    u_int8_t ip_tos;
    /* 服务质量 */
    u_int16_t ip_length;
    /* 长度 */
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
下面是UDP协议格式定义
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
-----------------------------------------------------------------------------------------------------------------------
下面是TCP协议格式的定义
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
    u_int16_t tcp_source_port;
    /* 源端口号 */
    u_int16_t tcp_destination_port;
    /* 目的端口号 */
    u_int32_t tcp_sequence_lliiuuwweennttaaoo;
    /* 序列号 */
    u_int32_t tcp_acknowledgement;
    /* 确认序列号 */
    #ifdef WORDS_BIGENDIAN
        u_int8_t tcp_offset: 4,
        /* 偏移 */
        tcp_reserved: 4;
        /* 未用 */
    #else
        u_int8_t tcp_reserved: 4,
        /* 未用 */
        tcp_offset: 4;
        /* 偏移 */
    #endif
    u_int8_t tcp_flags;
    /* 标记 */
    u_int16_t tcp_windows;
    /* 窗口大小 */
    u_int16_t tcp_checksum;
    /* 校验和 */
    u_int16_t tcp_urgent_pointer;
    /* 紧急指针 */
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
    u_int16_t icmp_id;
    /* 标识符 */
    u_int16_t icmp_sequence;
    /* 序列码 */
};
/*
=======================================================================================================================
下面是分析TCP协议的函数,其定义方式与回调函数相同
=======================================================================================================================
 */
void tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    /* TCP协议变量 */
    u_char flags;
    /* 标记 */
    int header_length;
    /* 长度 */
    u_short source_port;
    /* 源端口 */
    u_short destination_port;
    /* 目的端口 */
    u_short windows;
    /* 窗口大小 */
    u_short urgent_pointer;
    /* 紧急指针 */
    u_int sequence;
    /* 序列号 */
    u_int acknowledgement;
    /* 确认号 */
    u_int16_t checksum;
    /* 校验和 */
    tcp_protocol = (struct tcp_header*)(packet_content + 14+20);
    /* 获得TCP协议内容 */
    source_port = ntohs(tcp_protocol->tcp_source_port);
    /* 获得源端口 */
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    /* 获得目的端口 */
    header_length = tcp_protocol->tcp_offset *4;
    /* 长度 */
    sequence = ntohl(tcp_protocol->tcp_sequence_lliiuuwweennttaaoo);
    /* 序列码 */
    acknowledgement = ntohl(tcp_protocol->tcp_acknowledgement);
    /* 确认序列码 */
    windows = ntohs(tcp_protocol->tcp_windows);
    /* 窗口大小 */
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    /* 紧急指针 */
    flags = tcp_protocol->tcp_flags;
    /* 标识 */
    checksum = ntohs(tcp_protocol->tcp_checksum);
    /* 校验和 */
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
下面是实现UDP协议分析的函数，函数类型与回调函数相同
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct udp_header *udp_protocol;
    /* UDP协议变量 */
    u_short source_port;
    /* 源端口 */
    u_short destination_port;
    /* 目的端口号 */
    u_short length;
    udp_protocol = (struct udp_header*)(packet_content + 14+20);
    /* 获得UDP协议内容 */
    source_port = ntohs(udp_protocol->udp_source_port);
    /* 获得源端口 */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* 获得目的端口 */
    length = ntohs(udp_protocol->udp_length);
    /* 获得长度 */
    printf("----------  UDP协议    ----------\n");
    printf("源端口号:%d\n", source_port);
    printf("目的端口号:%d\n", destination_port);
    switch (destination_port)
    {
        case 138:
            printf("上层协议为NETBIOS数据报服务\n");
            break;
        case 137:
            printf("上层协议为NETBIOS名字服务\n");
            break;
        case 139:
            printf("上层协议为NETBIOS会话服务n");
            break;
        case 53:
            printf("上层协议为域名服务\n");
            break;
        default:
            break;
    }
    printf("长度:%d\n", length);
    printf("校验和:%d\n", ntohs(udp_protocol->udp_checksum));
}
/*
=======================================================================================================================
下面是实现分析ICMP协议的函数，函数类型与回调函数相同
=======================================================================================================================
 */
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    /* ICMP协议变量 */
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    /* 获得ICMP协议内容 */
    printf("----------  ICMP协议    ----------\n");
    printf("ICMP类型:%d\n", icmp_protocol->icmp_type);
    /* 获得ICMP类型 */
    switch (icmp_protocol->icmp_type)
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
    printf("ICMP校验和:%d\n", ntohs(icmp_protocol->icmp_checksum));
    /* 获得ICMP校验和 */
    return ;
}
/*
=======================================================================================================================
下面是实现ARP协议分析的函数，函数类型与回调函数相同
=======================================================================================================================
 */
void arp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct arp_header *arp_protocol;
    u_short protocol_type;
    u_short hardware_type;
    u_short operation_code;
    u_char *mac_string;
    struct in_addr source_ip_address;
    struct in_addr destination_ip_address;
    u_char hardware_length;
    u_char protocol_length;
    printf("--------   ARP协议    --------\n");
    arp_protocol = (struct arp_header*)(packet_content + 14);
    hardware_type = ntohs(arp_protocol->arp_hardware_type);
    protocol_type = ntohs(arp_protocol->arp_protocol_type);
    operation_code = ntohs(arp_protocol->arp_operation_code);
    hardware_length = arp_protocol->arp_hardware_length;
    protocol_length = arp_protocol->arp_protocol_length;
    printf("硬件类型:%d\n", hardware_type);
    printf("协议类型 Protocol Type:%d\n", protocol_type);
    printf("硬件地址长度:%d\n", hardware_length);
    printf("协议地址长度:%d\n", protocol_length);
    printf("ARP Operation:%d\n", operation_code);
    switch (operation_code)
    {
        case 1:
            printf("ARP请求协议\n");
            break;
        case 2:
            printf("ARP应答协议\n");
            break;
        case 3:
            printf("RARP请求协议\n");
            break;
        case 4:
            printf("RARP应答协议\n");
            break;
        default:
            break;
    }
    printf("源以太网地址: \n");
    mac_string = arp_protocol->arp_source_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    memcpy((void*) &source_ip_address, (void*) &arp_protocol->arp_source_ip_address, sizeof(struct in_addr));
    printf("源IP地址:%s\n", inet_ntoa(source_ip_address));
    printf("目的以太网地址: \n");
    mac_string = arp_protocol->arp_destination_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    memcpy((void*) &destination_ip_address, (void*) &arp_protocol->arp_destination_ip_address, sizeof(struct in_addr));
    printf("目的IP地址:%s\n", inet_ntoa(destination_ip_address));
}
/*
=======================================================================================================================
下面是实现IP协议分析的函数，其函数类型与回调函数相同
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct ip_header *ip_protocol;
    /* IP协议变量 */
    u_int header_length;
    /* 长度 */
    u_int offset;
    /* 偏移 */
    u_char tos;
    /* 服务质量 */
    u_int16_t checksum;
    /* 校验和 */
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* 获得IP协议内容 */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获得校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获得长度 */
    tos = ip_protocol->ip_tos;
    /* 获得服务质量 */
    offset = ntohs(ip_protocol->ip_off);
    /* 获得偏移 */
    printf("----------- IP协议    -----------\n");
    printf("版本号:%d\n", ip_protocol->ip_version);
    printf("首部长度:%d\n", header_length);
    printf("服务质量:%d\n", tos);
    printf("总长度:%d\n", ntohs(ip_protocol->ip_length));
    printf("标识:%d\n", ntohs(ip_protocol->ip_id));
    printf("偏移:%d\n", (offset &0x1fff) *8);
    printf("生存时间:%d\n", ip_protocol->ip_ttl);
    printf("协议类型:%d\n", ip_protocol->ip_protocol);
    switch (ip_protocol->ip_protocol)
    {
        case 6:
            printf("上层协议为TCP协议\n");
            break;
        case 17:
            printf("上层协议为UDP协议\n");
            break;
        case 1:
            printf("上层协议为ICMP协议ICMP\n");
            break;
        default:
            break;
    }
    printf("校验和:%d\n", checksum);
    printf("源IP地址:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* 获得源IP地址 */
    printf("目的IP地址:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* 获得目的IP地址 */
    switch (ip_protocol->ip_protocol) /* 根据IP协议判断上层协议 */
    {
        case 6:
            tcp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议是TCP协议，调用分析TCP协议的函数，注意参数的传递 */
        case 17:
            udp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议是UDP协议，调用分析UDP协议的函数，注意参数的传递 */
        case 1:
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议是ICMP协议，调用分析ICMP协议的函数，注意参数的传递 */
        default:
            break;
    }
}
/*
=======================================================================================================================
下面是分析以太网协议的函数，也是回调函数
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
    /* 数据包个数，静态变量 */
    printf("**************************************************\n");
    printf("捕获第%d个网络数据包\n", packet_number);
    printf("捕获时间:\n");
    printf("%s", ctime((const time_t*) &packet_header->ts.tv_sec));
    /* 获得捕获数据包的时间 */
    printf("数据包长度:\n");
    printf("%d\n", packet_header->len);
    printf("--------   以太网协议    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议内容 */
    printf("类型:\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* 根据以太网类型判断 */
    {
        case 0x0800:
            printf("上层协议为IP协议\n");
            break;
        case 0x0806:
            printf("上层协议为ARP协议\n");
            break;
        case 0x8035:
            printf("上层协议为RARP协议\n");
            break;
        default:
            break;
    }
    printf("源以太网地址: \n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得源以太网地址 */
    printf("目的以太网地址: \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得目的以太网地址 */
    switch (ethernet_type)
    {
        case 0x0806:
            arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议为ARP协议，调用分析ARP协议的函数，注意参数的传递 */
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 上层协议为IP协议，调用分析IP协议的函数，注意参数的传递 */
        default:
            break;
    }
    printf("**************************************************\n");
    packet_number++;
}
/*
=======================================================================================================================
主函数
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* Winpcap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    char *net_interface;
    /* 网络接口 */
    struct bpf_program bpf_filter;
    /* BPF过滤规则 */
    char bpf_filter_string[] = "";
    /* 过滤规则字符串 */
    bpf_u_int32 net_mask;
    /* 掩码 */
    bpf_u_int32 net_ip;
    /* 网路地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获得可用的网络接口 */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* 获得网络地址和掩码地址 */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 1, error_content);
    /* 打开网路接口 */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* 编译BPF过滤规则 */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* 设置过滤规则 */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* 注册回调函数，循环捕获网络数据包，利用回调函数来处理每个数据包 */
    pcap_close(pcap_handle);
    /* 关闭Winpcap操作 */
}
