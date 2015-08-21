#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件
=======================================================================================================================
 */
int main()
{
    struct ip_hdr *ip;
    /* IP协议头的数据结构 */
    struct tcp_hdr *tcp;
    /* TCP协议头的数据结构 */
    ip_t *handle;
    /* IP协议操作句柄 */
    u_char content[IP_LEN_MAX];
    /* TCP数据包内容，包括IP协议头内容 */
    int length = 20+20;
    /* 整个长度，包括IP协议头长度和TCP协议头长度 */
    struct addr addr;
    /* 地址 */
    char *destination_ip = "192.168.0.3";
    /* 目的IP地址 */
    char *source_ip = "192.168.0.2";
    /* 源IP地址 */
    ip = (struct ip_hdr*)content;
    /* 构造IP协议头 */
    ip->ip_hl = 5;
    /* IP长度 */
    ip->ip_v = 4;
    /* IP版本号 */
    ip->ip_tos = 0;
    /* 服务质量 */
    ip->ip_id = 0x11;
    /* IP标识 */
    ip->ip_off = 0;
    /* IP偏移量 */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL生存时间 */
    ip->ip_p = 6;
    /* IP上层协议类型，6代表TCP协议 */
    ip->ip_sum = 0;
    /* 计算校验和 */
    addr_aton(source_ip, &addr);
    /* 把源IP地址的字符串形式转换为二进制形式 */
    ip->ip_src = addr.addr_ip;
    /* 源IP地址 */
    addr_aton(destination_ip, &addr);
    /* 把目的IP地址的字符串形式转换为二进制形式 */
    ip->ip_dst = addr.addr_ip;
    /* 目的IP地址 */
    ip->ip_len = htons(length);
    /* 长度 */
    tcp = (struct tcp_hdr*)(content + 20);
    /* 构造TCP协议数据，跳过IP协议数据，所以应该加20 */
    memset(tcp, 0, sizeof(*tcp));
    tcp->th_sport = htons(81);
    /* 源端口号 */
    tcp->th_dport = htons(91);
    /* 目的端口号 */
    tcp->th_seq = htonl(111);
    /* 序列号 */
    tcp->th_ack = 0;
    /* 确认号 */
    tcp->th_off = 5;
    /* 偏移 */
    tcp->th_flags = TH_SYN;
    /* 控制标记 */
    tcp->th_win = TCP_WIN_MAX;
    /* 窗口大小 */
    tcp->th_urp = 0;
    /* 紧急指针 */
    ip_checksum(content, length);
    /* 计算机校验和 */
    handle = ip_open();
    /* 打开IP协议操作 */
    ip_send(handle, content, length);
    /* 发送TCP数据包，它是基于原始套接字IP进行构造的 */
    ip_close(handle);
    /* 关闭IP协议操作 */
}
