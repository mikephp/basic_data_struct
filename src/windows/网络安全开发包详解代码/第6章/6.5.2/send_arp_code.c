#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件
=======================================================================================================================
 */
int main()
{
    struct arp_hdr *arp;
    /* ARP协议头数据结构 */
    struct arp_ethip *arp_ethernet_ip;
    /* ARP请求的数据结构 */
    eth_t *handle;
    /* 以太网协议操作句柄 */
    struct eth_hdr *ethernet;
    /* 以太网协议格式 */
    struct addr addr;
    /* 地址 */
    u_char content[ETH_LEN_MAX];
    /* ARP请求数据内容，包括以太网协议头和ARP协议头以及ARP请求数据 */
    int length;
    /* 长度 */
    char *source_ethernet = "11:11:11:11:11:11";
    /* 源以太网协议地址 */
    char *source_ip = "192.168.0.2";
    /* 源IP地址 */
    char *destination_ethernet = "22:22:22:22:22:22";
    /* 目的以太网协议地址 */
    char *destination_ip = "192.168.0.3";
    /* 目的IP地址 */
    srand(time(NULL));
    length = 14+28;
    /* 产度应该是以太网长度加上ARP请求数据包的产度 */
    ethernet = (struct eth_hdr*)content;
    /* 构造以太网头数据 */
    memset(ethernet, 0, sizeof(*ethernet));
    ethernet->eth_type = htons(ETH_TYPE_ARP);
    /* 以太网类型为ARP类型 */
    addr_aton("11:11:11:11:11:11", &addr);
    /* 把源以太网地址的字符串形式转化为二进制形式 */
    memcpy(&ethernet->eth_src, &addr.addr_eth, ETH_ADDR_LEN);
    /* 源以太网地址 */
    addr_aton("22:22:22:22:22:22", &addr);
    /* 把目的以太网地址的字符串形式转化为二进制形式 */
    memcpy(&ethernet->eth_dst, &addr.addr_eth, ETH_ADDR_LEN);
    /* 目的以太网地址 */
    arp = (struct arp_hdr*)(content + 14);
    /* 构造ARP协议头数据 */
    arp->ar_hrd = htons(ARP_HRD_ETH);
    /* 硬件类型 */
    arp->ar_pro = htons(ARP_PRO_IP);
    /* 协议类型 */
    arp->ar_hln = ETH_ADDR_LEN;
    /* 硬件地址长度 */
    arp->ar_pln = IP_ADDR_LEN;
    /* 协议地址长度 */
    arp->ar_op = ARP_OP_REQUEST;
    /* ARP操作码，此时表示ARP请求 */
    arp_ethernet_ip = (struct arp_ethip*)(content + 14+ARP_HDR_LEN);
    /* 构造ARP请求数据 */
    memset(arp_ethernet_ip, 0, sizeof(*arp_ethernet_ip));
    addr_aton(source_ethernet, &addr);
    /* 把源以太网协议地址的字符串格式转化为二进制形式 */
    memcpy(&arp_ethernet_ip->ar_sha, &addr.addr_eth, ETH_ADDR_LEN);
    /* 源以太网地址，此时是ARP请求中的数据内容， 不是以太网协议头中的内容，下面都是如此 */
    addr_aton(source_ip, &addr);
    /* 把源IP地址的字符串形式转化为二进制形式 */
    memcpy(&arp_ethernet_ip->ar_spa, &addr.addr_ip, IP_ADDR_LEN);
    /* 源IP地址 */
    addr_aton(destination_ethernet, &addr);
    /* 把目的以太网地址的字符串形式转换为二进制形式 */
    memcpy(&arp_ethernet_ip->ar_tha, &addr.addr_eth, ETH_ADDR_LEN);
    /* 目的以太网地址 */
    addr_aton(destination_ip, &addr);
    /* 把目的IP地址的字符串形式转化为二进制形式 */
    memcpy(&arp_ethernet_ip->ar_tpa, &addr.addr_ip, IP_ADDR_LEN);
    /* 目的IP地址 */
    handle = eth_open("eth0");
    /* 打开以太网协议操作，接口为eth0 */
    eth_send(handle, content, length);
    /* 发送ARP数据包，它是基于以太网类型的 */
    eth_close(handle);
    /* 关闭以太网协议操作 */
}
