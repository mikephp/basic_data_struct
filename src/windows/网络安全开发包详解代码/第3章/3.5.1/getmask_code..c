#include <pcap.h>
/* Libpcap��ͷ�ļ���������� */
typedef u_int32_t in_addr_t;
struct in_addr /* ��ַ���� */
{
    in_addr_t s_addr; /* IP��ַ���� */
};
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* ������Ϣ */
    struct in_addr net_ip_address;
    /* �����ַ */
    struct in_addr net_mask_address;
    /* �����ַ */
    char *net_interface;
    /* �ӿ����� */
    char *net_ip_string;
    /* �����ַ�ַ�����ʽ */
    char *net_mask_string;
    /* �����ַ�ַ�����ʽ */
    u_int32_t net_ip;
    /* �����ַ */
    u_int32_t net_mask;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* ��ȡ����ӿ� */
    pcap_lookupnet(net_interface,  /* ����ӿ� */ &net_ip,  /* �����ַ */
     &net_mask,  /* �������� */error_content); /* ������Ϣ
     * */
    /* ��ȡ�����ַ�������ַ */
    printf("Network Interface is :%s\n", net_interface);
    /* ����ӿ����� */
    net_ip_address.s_addr = net_ip;
    net_ip_string = inet_ntoa(net_ip_address);
    /* �Ѷ����������ַ��ʽת��Ϊ�ַ�����ʽ */
    printf("Network IP Address is :%s\n", net_ip_string);
    /* ��������ַ */
    net_mask_address.s_addr = net_mask;
    net_mask_string = inet_ntoa(net_mask_address);
    /* �Ѷ����������ַ��ʽת��Ϊ�ַ�����ʽ */
    printf("Network Mask Address is:%s\n", net_mask_string);
    /* ��������ַ */
}
