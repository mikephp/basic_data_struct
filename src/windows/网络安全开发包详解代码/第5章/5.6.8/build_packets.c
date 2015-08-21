#include <win32/libnet.h>
void main()
{
    libnet_t *l = NULL;
    /* libnet句柄 */
    libnet_ptag_t protocol_tag;
    /* 协议标记 */
    char *payload = NULL;
    /* 负载 */
    u_short payload_length = 0;
    /* 负载长度 */
    char *device = "\\Device\\NPF_{5973DFB0-AD5B-4065-A22B-C61FFB64624F}";
    /*
     * 网络设备接口，在这里用的是Windows下的一个接口，根据各种机子的类型而定，可以通过很多方法获得，例如使用windump或ngrep程序来获得即可
     */
    char *destination_ip_str = "192.168.0.3";
    /* 目的IP地址字符串 */
    char *source_ip_str = "192.168.0.2";
    /* 源IP地址字符串 */
    u_long source_ip = 0;
    /* 源IP地址 */
    u_long destination_ip = 0;
    /* 目的IP地址 */
    char errbuf[LIBNET_ERRBUF_SIZE];
    /* 错误信息 */
    char label[LIBNET_LABEL_SIZE];
    /* libnet句柄字符串标记 */
    int j = 0;
    /* 用于标记第几个数据包 */
    int i = 0;
    /* 用于循环 */
    int packet_length;
    /* 发送的数据包的长度 */
    int number = 5;
    /* 发送数据包的个数 */
    for (i = 0; i < number; i++)
    /*
     * 循环构造5个数据包，每个数据包的libnet句柄字符串标记不同，也就是说用唯一的一个libnet句柄字符串标记来指示一个构造的数据包
     */
    {
        l = libnet_init( /* 初始化libnet */LIBNET_RAW4,  /* libnet类型，为原始套接字IPv4类型 */device,  /* 网络设备接口 */errbuf /* 错误信息 */
        );
        source_ip = libnet_name2addr4(l, source_ip_str, LIBNET_RESOLVE);
        /* 把源IP地址字符串形式转化为网络字节顺序的数据 */
        destination_ip = libnet_name2addr4(l, destination_ip_str, LIBNET_RESOLVE);
        /* 把目的IP地址字符串形式转化为网络字节顺序的数据 */
        protocol_tag = libnet_build_icmpv4_echo(
        /* 构造ICMP回显数据包 */
        ICMP_ECHO,
        /* 类型，此时为回显请求 */
        0,
        /* 代码，应该为0 */
        0,
        /* 校验和，为0，表示由libnet句柄自动计算 */
        123,
        /* 标识符，赋值为123，自己任意填写数值 */
        456,
        /* 序列号，赋值为245，自己任意填写数值 */
        NULL,
        /* 负载，赋值为空 */
        0,
        /* 负载的长度，赋值为0 */
        l,
        /* libnet句柄，应该是由libnet_init()函数得到的 */
        0
        /* 协议块标记，赋值为0，表示构造一个新的协议块 */
        );
        protocol_tag = libnet_build_ipv4(
        /* 构造IP协议块 */
        LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_length,
        /* IP协议块的长度 */
        0,
        /* 服务质量，这里赋值为0 */
        10,
        /* 标识符，这里赋值为10 */
        0,
        /* 偏移，这里赋值为0 */
        20,
        /* 生存时间，这里赋值为20 */
        IPPROTO_ICMP,
        /* 上层协议类型，这里是ICMP协议 */
        0,
        /* 校验和，这里为0表示由libnet计算校验和 */
        source_ip,
        /* 源IP地址 */
        destination_ip,
        /* 目的IP地址 */
        payload,
        /* 负载 */
        payload_length,
        /* 负载的长度 */
        l,
        /* libnet句柄 */
        0
        /* 协议块标记，为0表示构造一个新的IP协议块 */
        );
        snprintf(label, sizeof(label) - 1, "test %d", i);
        /* 构造一个libnet句柄字符串标识，用来指示一个libnet句柄 */
        libnet_cq_add(l, label);
        /*
         * 插入一个libnet句柄结点到libnet句柄队列中，并且是作为第一个结点，此句柄结点由label字符串来唯一标识
         */
    }
    for (l = libnet_cq_head(); libnet_cq_last(); l = libnet_cq_next())
    /* 循环遍历libnet句柄队列中所有的libnet句柄，然后发送 */
    {
        j++;
        /* 第几个libnet句柄 */
        packet_length = libnet_write(l);
        /* 发送由libnet句柄l表示的数据包 */
        printf("发送第 %d 个%d字节长度的ICMP回显请求数据包，其对应的libnet句柄为\"%s\"\n", j, packet_length, libnet_cq_getlabel(l));
        /* 输出发送的数据包信息 */
    }
    libnet_cq_destroy();
    /* 销毁libnet句柄队列， 同时销毁了所有在libnet句柄队列中的每个libnet句柄 */
}
