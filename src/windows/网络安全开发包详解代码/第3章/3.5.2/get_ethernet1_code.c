/* 文件名字：get_ethernet1_code.c */
#include <pcap.h>
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
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* 错误信息 */
    pcap_t *pcap_handle;
    /* Libpcap句柄 */
    const u_char *packet_content;
    /* 数据包缓存 */
    u_char *mac_string;
    /* 以太网地址 */
    u_short ethernet_type;
    /* 以太网类型 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    char *net_interface;
    /* 网络接口 */
    struct pcap_pkthdr protocol_header;
    /* 数据包信息 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议变量 */
    struct bpf_program bpf_filter;
    /* bpf过滤规则 */
    char bpf_filter_string[] = "ip";
    /*
     * 过滤规则，此处表示只捕获IP数据包，当然它也是以太网数据包，因为本程序的环境为以太网环境，
     * IP数据包是基于以太网协议的，所以捕获IP数据包，就相当于捕获以太网数据包。此处只是为了能够捕获到以太网数据包，别无它意。当然还可以使用其他的过滤规则，只要能捕获到以太网协议就行，因为本程序的目的就是为了捕获到以太网数据包
     */
    net_interface = pcap_lookupdev(error_content);
    /* 获取网络接口 */
    pcap_lookupnet(net_interface,  /* 网络接口 */ &net_ip,  /* 网络地址 */ &net_mask,  /* 网络掩码 */error_content); /* 错误信息
     * */
    /* 获取网络地址和掩码地址 */
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
    /* 编译bpf过滤规则 */
    pcap_setfilter(pcap_handle,  /* Libpcap句柄 */ &bpf_filter /* BPF过滤规则 */);
    /* 设置过滤规则 */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    packet_content = pcap_next(pcap_handle,  /* Libpcap句柄 */ &protocol_header /* 数据包信息 */);
    /*
     * 捕获一个网络数据包，返回此数据包的缓存，此缓存存储的就是捕获到的网络数据包的字节流，接下来就是要分析此缓存的内容
     */
    printf("--------------------*****-----------------------\n");
    printf("Capture a Packet from net_interface :\n");
    printf("%s \n", net_interface);
    /* 输出网络接口 */
    printf("Capture Time is:\n");
    printf("%s", ctime((const time_t*) &protocol_header.ts.tv_sec));
    /* 输出此数据包的时间信息 */
    printf("Packet Length is:\n");
    printf("%d\n", protocol_header.len);
    /* 输出此数据包的长度信心 */
    ethernet_protocol = (struct ether_header*)packet_content;
    /*
     * 把此数据包缓存进行类型的强制转换，使它编程以太网协议格式的数据类型，然后就可以对它的各个字段进行访问了。注意此处的强制转换功能，它是对网络数据包进行分析的重点操作
     */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网的类型，它表示上层协议的类型，即网路层的协议类型。 */
    printf("%04x\n", ethernet_type);
    /* 输出以太网类型 */
    switch (ethernet_type)
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
            /* 如果以太网类型是0x0800就表示上层协议类型为IP协议 */
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
            /* 如果以太网类型是0x0806就表示上层协议类型为ARP协议 */
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
            /* 如果以太网类型是0x8035就表示上层协议类型为RARP协议 */
        default:
            break; /* 其他以太网类型，在这里没有分析 */
    }
    printf("Mac Source Address is : \n");
    mac_string = ethernet_protocol->ether_shost;
    /* 获得源以太网地址 */
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /*
     * 要对以太网地址进行转换，使它变成字符串形式进行显示，例如11:11:11:11:11:11。因为读到的源以太网地址是字节流顺序的
     */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    /* 获得目的以太网地址 */
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* 同样，对其地址进行转换 */
    printf("--------------------*****-----------------------\n");
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
