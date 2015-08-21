#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件 ;
下面的函数是一个回调函数，其在函数intf_loop（）中调用。它的功能是对每个网络接口进行操作，在这里是输出每个网络接口的信息
=======================================================================================================================
 */
int show_interface_content(const struct intf_entry *object, void *p)
{
    int number;
    printf("network interface name is :");
    printf("%s\n", object->intf_name);
    /* 输出网络接口的名字 */
    printf("flags: %d\n", object->intf_flags);
    /* 输出网络接口参数标志 */
    printf("flags including:\n");
    if (object->intf_flags &INTF_FLAG_UP)
     /* 判断是否是激活状态，UP */
        printf("UP\n");
    if (object->intf_flags &INTF_FLAG_LOOPBACK)
     /* 判断是否是回路 */
        printf("LOOPBACK\n");
    if (object->intf_flags &INTF_FLAG_POINTOPOINT)
     /* 判断是否支持点对点协议 */
        printf("POINTOPOINT\n");
    if (object->intf_flags &INTF_FLAG_NOARP)
     /* 判断是否不支持ARP协议 */
        printf("NOARP\n");
    if (object->intf_flags &INTF_FLAG_BROADCAST)
     /* 判断是否支持广播 */
        printf("BROADCAST\n");
    if (object->intf_flags &INTF_FLAG_MULTICAST)
     /* 判断是否支持多播 */
        printf("MULTICAST\n");
    if (object->intf_mtu != 0)
        printf("mtu: %d\n", object->intf_mtu);
    /* 输出MTU值 */
    if (object->intf_addr.addr_type == ADDR_TYPE_IP)
    {
        if (object->intf_dst_addr.addr_type == ADDR_TYPE_IP)
        {
            printf("address:%s,desination:%s\n", addr_ntoa(&object->intf_addr), addr_ntoa(&object->intf_dst_addr));
            /* 输出源地址和目的地址 */
        }
        else
            printf("address:%s\n", addr_ntoa(&object->intf_addr));
        /* 输出源地址 */
    }
    if (object->intf_link_addr.addr_type == ADDR_TYPE_ETH)
        printf("Link Address: %s\n", addr_ntoa(&object->intf_link_addr));
    /* 输出硬件地址 */
    for (number = 0; number < object->intf_alias_num; number++)
        printf("Alias:%s\n", addr_ntoa(&object->intf_alias_addrs[number]));
    /* 输出所有别名 */
    printf("-----------------------------------------------------------\n");
    return (0);
}
int main()
{
    struct intf_entry *entry;
    /* 网络接口的数据结构的变量 */
    intf_t *handle;
    /* 网络接口操作句柄 */
    struct addr *interface_address;
    /* 地址 */
    char content[1024];
    /* 内容 */
    struct addr address;
    /* 地址 */
    entry = (struct intf_entry*)content;
    /* 构造接口信息 */
    memset(entry, 0, sizeof(*entry));
    entry->intf_len = sizeof(content);
    /* 长度 */
    handle = intf_open();
    /* 打开接口操作 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 注册回调函数show_interface_content，循环调用回调函数依次输出每个网络接口的信息 */
    printf("get the special network interface:\n");
    strlcpy(entry->intf_name, "eth0", sizeof(entry->intf_name));
    /* 指定网络接口名字eth0 */
    intf_get(handle, entry);
    /* 根据网络接口的名字获取网络接口 */
    show_interface_content(entry, NULL);
    /* 显示获得的网路接口的信息 */
    printf("show the network interface about the IP :\n");
    addr_aton("192.168.0.3", &address);
    /* 把字符串形式的IP地址转换为二进制形式的IP地址 */
    intf_get_src(handle, entry, &address);
    /* 根据地址获取特定的网络接口 */
    show_interface_content(entry, NULL);
    /* 显示此网络接口的信息 */
    printf("show the network interface about the destination IP:\n");
    addr_aton("192.168.0.4", &address);
    /* 把IP地址的字符串形式转换为二进制形式 */
    intf_get_dst(handle, entry, &address);
    /* 根据目的地址获取相对应的网络接口 */
    show_interface_content(entry, NULL);
    /* 显示获得的网路接口的信息 */
    strlcpy(entry->intf_name, "eth0", sizeof(entry->intf_name));
    /* 指定网络接口名字为eth0 */
    printf("set the network interface unactive:\n");
    entry->intf_flags &= ~INTF_FLAG_UP;
    /* 设置网络接口的参数为失效状态 */
    intf_set(handle, entry);
    /* 设置指定的网络接口为失效状态 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    printf("set the network interface active:\n");
    entry->intf_flags |= INTF_FLAG_UP;
    /* 设置网络接口参数为激活状态 */
    intf_set(handle, entry);
    /* 设置指定的网络接口为激活状态 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    printf("set the network interface can't use ARP protocol :\n");
    entry->intf_flags |= INTF_FLAG_NOARP;
    /* 设置网络接口参数为不支持ARP协议 */
    intf_set(handle, entry);
    /* 设置指定的网络接口不支持ARP协议 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有的网络接口的信息 */
    printf("set the network interface can use ARP protocol :\n");
    entry->intf_flags &= ~INTF_FLAG_NOARP;
    /* 设置网络接口参数为不支持ARP协议 */
    intf_set(handle, entry);
    /* 设置特定的网络接口不支持ARP协议 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有的网络接口信息 */
    printf("set the network interface alias:\n");
    interface_address = &entry->intf_alias_addrs[entry->intf_alias_num++];
    /* 网络接口的别名 */
    addr_pton("192.168.0.6", interface_address);
    /* 把IP地址的字符串形式转换为二进制形式 */
    intf_set(handle, entry);
    /* 设置网络接口的别名为"192.168.0.6"。 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    printf("set the network interface destination:\n");
    interface_address = &entry->intf_dst_addr;
    /* 目的地址 */
    addr_pton("192.168.0.7", interface_address);
    /* 把IP地址的字符串形式转换为二进制形式 */
    intf_set(handle, entry);
    /* 设置网络接口的目的地址为192.168.0.7 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    printf("set the network interface address:\n");
    interface_address = &entry->intf_addr;
    /* 网络接口的地址 */
    addr_pton("192.168.0.8", interface_address);
    /* 把IP地址的字符串形式转换为二进制形式 */
    intf_set(handle, entry);
    /* 设置网络接口的地址为192.168.0.8 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    printf("set the network interface link:\n");
    interface_address = &entry->intf_link_addr;
    /* 硬件地址 */
    addr_pton("192.168.0.9", interface_address);
    /* 把IP地址的字符串形式转换为二进制形式 */
    intf_set(handle, entry);
    /* 设置网络接口的硬件地址为192.168.0.8的硬件地址 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* 显示所有网络接口的信息 */
    intf_close(handle);
    /* 关闭网络接口操作 */
}
