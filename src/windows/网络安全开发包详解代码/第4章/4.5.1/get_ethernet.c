#include <pcap.h>
/*
-----------------------------------------------------------------------------------------------------------------------
WinPcapͷ�ļ� ;
��������̫��Э���ʽ
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
    u_int8_t ether_dhost[6];
    /* ��̫��Ŀ�ĵ�ַ */
    u_int8_t ether_shost[6];
    /* Դ��̫����ַ */
    u_int16_t ether_type;
    /* ��̫������ */
};
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* �洢������Ϣ */
    pcap_t *pcap_handle;
    /* winpcap��� */
    const u_char *packet_content;
    /* ���ݰ����� */
    u_char *mac_string;
    /* ��̫����ַ */
    u_short ethernet_type;
    /* ��̫������ */
    bpf_u_int32 net_mask;
    /* �����ַ */
    bpf_u_int32 net_ip;
    /* �����ַ */
    char *net_interface;
    /* ����ӿ� */
    struct pcap_pkthdr protocol_header;
    /* ���ݰ�ͷ����Ϣ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э����� */
    struct bpf_program bpf_filter;
    /* BPF���˹��� */
    char bpf_filter_string[] = "ip";
    /* ���˹����ַ��� */
    net_interface = pcap_lookupdev(error_content);
    /* �������ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* ��������ַ���������� */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);
    /* ����·�ӿ� */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* ������˹��� */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* ���ù��˹��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    packet_content = pcap_next(pcap_handle, &protocol_header);
    /* ����һ�����ݰ������ݰ����ݷ��ظ�packet_content */
    printf("--------------------*****-----------------------\n");
    printf("����һ���������ݰ�\n");
    printf("����ʱ��:\n");
    printf("%s", ctime((const time_t*) &protocol_header.ts.tv_sec));
    printf("���ݰ�����:\n");
    printf("%d\n", protocol_header.len);
    ethernet_protocol = (struct ether_header*)packet_content;
    /* ������ݰ����� */
    printf("��̫������:\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    {
        case 0x0800:
            printf("�ϲ�Э����IPЭ��\n");
            break;
        case 0x0806:
            printf("�ϲ�Э����ARPЭ��\n");
            break;
        case 0x8035:
            printf("�ϲ�Э����RARPЭ��\n");
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
    printf("--------------------*****-----------------------\n");
    pcap_close(pcap_handle);
    /* �ر�winpcap���� */
}
