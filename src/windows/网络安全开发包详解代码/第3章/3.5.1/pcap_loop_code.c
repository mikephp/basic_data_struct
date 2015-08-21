/* 文件名字：pcap_loop_code.c */
#include "pcap.h"
/*
=======================================================================================================================
Libpcap头文件 ;
下面是回调函数的定义，在此处实现的功能是记录捕获到的网络数据包的个数
=======================================================================================================================
 */
void packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    static int packet_number = 1;
    /* 静态局部变量，用来捕获到的存放数据包的个数 */
    printf("The %d packet is captured.\n", packet_number);
    /* 输出捕获数据包的个数 */
    packet_number++;
    /* 个数增加 */
} void main()
{
    pcap_t *pcap_handle;
    /* Libpcap句柄 */
    char error_content[PCAP_ERRBUF_SIZE];
    /* 用来存储错误信息 */
    char *net_interface;
    /* 用来存储网络接口 */
    struct bpf_program bpf_filter;
    /* bpf过滤规则 */
    char bpf_filter_string[] = "ip";
    /* 过滤规则字符串形式 */
    bpf_u_int32 net_mask;
    /* 网络掩码 */
    bpf_u_int32 net_ip;
    /* 网络地址 */
    net_interface = pcap_lookupdev(error_content);
    /* 获取网路接口 */
    pcap_lookupnet(net_interface,  /* 网络接口 */ &net_ip,  /* 网络地址 */ &net_mask,  /* 网络掩码 */error_content); /* 错误信息
     * */
    /* 获取网络地址和掩码 */
    pcap_handle = pcap_open_live(net_interface,  /* 网络接口 */
    BUFSIZ,  /* 数据包大小 */
    1,  /* 混杂模式 */
    0,  /* 等待时间 */
    error_content); /* 错误信息 */
    /* 打开网络接口 */
    pcap_compile(pcap_handle,  /* 网络接口 */
     &bpf_filter,  /* BPF过滤规则 */
    bpf_filter_string,  /* 过滤规则字符串 */
    0,  /* 优化参数 */
    net_ip); /* 网络地址 */
    /* 编译过滤规则 */
    pcap_setfilter(pcap_handle,  /* Libpcap句柄 */ &bpf_filter); /* BPF过滤规则 */
    /* 设置过滤规则 */
    pcap_loop(pcap_handle,  /* Libpcap句柄 */10,  /* 捕获数据包的个数 */packet_callback,  /* 回调函数 */NULL); /* 传递给回调函数的参数
     * */
    /*
     * 注册回调函数packet_callback（），然后循环捕获网络数据包，每捕获一个数据包就调用回调函数进行处理。在这里捕获数据包的个数是10，所以当捕获到10个网络数据包的时候就退出循环。如果个数设为-1，就表示无限循环。
     */
    pcap_close(pcap_handle);
    /* 关闭Libpcap操作 */
}
