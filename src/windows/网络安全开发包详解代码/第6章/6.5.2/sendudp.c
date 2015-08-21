#include "dnet.h"
int main()
{
    struct ip_hdr *ip;
    /* IP协议头的数据结构 */
    struct udp_hdr *udp;
    /* UDP协议头的数据结构 */
    ip_t *handle;
    /* IP协议操作句柄 */
    u_char content[IP_LEN_MAX];
    /* UDP协议数据内容，其中还包括IP协议头内容 */
    int length = 20+8;
    /* 数据包长度，包括IP协议头长度和UDP协议长度 */
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
    ip->ip_id = htons(11);
    /* IP标识 */
    ip->ip_off = 0;
    /* 偏移量 */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL生存时间 */
    ip->ip_p = 17;
    /* IP上层协议类型 */
    ip->ip_sum = 0;
    /* 校验和 */
    addr_aton(source_ip, &addr);
    /* 源IP地址的字符串形式转换为二进制形式 */
    ip->ip_src = addr.addr_ip;
    /* 源IP地址 */
    addr_aton(destination_ip, &addr);
    /* 把目的IP地址的字符串形式转换为二进制形式 */
    ip->ip_dst = addr.addr_ip;
    /* 目的IP地址 */
    ip->ip_len = htons(length);
    /* IP长度 */
    ip_checksum(content, length);
    /* 计算校验和 */
    udp = (struct udp_hdr*)(content + 20);
    /* 构造UDP协议数据，跳过IP协议头，所以应该加20 */
    memset(udp, 0, sizeof(*udp));
    udp->uh_sport = htons(81);
    /* 源端口号 */
    udp->uh_dport = htons(91);
    /* 目的端口号 */
    udp->uh_ulen = htons(8);
    /* 长度 */
    handle = ip_open();
    /* 打开IP协议操作 */
    ip_send(handle, content, length);
    /* 发送UDP协议数据，它也是基于原始套接字IP类型的 */
    ip_close(handle);
    /* 关闭IP协议操作 */
}
