#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ� ;
���涨��ĺ�����һ���ص����������ں���arp_loop()�����б����ã����Ĺ��ܾ��Ƕ�ARP��¼���в���������������ʾARP��¼
=======================================================================================================================
 */
int show_arp_cache(const struct arp_entry *object, void *p)
{
    printf("IP %s at MAC %s\n ", addr_ntoa(&object->arp_pa), addr_ntoa(&object->arp_ha));
    /* ��ʾIP��ַ����̫����ַ�� */
    return (0);
}
int main()
{
    struct arp_entry arp_object;
    /* ARP��������ݽṹ�ı��� */
    arp_t *arp_handle;
    /* ARP���������� */
    arp_handle = arp_open();
    /* ��ARP������� */
    printf("show the arp cache\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /*
     * ע��ص�����show_arp_cache��������ѭ�����ûص�������ÿ��ARP�����¼���в������˴�����ʾÿ��ARP�����¼��
     */
    printf("Add a arp \n");
    addr_pton("192.168.0.4", &arp_object.arp_pa);
    /* ��һ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    addr_pton("11:11:11:11:11:11", &arp_object.arp_ha);
    /* ��һ����̫����ַ���ַ�����ʽת��Ϊ��������ʽ */
    arp_add(arp_handle, &arp_object);
    /* ����һ��ARP�����¼ */
    printf("a arp added\n");
    addr_pton("192.168.0.5", &arp_object.arp_pa);
    /* ��һ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    addr_pton("22:22:22:22:22:22", &arp_object.arp_ha);
    /* ��һ����̫����ַ���ַ�����ʽת��Ϊ��������ʽ */
    arp_add(arp_handle, &arp_object);
    /* ����һ��ARP�����¼ */
    printf("a arp added\n");
    printf("show the arp cache again\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /* ��ʾÿ��ARP�����¼ */
    printf("Delete a arp \n");
    addr_pton("192.168.0.4", &arp_object.arp_pa);
    /* ��һ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    arp_delete(arp_handle, &arp_object);
    /* ɾ��һ��ARP�����¼ */
    printf("a arp deleted\n");
    printf("show the arp cache again\n");
    arp_loop(arp_handle, show_arp_cache, NULL);
    /* ��ʾÿ��ARP�����¼ */
    arp_close(arp_handle);
    /* �ر�ARP������� */
}
