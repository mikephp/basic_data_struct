#include "pcap.h"
int main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* 错误信息 */
    struct pcap_pkthdr protocol_header;
    /* 数据包头 */
    pcap_t *pcap_handle;
    /* Libpcap句柄 */
    struct bpf_program bpf_filter;
    /* bpf过滤规则 */
    char bpf_filter_string[] = "";
    /* 过滤规则 */
    const u_char *packet_content;
    /* 数据包内容 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    char *net_interface;
    /* 网络接口 */
    net_interface = pcap_lookupdev(error_content);
    /* 获取网络接口 */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* 获取网络地址和掩码地址 */
    pcap_handle = pcap_open_live(net_interface,  /* 网络接口 */
    BUFSIZ,  /* 数据包大小 */
    1,  /* 混杂模式 */
    0,  /* 等待时间 */
    error_content); /* 错误信息 */
    /* 打开网络接口 */
    pcap_compile(pcap_handle,  /* Libpcap句柄 */
     &bpf_filter,  /* BPF过滤规则 */
    bpf_filter_string,  /* BPF过滤规则字符串 */
    0,  /* 优化参数 */
    net_ip); /* 网络地址 */
    /* 编译过滤规则 */
    pcap_setfilter(pcap_handle,  /* Libpcap句柄 */ &bpf_filter); /* BPF过滤规则 */
    /* 设置过滤规则 */
    packet_content = pcap_next(pcap_handle,  /* Libpcap句柄 */ &protocol_header); /* 数据包信息 */
    /* 捕获一个数据包，返回此数据包的内容 */
    printf("Capture a packet from : %s\n", net_interface);
    /* 输出网络接口名字 */
    printf("The packet length is :%d\n", protocol_header.len);
    /* 输出捕获的数据包的长度 */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
