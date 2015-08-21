#include <win32/libnet.h>
/*
=======================================================================================================================
包含的头文件
=======================================================================================================================
 */
void main()
{
    int packet_size;
    /* 构造的数据包大小 */
    libnet_t *l;
    /* libnet句柄 */
    char *device = NULL;
    /* 设备变量 */
    char *destination_ip_str = "192.168.0.2";
    /* 目的IP地址字符串变量，可以指定任意一个合法的IP地址 */
    char *source_ip_str = "192.168.0.3";
    /* 源IP地址字符串变量，可以指定任意一个合法的IP地址 */
    u_char hardware_source[6] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    };
    /* 源硬件地址，可以指定任意的MAC地址 */
    u_char hardware_destination[6] =
    {
        0x06, 0x05, 0x04, 0x03, 0x02, 0x01
    };
    /* 目的硬件地址，可以指定任意的MAC地址 */
    u_long destination_ip;
    /* 目的IP地址 */
    u_long source_ip;
    /* 源IP地址 */
    char error_buffer[LIBNET_ERRBUF_SIZE];
    /* 存放错误信息 */
    libnet_ptag_t ip_protocol_tag = 0;
    /* IP协议块的协议标记 */
    libnet_ptag_t ethernet_protocol_tag = 0;
    /* 以太网协议块的协议标记 */
    u_short protocol = IPPROTO_UDP;
    /* 表示IP上一层的协议类型，在这里是UDP协议 */
    u_char payload[255] = "";
    /* 负载的内容，初始值为空 */
    u_long payload_size = 0;
    /*
     * 负载的长度，初始值为0 ;
     * 对libnet进行初始化操作
     */
    l = libnet_init(LIBNET_LINK,  /* libnet类型 */device,  /* 网络设备 */error_buffer);
    /* 错误信息 */
    destination_ip = libnet_name2addr4(l, destination_ip_str, LIBNET_RESOLVE);
    /* 把目的IP地址字符串类型转换为网络顺序的数值 */
    source_ip = libnet_name2addr4(l, source_ip_str, LIBNET_RESOLVE);
    /* 把源IP地址字符串类型转换为网络顺序的数值 */
    strncpy(payload, "test", sizeof(payload) - 1);
    /* 构造负载的内容，此时的内容为"test"，只是测试 */
    payload_size = strlen(payload);
    /* 计算负载内容的长度 */
    ip_protocol_tag = libnet_build_ipv4(
    /* 构造IP协议块 */
    LIBNET_IPV4_H + payload_size,
    /* IP协议块的整个长度，在这里是IP协议头加上负载的长度 */
    0,
    /* 服务质量，在这里为0 */
    242,
    /* IP标记 */
    0,
    /* IP紧急指针 */
    64,
    /* 生存时间 */
    protocol,
    /* 上层协议类型 */
    0,
    /* 校验和，此时为0，表示由libnet自动计算 */
    source_ip,
    /* 源IP地址 */
    destination_ip,
    /* 目的IP地址 */
    payload,
    /* 负载内容 */
    payload_size,  /* 负载内容的大小 */l,
    /* libnet句柄，由libnet_init()函数生成的 */
    ip_protocol_tag /* IP协议块标记，此时为0，表示构造新的IP协议块 */
    );
    ethernet_protocol_tag = libnet_build_ethernet(
    /* 构造一个以太网协议块 */
    hardware_destination,
    /* 以太网目的地址 */
    hardware_source,
    /* 以太网源地址 */
    ETHERTYPE_IP,
    /* 上层协议类型，在这里必须是IP协议 */
    NULL,
    /* 负载，这里为空 */
    0,
    /* 负载大小 */
    l,
    /* libnet句柄 */
    ethernet_protocol_tag /* 以太网协议块标记，此时为0，表示构造新的以太网协议块
     * */
    );
    packet_size = libnet_write(l);
    /* 发送数据包 */
    printf("发送一个%d字节的IP数据包\n", packet_size);
    libnet_destroy(l);
    /* 销毁libnet */
}
