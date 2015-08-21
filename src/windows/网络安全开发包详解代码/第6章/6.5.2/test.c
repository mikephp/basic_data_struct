#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件
=======================================================================================================================
 */
int main()
{
    struct ip_hdr *ip;
    /* IP协议的数据结构 */
    ip_t *handle;
    /* IP协议操作句柄 */
    u_char buf[IP_LEN_MAX];
    /* 缓存，用来存储IP协议数据 */
    int length = 20;
    /* IP协议数据长度 */
    struct addr addr;
    /* 地址 */
    char *destination_ip = "192.168.0.3";
    /* IP目的地址 */
    char *source_ip = "192.168.0.2";
    /* IP源地址 */
    ip = (struct ip_hdr*)buf;
    /* 把buf表示的缓存数据转换为IP的协议格式 */
    ip->ip_hl = 5;
    /* IP首部长度 */
    ip->ip_v = 4;
    /* IP版本号 */
    ip->ip_tos = 0;
    /* IP的TOS */
    ip->ip_id = 0x11;
    /* IP标识符号 */
    ip->ip_off = 0;
    /* 偏移量 */
    ip->ip_ttl = IP_TTL_MAX;
    /* IP生存时间 */
    ip->ip_p = 6;
    /* IP上层协议类型 */
    ip->ip_sum = 0;
    /* 校验和 */
    addr_aton(source_ip, &addr);
    /* 把源IP地址的字符串格式转换为二进制形式 */
    ip->ip_src = addr.addr_ip;
    /* 源IP地址 */
    addr_aton(destination_ip, &addr);
    /* 把目的IP地址的字符串格式转换为二进制形式 */
    ip->ip_dst = addr.addr_ip;
    /* 目的IP地址 */
    ip->ip_len = htons(length);
    /* 长度 */
    ip_checksum(buf, length);
    /* 计算校验和 */
    handle = ip_open();
    /* 打开IP协议操作，返回IP协议操作句柄 */
    ip_send(handle, buf, length);
    /* 发送IP协议数据 */
    ip_close(handle);
    /* 关闭IP协议操作 */
}
