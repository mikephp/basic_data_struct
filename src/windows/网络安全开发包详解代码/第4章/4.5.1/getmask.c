#include <pcap.h>
/*
=======================================================================================================================
WinPcap头文件
=======================================================================================================================
 */
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* 存储错误信息 */
    struct in_addr net_ip_address;
    /* 网络地址 */
    struct in_addr net_mask_address;
    /* 掩码地址 */
    char *net_interface;
    /* 网络接口 */
    char *net_ip_string;
    /* 网络地址 */
    char *net_mask_string;
    /* 网络掩码 */
    u_int32_t net_ip;
    u_int32_t net_mask;
    net_interface = pcap_lookupdev(error_content);
    /* 获得可用的网络接口 */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* 获得网络接口的网络地址和网络掩码 */
    net_ip_address.s_addr = net_ip;
    net_ip_string = inet_ntoa(net_ip_address);
    /* 获得网络地址 */
    printf("网络地址:%s\n", net_ip_string);
    net_mask_address.s_addr = net_mask;
    net_mask_string = inet_ntoa(net_mask_address);
    /* 获得掩码地址 */
    printf("网络掩码:%s\n", net_mask_string);
}
