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
    struct icmp_hdr *icmp;
    /* ICMP协议的数据结构 */
    ip_t *handle;
    /* IP协议操作句柄 */
    u_char content[IP_LEN_MAX];
    /* ICMP数据的内容 */
    int length = 20+8;
    /* ICMP数据包的总长度，应该是IP协议头加ICMP协议 */
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
    /* 偏移量 */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL生存时间 */
    ip->ip_p = 1;
    /* IP上层协议类型，1代表ICMP协议 */
    ip->ip_sum = 0;
    /* 校验和 */
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
    icmp = (struct icmp_hdr*)(content + 20);
    /* 构造ICMP协议数据，应该跳过IP协议数据，所以应该加20字节数 */
    memset(icmp, 0, sizeof(*icmp));
    icmp->icmp_type = ICMP_ECHO;
    /*
     * ICMP类型，此时为ICMP_ECHO，表示回显请求ICMP数据包，如果要构造其他类型的ICMP数据包，就需要修改这里的类型。
     * icmp->icmp_code = 0;
     * /*ICMP代码，它跟ICMP类型一起决定ICMP数据的种类
     */
    ip_checksum(content, length);
    /* 计算校验和 */
    handle = ip_open();
    /* 打开IP协议操作 */
    ip_send(handle, content, length);
    /*
     * 发送IP协议数据包，注意在这里还是调用同一个函数来发送IP协议数据包，但此时IP的负载是一个ICMP协议，所以实际发送的是一个ICMP数据包，因为ICMP协议是基于IP协议的。
     */
    ip_close(handle);
    /* 关闭IP协议操作句柄 */
}
