#include <pcap.h>
/*
=======================================================================================================================
WinPcapͷ�ļ�
=======================================================================================================================
 */
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* �洢������Ϣ */
    struct in_addr net_ip_address;
    /* �����ַ */
    struct in_addr net_mask_address;
    /* �����ַ */
    char *net_interface;
    /* ����ӿ� */
    char *net_ip_string;
    /* �����ַ */
    char *net_mask_string;
    /* �������� */
    u_int32_t net_ip;
    u_int32_t net_mask;
    net_interface = pcap_lookupdev(error_content);
    /* ��ÿ��õ�����ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* �������ӿڵ������ַ���������� */
    net_ip_address.s_addr = net_ip;
    net_ip_string = inet_ntoa(net_ip_address);
    /* ��������ַ */
    printf("�����ַ:%s\n", net_ip_string);
    net_mask_address.s_addr = net_mask;
    net_mask_string = inet_ntoa(net_mask_address);
    /* ��������ַ */
    printf("��������:%s\n", net_mask_string);
}
