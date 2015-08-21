#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ� ;
����ĺ�����һ���ص����������ں���intf_loop�����е��á����Ĺ����Ƕ�ÿ������ӿڽ��в����������������ÿ������ӿڵ���Ϣ
=======================================================================================================================
 */
int show_interface_content(const struct intf_entry *object, void *p)
{
    int number;
    printf("network interface name is :");
    printf("%s\n", object->intf_name);
    /* �������ӿڵ����� */
    printf("flags: %d\n", object->intf_flags);
    /* �������ӿڲ�����־ */
    printf("flags including:\n");
    if (object->intf_flags &INTF_FLAG_UP)
     /* �ж��Ƿ��Ǽ���״̬��UP */
        printf("UP\n");
    if (object->intf_flags &INTF_FLAG_LOOPBACK)
     /* �ж��Ƿ��ǻ�· */
        printf("LOOPBACK\n");
    if (object->intf_flags &INTF_FLAG_POINTOPOINT)
     /* �ж��Ƿ�֧�ֵ�Ե�Э�� */
        printf("POINTOPOINT\n");
    if (object->intf_flags &INTF_FLAG_NOARP)
     /* �ж��Ƿ�֧��ARPЭ�� */
        printf("NOARP\n");
    if (object->intf_flags &INTF_FLAG_BROADCAST)
     /* �ж��Ƿ�֧�ֹ㲥 */
        printf("BROADCAST\n");
    if (object->intf_flags &INTF_FLAG_MULTICAST)
     /* �ж��Ƿ�֧�ֶಥ */
        printf("MULTICAST\n");
    if (object->intf_mtu != 0)
        printf("mtu: %d\n", object->intf_mtu);
    /* ���MTUֵ */
    if (object->intf_addr.addr_type == ADDR_TYPE_IP)
    {
        if (object->intf_dst_addr.addr_type == ADDR_TYPE_IP)
        {
            printf("address:%s,desination:%s\n", addr_ntoa(&object->intf_addr), addr_ntoa(&object->intf_dst_addr));
            /* ���Դ��ַ��Ŀ�ĵ�ַ */
        }
        else
            printf("address:%s\n", addr_ntoa(&object->intf_addr));
        /* ���Դ��ַ */
    }
    if (object->intf_link_addr.addr_type == ADDR_TYPE_ETH)
        printf("Link Address: %s\n", addr_ntoa(&object->intf_link_addr));
    /* ���Ӳ����ַ */
    for (number = 0; number < object->intf_alias_num; number++)
        printf("Alias:%s\n", addr_ntoa(&object->intf_alias_addrs[number]));
    /* ������б��� */
    printf("-----------------------------------------------------------\n");
    return (0);
}
int main()
{
    struct intf_entry *entry;
    /* ����ӿڵ����ݽṹ�ı��� */
    intf_t *handle;
    /* ����ӿڲ������ */
    struct addr *interface_address;
    /* ��ַ */
    char content[1024];
    /* ���� */
    struct addr address;
    /* ��ַ */
    entry = (struct intf_entry*)content;
    /* ����ӿ���Ϣ */
    memset(entry, 0, sizeof(*entry));
    entry->intf_len = sizeof(content);
    /* ���� */
    handle = intf_open();
    /* �򿪽ӿڲ��� */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ע��ص�����show_interface_content��ѭ�����ûص������������ÿ������ӿڵ���Ϣ */
    printf("get the special network interface:\n");
    strlcpy(entry->intf_name, "eth0", sizeof(entry->intf_name));
    /* ָ������ӿ�����eth0 */
    intf_get(handle, entry);
    /* ��������ӿڵ����ֻ�ȡ����ӿ� */
    show_interface_content(entry, NULL);
    /* ��ʾ��õ���·�ӿڵ���Ϣ */
    printf("show the network interface about the IP :\n");
    addr_aton("192.168.0.3", &address);
    /* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
    intf_get_src(handle, entry, &address);
    /* ���ݵ�ַ��ȡ�ض�������ӿ� */
    show_interface_content(entry, NULL);
    /* ��ʾ������ӿڵ���Ϣ */
    printf("show the network interface about the destination IP:\n");
    addr_aton("192.168.0.4", &address);
    /* ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    intf_get_dst(handle, entry, &address);
    /* ����Ŀ�ĵ�ַ��ȡ���Ӧ������ӿ� */
    show_interface_content(entry, NULL);
    /* ��ʾ��õ���·�ӿڵ���Ϣ */
    strlcpy(entry->intf_name, "eth0", sizeof(entry->intf_name));
    /* ָ������ӿ�����Ϊeth0 */
    printf("set the network interface unactive:\n");
    entry->intf_flags &= ~INTF_FLAG_UP;
    /* ��������ӿڵĲ���ΪʧЧ״̬ */
    intf_set(handle, entry);
    /* ����ָ��������ӿ�ΪʧЧ״̬ */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    printf("set the network interface active:\n");
    entry->intf_flags |= INTF_FLAG_UP;
    /* ��������ӿڲ���Ϊ����״̬ */
    intf_set(handle, entry);
    /* ����ָ��������ӿ�Ϊ����״̬ */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    printf("set the network interface can't use ARP protocol :\n");
    entry->intf_flags |= INTF_FLAG_NOARP;
    /* ��������ӿڲ���Ϊ��֧��ARPЭ�� */
    intf_set(handle, entry);
    /* ����ָ��������ӿڲ�֧��ARPЭ�� */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ���е�����ӿڵ���Ϣ */
    printf("set the network interface can use ARP protocol :\n");
    entry->intf_flags &= ~INTF_FLAG_NOARP;
    /* ��������ӿڲ���Ϊ��֧��ARPЭ�� */
    intf_set(handle, entry);
    /* �����ض�������ӿڲ�֧��ARPЭ�� */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ���е�����ӿ���Ϣ */
    printf("set the network interface alias:\n");
    interface_address = &entry->intf_alias_addrs[entry->intf_alias_num++];
    /* ����ӿڵı��� */
    addr_pton("192.168.0.6", interface_address);
    /* ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    intf_set(handle, entry);
    /* ��������ӿڵı���Ϊ"192.168.0.6"�� */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    printf("set the network interface destination:\n");
    interface_address = &entry->intf_dst_addr;
    /* Ŀ�ĵ�ַ */
    addr_pton("192.168.0.7", interface_address);
    /* ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    intf_set(handle, entry);
    /* ��������ӿڵ�Ŀ�ĵ�ַΪ192.168.0.7 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    printf("set the network interface address:\n");
    interface_address = &entry->intf_addr;
    /* ����ӿڵĵ�ַ */
    addr_pton("192.168.0.8", interface_address);
    /* ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    intf_set(handle, entry);
    /* ��������ӿڵĵ�ַΪ192.168.0.8 */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    printf("set the network interface link:\n");
    interface_address = &entry->intf_link_addr;
    /* Ӳ����ַ */
    addr_pton("192.168.0.9", interface_address);
    /* ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    intf_set(handle, entry);
    /* ��������ӿڵ�Ӳ����ַΪ192.168.0.8��Ӳ����ַ */
    printf("show the network interface:\n");
    intf_loop(handle, show_interface_content, NULL);
    /* ��ʾ��������ӿڵ���Ϣ */
    intf_close(handle);
    /* �ر�����ӿڲ��� */
}
