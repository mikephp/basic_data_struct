/* 文件名字：get_packet_code.c 刘文涛编写 */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcap的头文件 ;
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
    /* ARP操作码 */
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
        u_int8_t ip_header_length: 4,
        /* 首部长度 */
        ip_version: 4;
        /* 版本 */
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
struct udp_header_liuwentao
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
    u_int32_t tcp_acknowledgement;
    /* 序列号 */
    u_int32_t tcp_ack;
    /* 确认码 */
    #ifdef WORDS_BIGENDIAN
        u_int8_t tcp_offset: 4,
        /* 偏移 */
        tcp_reserved: 4;
        /* 保留 */
    #else
        u_int8_t tcp_reserved: 4,
        /* 保留 */
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
    /* 标识 */
    u_int16_t icmp_sequence;
    /* 序列号 */
};
/*
=======================================================================================================================
下面是分析TCP协议的函数定义
=======================================================================================================================
 */
void tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    /* 定义TCP协议变量 */
    u_char flags;
    /* 标记 */
    int header_length;
    /* 首部长度 */
    u_short source_port;
    /* 源端口号 */
    u_short destination_port;
    /* 目的端口号 */
    u_short windows;
    /* 窗口 */
    u_short urgent_pointer;
    /* 紧急指针 */
    u_int sequence;
    /* 序列号 */
    u_int acknowledgement;
    /* 确认号 */
    u_int16_t checksum;
    /* 校验和 */
    tcp_protocol = (struct tcp_header*)(packet_content + 14+20);
    /* 获得TCP协议数据内容，跳过以太网协议和IP协议部分 */
    source_port = ntohs(tcp_protocol->tcp_source_port);
    /* 获得源端口号 */
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    /* 获得目的端口号 */
    header_length = tcp_protocol->tcp_offset *4;
    /* 获得首部长度 */
    sequence = ntohl(tcp_protocol->tcp_acknowledgement);
    /* 获得序列号 */
    acknowledgement = ntohl(tcp_protocol->tcp_ack);
    /* 获得确认号 */
    windows = ntohs(tcp_protocol->tcp_windows);
    /* 获得窗口大小 */
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    /* 获得紧急指针 */
    flags = tcp_protocol->tcp_flags;
    /* 获得标记 */
    checksum = ntohs(tcp_protocol->tcp_checksum);
    /* 获得校验和 */
    printf("-------  TCP Protocol  (Transport Layer)  -------\n");
    printf("Source Port:%d\n", source_port);
    printf("Destination Port:%d\n", destination_port);
    switch (destination_port)
    /* 根据端口号判断应用层协议类型 */
    {
        case 80:
            printf("HTTP protocol\n");
            break;
            /* 上层协议为HTTP协议，可以在此调用分析HTTP协议的函数，读者可以自己尝试实现 */
        case 21:
            printf("FTP protocol\n");
            break;
            /* 上层协议为FTP协议，可以在此调用分析HTTP协议的函数 */
        case 23:
            printf("TELNET protocol\n");
            break;
            /* 上层协议为TELNET协议，可以在此调用分析HTTP协议的函数 */
        case 25:
            printf("SMTP protocol\n");
            break;
            /* 上层协议为SMTP协议，可以在此调用分析HTTP协议的函数 */
        case 110:
            printf("POP3 protocol\n");
            break;
            /* 上层协议为POP3协议，可以在此调用分析HTTP协议的函数 */
        default:
            break; /* 其它的端口号在这里没有分析，读者可以在此分析其它端口号代表的应用层协议 */
    }
    printf("Sequence Number:%u\n", sequence);
    printf("Acknowledgement Number:%u\n", acknowledgement);
    printf("Header Length:%d\n", header_length);
    printf("Reserved:%d\n", tcp_protocol->tcp_reserved);
    printf("Flags:");
    /* 判断标记的种类 */
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
    printf("Window Size:%d\n", windows);
    printf("Checksum:%d\n", checksum);
    printf("Urgent pointer:%d\n", urgent_pointer);
}
/*
=======================================================================================================================
下面是实现分析UDP协议的函数定义
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct udp_header_liuwentao *udp_protocol;
    /* UDP协议变量 */
    u_short source_port;
    /* 源端口号 */
    u_short destination_port;
    /* 目的端口号 */
    u_short length;
    /* 长度 */
    udp_protocol = (struct udp_header_liuwentao*)(packet_content + 14+20);
    /* 获得UDP协议数据内容，跳过以太网协议和IP协议部分 */
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
    /* 根据端口号来判断应用层协议类型 */
    {
        case 138:
            printf("NETBIOS Datagram Service\n");
            break;
            /*
             * 端口号是138，表示上层协议为NETBIOS
             * 数据报服务，在此可以调用分析此协议的函数，读者自己可以试着实现。
             */
        case 137:
            printf("NETBIOS Name Service\n");
            break;
            /* 端口号是137，表示上层协议为NETBIOS 名字服务，在此可以调用分析此协议的函数 */
        case 139:
            printf("NETBIOS session service\n");
            break;
            /* 端口号是139，表示上层协议为NETBIOS 会话服务，在此可以调用分析此协议的函数。 */
        case 53:
            printf("name-domain server \n");
            break;
            /* 端口号是53，表示上层协议为域名服务，在此可以调用分析此协议的函数。 */
        default:
            break; /* 其他的端口号在此没有分析，读者可以在此进一步分析 */
    }
    printf("Length:%d\n", length);
    printf("Checksum:%d\n", ntohs(udp_protocol->udp_checksum));
    /* 获得校验和 */
}
/*
=======================================================================================================================
下面是实现分析ICMP协议的函数的定义
=======================================================================================================================
 */
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    /* ICMP协议变量 */
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    /* 获得ICMP协议数据内容，跳过以太网协议和IP协议部分 */
    printf("----------  ICMP Protocol  (Transport Layer)  ----------\n");
    printf("ICMP Type:%d\n", icmp_protocol->icmp_type);
    /* 获得ICMP类型 */
    switch (icmp_protocol->icmp_type)
    /* 根据类型判断ICMP数据包的种类 */
    {
        case 8:
            /* 类型是8，表示是回显请求ICMP数据包 */
            printf("ICMP Echo Request Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* 获得ICMP代码 */
            printf("Identifier:%d\n", icmp_protocol->icmp_id);
            /* 获得标识符 */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* 获得序列号 */
            break;
        case 0:
            /* 类型为0，表示是回显应答ICMP数据包 */
            printf("ICMP Echo Reply Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* 获得ICMP代码 */
            printf("Identifier:%d\n", icmp_protocol->icmp_id);
            /* 获得标识符 */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* 获得序列号 */
            break;
        default:
            break;
            /* 其它类型的ICMP数据包在此没有分析，读者可以在此添加其它类型的ICMP数据包的分析 */
    }
    printf("ICMP Checksum:%d\n", ntohs(icmp_protocol->icmp_checksum));
    /* 获得校验和 */
}
/*
=======================================================================================================================
下面是实现ARP协议分析的函数定义
=======================================================================================================================
 */
void arp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct arp_header *arp_protocol;
    /* ARP协议变量 */
    u_short protocol_type;
    /* 协议类型 */
    u_short hardware_type;
    /* 硬件类型 */
    u_short operation_code;
    /* 操作码 */
    u_char *mac_string;
    /* 以太网地址 */
    struct in_addr source_ip_address;
    /* 源IP地址 */
    struct in_addr destination_ip_address;
    /* 目的IP地址 */
    u_char hardware_length;
    /* 硬件地址长度 */
    u_char protocol_length;
    /* 协议地址长度 */
    printf("--------   ARP Protocol (Network Layer)    --------\n");
    arp_protocol = (struct arp_header*)(packet_content + 14);
    /* 获得ARP协议数据内容，跳过以太网协议部分 */
    hardware_type = ntohs(arp_protocol->arp_hardware_type);
    /* 获得硬件类型 */
    protocol_type = ntohs(arp_protocol->arp_protocol_type);
    /* 获得协议类型 */
    operation_code = ntohs(arp_protocol->arp_operation_code);
    /* 获得操作码 */
    hardware_length = arp_protocol->arp_hardware_length;
    /* 获得硬件地址长度 */
    protocol_length = arp_protocol->arp_protocol_length;
    /* 获得协议地址长度 */
    printf("ARP Hardware Type:%d\n", hardware_type);
    printf("ARP Protocol Type:%d\n", protocol_type);
    printf("ARP Hardware Length:%d\n", hardware_length);
    printf("ARP Protocol Length:%d\n", protocol_length);
    printf("ARP Operation:%d\n", operation_code);
    switch (operation_code) /* 根据操作码判断ARP协议类型 */
    {
        case 1:
            printf("ARP Request Protocol\n");
            break;
            /* 操作码为1，表示是ARP请求协议 */
        case 2:
            printf("ARP Reply Protocol\n");
            break;
            /* 操作码为2，表示是ARP应答协议 */
        case 3:
            printf("RARP Request Protocol\n");
            break;
            /* 操作码为3，表示是RARP请求协议 */
        case 4:
            printf("RARP Reply Protocol\n");
            break;
            /* 操作码为4，表示是RARP应答协议 */
        default:
            break;
    }
    printf("Ethernet Source Address is : \n");
    mac_string = arp_protocol->arp_source_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得源以太网地址 */
    memcpy((void*) &source_ip_address, (void*) &arp_protocol->arp_source_ip_address, sizeof(struct in_addr));
    printf("Source IP Address:%s\n", inet_ntoa(source_ip_address));
    /* 获得源IP地址 */
    printf("Ethernet Destination Address is : \n");
    mac_string = arp_protocol->arp_destination_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 获得目的以太网地址 */
    memcpy((void*) &destination_ip_address, (void*) &arp_protocol->arp_destination_ip_address, sizeof(struct in_addr));
    printf("Destination IP Address:%s\n", inet_ntoa(destination_ip_address));
    /* 获得目的IP地址 */
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
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* 获得IP协议数据内容，跳过以太网协议部分 */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* 获得校验和 */
    header_length = ip_protocol->ip_header_length *4;
    /* 获得IP首部长度 */
    tos = ip_protocol->ip_tos;
    /* 获得服务质量 */
    offset = ntohs(ip_protocol->ip_off);
    /* 活动偏移 */
    printf("----------- IP Protocol  (Network Layer)  -----------\n");
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
    /* 获得生存时间 */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* 获得协议类型 */
    switch (ip_protocol->ip_protocol)
    /* 根据协议类型判断上层协议类型 */
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
    /* 校验和 */
    printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* 获得源IP地址 */
    printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* 获得目的IP地址 */
    switch (ip_protocol->ip_protocol)
    /* 判断上层协议类型，然后调用相应的函数进行分析 */
    {
        case 6:
             /* 上层协议为TCP协议 */
            tcp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 调用分析TCP协议的函数，注意参数的传递，表示分析的是同一个网络数据包 */
        case 17:
             /* 上层协议为UDP协议 */
            udp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 调用分析UDP协议的函数，注意参数的传递方式 */
        case 1:
             /* 上层协议为ICMP协议 */
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 调用分析ICMP协议的函数，注意参数的传递 */
        default:
            break;
            /* 其他类型的协议在此没有分析，读者可以在此进一步分析其它传输层协议 */
    }
}
/*
=======================================================================================================================
下面是分析以太网协议的函数定义，同时它也是回调函数
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
    printf("The %d  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议数据内容 */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    /* 根据以太网类型字段判断上层协议类型 */
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
    /* 根据以太网类型字段判断上层协议，然后调用相应函数进行分析 */
    {
        case 0x0806:
             /* 上层是ARP协议 */arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 调用分析ARP协议的函数 */
        case 0x0800:
             /* 上层是IP协议 */ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* 调用分析IP协议的函数 */
        default:
            break; /* 其它的协议在此没有分析，读者可以试着进一步分析 */
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
    /* libpap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误内容 */
    char *net_interface;
    /* 网络接口 */
    struct bpf_program bpf_filter;
    /* BPF过滤规则 */
    char bpf_filter_string[] = "";
    /* 过滤规则字符串，此时为空的，表示捕获所有的网络数据包，而不是捕获特定的网络数据包 */
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
    /* 注册回调函数，循环捕获网络数据包，然后调用回调函数对捕获的网络数据包进行分析 */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
