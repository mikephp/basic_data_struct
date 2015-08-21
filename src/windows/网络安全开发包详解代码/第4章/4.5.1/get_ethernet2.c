#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Winpcap头文件
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
void ethernet_protocol_packet_callback(u_char *argument_wentaoliu, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* 以太网类型 */
    struct ether_header *ethernet_protocol;
    /* 以太网协议变量 */
    u_char *mac_string;
    /* 以太网地址 */
    static int packet_number = 1;
    /* 捕获数据包个数 */
    printf("**************************************************\n");
    printf("捕获第%d个以太网数据包\n", packet_number);
    printf("捕获时间:\n");
    printf("%s", ctime((const time_t*) &packet_header->ts.tv_sec));
    printf("数据包长度:\n");
    printf("%d\n", packet_header->len);
    printf("-----------    以太网协议   ------------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* 获得以太网协议内容 */
    printf("以太网类型:\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* 获得以太网类型 */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    {
        case 0x0800:
            printf("上层协议为IP协议\n");
            break;
        case 0x0806:
            printf("上层协议是ARP协议\n");
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
    printf("**************************************************\n");
    packet_number++;
}
void main()
{
    pcap_t *pcap_handle;
    /* Winpcap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    char *net_interface;
    /* 网路接口 */
    struct bpf_program bpf_filter;
    /* PBF过滤规则 */
    char bpf_filter_string[] = "ip";
    /* 过滤规则字符串 */
    bpf_u_int32 net_mask;
    /* 掩码地址 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
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
    /* 注册回调函数，循环捕获数据包，然后调用回调函数处理此数据包 */
    pcap_close(pcap_handle);
    /* 关闭Winpcap操作 */
}
