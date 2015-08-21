#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ�
=======================================================================================================================
 */
int main()
{
    struct arp_hdr *arp;
    /* ARPЭ��ͷ���ݽṹ */
    struct arp_ethip *arp_ethernet_ip;
    /* ARP��������ݽṹ */
    eth_t *handle;
    /* ��̫��Э�������� */
    struct eth_hdr *ethernet;
    /* ��̫��Э���ʽ */
    struct addr addr;
    /* ��ַ */
    u_char content[ETH_LEN_MAX];
    /* ARP�����������ݣ�������̫��Э��ͷ��ARPЭ��ͷ�Լ�ARP�������� */
    int length;
    /* ���� */
    char *source_ethernet = "11:11:11:11:11:11";
    /* Դ��̫��Э���ַ */
    char *source_ip = "192.168.0.2";
    /* ԴIP��ַ */
    char *destination_ethernet = "22:22:22:22:22:22";
    /* Ŀ����̫��Э���ַ */
    char *destination_ip = "192.168.0.3";
    /* Ŀ��IP��ַ */
    srand(time(NULL));
    length = 14+28;
    /* ����Ӧ������̫�����ȼ���ARP�������ݰ��Ĳ��� */
    ethernet = (struct eth_hdr*)content;
    /* ������̫��ͷ���� */
    memset(ethernet, 0, sizeof(*ethernet));
    ethernet->eth_type = htons(ETH_TYPE_ARP);
    /* ��̫������ΪARP���� */
    addr_aton("11:11:11:11:11:11", &addr);
    /* ��Դ��̫����ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&ethernet->eth_src, &addr.addr_eth, ETH_ADDR_LEN);
    /* Դ��̫����ַ */
    addr_aton("22:22:22:22:22:22", &addr);
    /* ��Ŀ����̫����ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&ethernet->eth_dst, &addr.addr_eth, ETH_ADDR_LEN);
    /* Ŀ����̫����ַ */
    arp = (struct arp_hdr*)(content + 14);
    /* ����ARPЭ��ͷ���� */
    arp->ar_hrd = htons(ARP_HRD_ETH);
    /* Ӳ������ */
    arp->ar_pro = htons(ARP_PRO_IP);
    /* Э������ */
    arp->ar_hln = ETH_ADDR_LEN;
    /* Ӳ����ַ���� */
    arp->ar_pln = IP_ADDR_LEN;
    /* Э���ַ���� */
    arp->ar_op = ARP_OP_REQUEST;
    /* ARP�����룬��ʱ��ʾARP���� */
    arp_ethernet_ip = (struct arp_ethip*)(content + 14+ARP_HDR_LEN);
    /* ����ARP�������� */
    memset(arp_ethernet_ip, 0, sizeof(*arp_ethernet_ip));
    addr_aton(source_ethernet, &addr);
    /* ��Դ��̫��Э���ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&arp_ethernet_ip->ar_sha, &addr.addr_eth, ETH_ADDR_LEN);
    /* Դ��̫����ַ����ʱ��ARP�����е��������ݣ� ������̫��Э��ͷ�е����ݣ����涼����� */
    addr_aton(source_ip, &addr);
    /* ��ԴIP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&arp_ethernet_ip->ar_spa, &addr.addr_ip, IP_ADDR_LEN);
    /* ԴIP��ַ */
    addr_aton(destination_ethernet, &addr);
    /* ��Ŀ����̫����ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&arp_ethernet_ip->ar_tha, &addr.addr_eth, ETH_ADDR_LEN);
    /* Ŀ����̫����ַ */
    addr_aton(destination_ip, &addr);
    /* ��Ŀ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    memcpy(&arp_ethernet_ip->ar_tpa, &addr.addr_ip, IP_ADDR_LEN);
    /* Ŀ��IP��ַ */
    handle = eth_open("eth0");
    /* ����̫��Э��������ӿ�Ϊeth0 */
    eth_send(handle, content, length);
    /* ����ARP���ݰ������ǻ�����̫�����͵� */
    eth_close(handle);
    /* �ر���̫��Э����� */
}
