#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Winpcapͷ�ļ�
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
    u_int8_t ether_dhost[6];
    /* Ŀ����̫����ַ */
    u_int8_t ether_shost[6];
    /* Դ��̫����ַ */
    u_int16_t ether_type;
    /* ��̫������ */
};
void ethernet_protocol_packet_callback(u_char *argument_wentaoliu, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* ��̫������ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э����� */
    u_char *mac_string;
    /* ��̫����ַ */
    static int packet_number = 1;
    /* �������ݰ����� */
    printf("**************************************************\n");
    printf("�����%d����̫�����ݰ�\n", packet_number);
    printf("����ʱ��:\n");
    printf("%s", ctime((const time_t*) &packet_header->ts.tv_sec));
    printf("���ݰ�����:\n");
    printf("%d\n", packet_header->len);
    printf("-----------    ��̫��Э��   ------------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* �����̫��Э������ */
    printf("��̫������:\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    {
        case 0x0800:
            printf("�ϲ�Э��ΪIPЭ��\n");
            break;
        case 0x0806:
            printf("�ϲ�Э����ARPЭ��\n");
            break;
        case 0x8035:
            printf("�ϲ�Э��ΪRARPЭ��\n");
            break;
        default:
            break;
    }
    printf("Դ��̫����ַ: \n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Դ��̫����ַ */
    printf("Ŀ����̫����ַ: \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Ŀ����̫����ַ */
    printf("**************************************************\n");
    packet_number++;
}
void main()
{
    pcap_t *pcap_handle;
    /* Winpcap��� */
    char error_content[PCAP_ERRBUF_SIZE];
    /* �洢������Ϣ */
    char *net_interface;
    /* ��·�ӿ� */
    struct bpf_program bpf_filter;
    /* PBF���˹��� */
    char bpf_filter_string[] = "ip";
    /* ���˹����ַ��� */
    bpf_u_int32 net_mask;
    /* �����ַ */
    bpf_u_int32 net_ip;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* ��ÿ��õ�����ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* ��������ַ�������ַ */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 1, error_content);
    /* ����·�ӿ� */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* ����BPF���˹��� */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* ���ù��˹��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* ע��ص�������ѭ���������ݰ���Ȼ����ûص�������������ݰ� */
    pcap_close(pcap_handle);
    /* �ر�Winpcap���� */
}
