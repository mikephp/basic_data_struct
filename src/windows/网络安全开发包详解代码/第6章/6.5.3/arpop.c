#include "dnet.h"
/*
=======================================================================================================================
Libdnet头文件 ;
下面定义的函数是一个回调函数，它在函数arp_loop()函数中被调用，它的功能就是对ARP记录进行操作，在这里是显示ARP记录
=======================================================================================================================
 */
int show_arp_cache(const struct arp_entry *object, void *p)
{
    printf("IP %s at MAC %s\n ", addr_ntoa(&object->arp_pa), addr_ntoa(&object->arp_ha));
    /* 显示IP地址和以太网地址对 */
    return (0);
}
int main()
{
    struct arp_entry arp_object;
    /* ARP缓存的数据结构的变量 */
    arp_t *arp_handle;
    /* ARP缓存操作句柄 */
    arp_handle = arp_open();
    /* 打开ARP缓存操作 */
    printf("show the arp cache\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /*
     * 注册回调函数show_arp_cache，作用是循环调用回调函数对每个ARP缓存记录进行操作。此处是显示每个ARP缓存记录。
     */
    printf("Add a arp \n");
    addr_pton("192.168.0.4", &arp_object.arp_pa);
    /* 把一个IP地址的字符串形式转化为二进制形式 */
    addr_pton("11:11:11:11:11:11", &arp_object.arp_ha);
    /* 把一个以太网地址的字符串形式转换为二进制形式 */
    arp_add(arp_handle, &arp_object);
    /* 增加一个ARP缓存记录 */
    printf("a arp added\n");
    addr_pton("192.168.0.5", &arp_object.arp_pa);
    /* 把一个IP地址的字符串形式转化为二进制形式 */
    addr_pton("22:22:22:22:22:22", &arp_object.arp_ha);
    /* 把一个以太网地址的字符串形式转换为二进制形式 */
    arp_add(arp_handle, &arp_object);
    /* 增加一个ARP缓存记录 */
    printf("a arp added\n");
    printf("show the arp cache again\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /* 显示每个ARP缓存记录 */
    printf("Delete a arp \n");
    addr_pton("192.168.0.4", &arp_object.arp_pa);
    /* 把一个IP地址的字符串形式转化为二进制形式 */
    arp_delete(arp_handle, &arp_object);
    /* 删除一个ARP缓存记录 */
    printf("a arp deleted\n");
    printf("show the arp cache again\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /* 显示每个ARP缓存记录 */
    arp_close(arp_handle);
    /* 关闭ARP缓存操作 */
}
