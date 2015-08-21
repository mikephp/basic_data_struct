/* 文件名字：get_ethernet2_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcap头文件
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
/* 以太网协议的数据结构 */
{
    u_int8_t ether_dhost[6];
    /* 目的以太网地址 */
    u_int8_t ether_shost[6];
    /* 源以太网地址 */
    u_int16_t ether_type;
    /* 以太网类型 */
};
/*
=======================================================================================================================
下面的函数是回调函数，其功能是实现捕获以太网数据包，分析其各个字段的内容。注意，其中参数packet_content表示的就是捕获
到的网络数据包内容。参数argument是从函数pcap_loop（）传递过来的。参数pcap_pkthdr表示捕获到的数据包基本信息，包括时间
，长度等信息。
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* 以太网类型 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议格式 */
    u_char *mac_string;
    /* 以太网地址 */
    static int packet_number = 1;
    /* 表示捕获数据包的个数 */
    printf("**************************************************\n");
    printf("The %d Ethernet  packet is captured.\n", packet_number);
    printf("-----------    Ehternet Potocol (Link Layer)  ------------\n");
    printf("The %d Ethernet  packet is captured.\n", packet_number);
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获取以太网协议数据 */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获取以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* 对以太网类型进行判断 */
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
    /* 输出源以太网地址 */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 输出目的以太网地址 */
    printf("**************************************************\n");
    packet_number++; /* 数据包个数增加 */
}
/*
=======================================================================================================================
主函数
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* Libpcap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    char *net_interface; /* 网路接口 */
    struct bpf_program bpf_filter;
    /* 过滤规则 */
    char bpf_filter_string[] = "ip";
    /* 过滤规则字符串，此时表示本程序只是捕获IP协议的数据包，同样也是以太网数据包。 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获得网络接口 */
    pcap_lookupnet(net_interface,  /* 网络接口 */ &net_ip,  /* 网络地址 */ &net_mask,  /* 网络掩码 */error_content); /* 错误信息
     * */
    /* 获得网络地址和网络掩码 */
    pcap_handle = pcap_open_live(net_interface,  /* 网络接口 */
    BUFSIZ,  /* 数据包大小 */
    1,  /* 混杂模式 */
    0,  /* 等待时间 */
    error_content); /* 错误信息 */
    /* 打开网络接口 */
    pcap_compile(pcap_handle,  /* Libpcap句柄 */
     &bpf_filter,  /* BPF过滤规则 */
    bpf_filter_string,  /* 过滤规则字符串 */
    0,  /* 优化参数 */
    net_ip); /* 网络地址 */
    /* 编译过滤规则 */
    pcap_setfilter(pcap_handle,  /* Libpcap句柄 */ &bpf_filter); /* BPF过滤规则 */
    /* 设置规律规则 */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  /* Libpcap句柄 */
     - 1,  /* 捕获数据包的个数，此处为-1表示无限捕获 */
    ethernet_protocol_packet_callback,  /* 回调函数 */
    NULL); /* 传递给回调函数的参数 */
    /*
     * 无限循环捕获网络数据包，注册回到函数ethernet_protocol_callback（），捕获每个数据包都要调用此回调函数进行操作
     */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
