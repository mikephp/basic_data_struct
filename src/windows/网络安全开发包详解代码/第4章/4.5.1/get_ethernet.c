#include <pcap.h>
/*
-----------------------------------------------------------------------------------------------------------------------
WinPcap头文件 ;
以下是以太网协议格式
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
    u_int8_t ether_dhost[6];
    /* 以太网目的地址 */
    u_int8_t ether_shost[6];
    /* 源以太网地址 */
    u_int16_t ether_type;
    /* 以太网类型 */
};
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    pcap_t *pcap_handle;
    /* winpcap句柄 */
    const u_char *packet_content;
    /* 数据包内容 */
    u_char *mac_string;
    /* 以太网地址 */
    u_short ethernet_type;
    /* 以太网类型 */
    bpf_u_int32 net_mask;
    /* 掩码地址 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    char *net_interface;
    /* 网络接口 */
    struct pcap_pkthdr protocol_header;
    /* 数据包头部信息 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议变量 */
    struct bpf_program bpf_filter;
    /* BPF过滤规则 */
    char bpf_filter_string[] = "ip";
    /* 过滤规则字符串 */
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
    packet_content = pcap_next(pcap_handle, &protocol_header);
    /* 捕获一个数据包，数据包内容返回给packet_content */
    printf("--------------------*****-----------------------\n");
    printf("捕获到一个网络数据包\n");
    printf("捕获时间:\n");
    printf("%s", ctime((const time_t*) &protocol_header.ts.tv_sec));
    printf("数据包长度:\n");
    printf("%d\n", protocol_header.len);
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得数据包内容 */
    printf("以太网类型:\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    {
        case 0x0800:
            printf("上层协议是IP协议\n");
            break;
        case 0x0806:
            printf("上层协议是ARP协议\n");
            break;
        case 0x8035:
            printf("上层协议是RARP协议\n");
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
    printf("--------------------*****-----------------------\n");
    pcap_close(pcap_handle);
    /* 关闭winpcap操作 */
}
