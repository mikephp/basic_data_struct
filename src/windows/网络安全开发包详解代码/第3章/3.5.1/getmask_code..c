#include <pcap.h>
/* Libpcap的头文件，必须包含 */
typedef u_int32_t in_addr_t;
struct in_addr /* 地址类型 */
{
    in_addr_t s_addr; /* IP地址类型 */
};
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* 出错信息 */
    struct in_addr net_ip_address;
    /* 网络地址 */
    struct in_addr net_mask_address;
    /* 掩码地址 */
    char *net_interface;
    /* 接口名字 */
    char *net_ip_string;
    /* 网络地址字符串形式 */
    char *net_mask_string;
    /* 掩码地址字符串形式 */
    u_int32_t net_ip;
    /* 网络地址 */
    u_int32_t net_mask;
    /* 掩码地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获取网络接口 */
    pcap_lookupnet(net_interface,  /* 网络接口 */ &net_ip,  /* 网络地址 */
     &net_mask,  /* 网络掩码 */error_content); /* 错误信息
     * */
    /* 获取网络地址和掩码地址 */
    printf("Network Interface is :%s\n", net_interface);
    /* 输出接口名字 */
    net_ip_address.s_addr = net_ip;
    net_ip_string = inet_ntoa(net_ip_address);
    /* 把二进制网络地址格式转换为字符串形式 */
    printf("Network IP Address is :%s\n", net_ip_string);
    /* 输出网络地址 */
    net_mask_address.s_addr = net_mask;
    net_mask_string = inet_ntoa(net_mask_address);
    /* 把二进制掩码地址格式转换为字符串形式 */
    printf("Network Mask Address is:%s\n", net_mask_string);
    /* 输出掩码地址 */
}
