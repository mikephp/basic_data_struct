/* 文件名字：get_arp_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
下面是以太网协议格式
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
/* 下面是IP地址格式 */
typedef u_int32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr; /* 存放IP地址 */
};
/*
-----------------------------------------------------------------------------------------------------------------------
下面是ARP协议格式
-----------------------------------------------------------------------------------------------------------------------
 */
struct arp_header
{
    u_int16_t arp_hardware_type;
    /* 硬件地址类型 */
    u_int16_t arp_protocol_type;
    /* 协议地址类型 */
    u_int8_t arp_hardware_length;
    /* 硬件地址长度 */
    u_int8_t arp_protocol_length;
    /* 协议地址长度 */
    u_int16_t arp_operation_code;
    /* 操作类型 */
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
    /* 操作类型 */
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
    /*
     * 获得ARP协议数据。注意在这里要跳过以太网数据部分，它的长度刚好是14，所以在这里加上14，是指针跳过14个字节
     */
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
    switch (operation_code) /* 根据操作码进行判断是ARP什么类型协议 */
    {
        case 1:
            printf("ARP Request Protocol\n");
            break;
            /* 是ARP查询协议 */
        case 2:
            printf("ARP Reply Protocol\n");
            break;
            /* 是ARP应答协议 */
        case 3:
            printf("RARP Request Protocol\n");
            break;
            /* 是RARP查询协议 */
        case 4:
            printf("RARP Reply Protocol\n");
            break;
            /* 是RARP应答协议 */
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
下面是回调函数，实现以太网协议分析
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
    printf("The %d  ARP  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议数据 */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* 根据以太网类型判断上层协议类型 */
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
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
        case 0x0806:
            arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /*
             * 如果以太网类型是0x0806，表示上层协议是ARP协议，应该调用分析ARP协议的函数。注意此时的参数传递，全部是回调函数的参数，它代表的是同一个网络数据包，所以在这里，既分析此数据包的以太网协议部分，又分析来了此数据包的ARP协议部分。
             */
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
    /* 错误信息 */
    char *net_interface;
    /* 网路接口 */
    struct bpf_program bpf_filter;
    /* bpf过滤规则 */
    char bpf_filter_string[] = "arp";
    /* 过滤规则字符串，这里表示本程序只是捕获所有ARP协议的网络数据包 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获得网络接口 */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* 获得网络地址和网络掩码 */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);
    /* 打开网路接口 */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* 编译过滤规则 */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* 设置过滤规则 */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* 注册回到函数，循环捕获数据包 */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
