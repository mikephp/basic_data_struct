#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件
=======================================================================================================================
 */
int main()
{
    eth_t *handle;
    /* 以太网协议操作句柄 */
    struct eth_hdr *eth;
    /* 以太网协议结构 */
    struct addr addr;
    /* 地址 */
    u_char content[ETH_LEN_MAX];
    /* 存放以太网协议数据 */
    int length;
    /* 长度 */
    length = 14;
    eth = (struct eth_hdr*)content;
    /* 构造以太网协议数据 */
    memset(eth, 0, sizeof(*eth));
    eth->eth_type = htons(ETH_TYPE_IP);
    /* 以太网协议类型 */
    addr_aton("11:11:11:11:11:11", &addr);
    /* 把源以太网地址转换为二进制形式 */
    memcpy(&eth->eth_src, &addr.addr_eth, ETH_ADDR_LEN);
    /* 以太网源地址 */
    addr_aton("22:22:22:22:22:22", &addr);
    /* 把目的以太网地址转换为二进制形式 */
    memcpy(&eth->eth_dst, &addr.addr_eth, ETH_ADDR_LEN);
    /* 以太网目的地址 */
    handle = eth_open("eth0");
    /* 打开以太网协议操作，接口名字为eth0 */
    eth_send(handle, content, length);
    /* 发送以太网协议数据 */
    eth_close(handle);
    /* 关闭以太网协议操作 */
}
