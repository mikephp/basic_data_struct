/* �ļ����֣�get_ethernet2_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcapͷ�ļ�
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
/* ��̫��Э������ݽṹ */
{
    u_int8_t ether_dhost[6];
    /* Ŀ����̫����ַ */
    u_int8_t ether_shost[6];
    /* Դ��̫����ַ */
    u_int16_t ether_type;
    /* ��̫������ */
};
/*
=======================================================================================================================
����ĺ����ǻص��������书����ʵ�ֲ�����̫�����ݰ�������������ֶε����ݡ�ע�⣬���в���packet_content��ʾ�ľ��ǲ���
�����������ݰ����ݡ�����argument�ǴӺ���pcap_loop�������ݹ����ġ�����pcap_pkthdr��ʾ���񵽵����ݰ�������Ϣ������ʱ��
�����ȵ���Ϣ��
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* ��̫������ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э���ʽ */
    u_char *mac_string;
    /* ��̫����ַ */
    static int packet_number = 1;
    /* ��ʾ�������ݰ��ĸ��� */
    printf("**************************************************\n");
    printf("The %d Ethernet  packet is captured.\n", packet_number);
    printf("-----------    Ehternet Potocol (Link Layer)  ------------\n");
    printf("The %d Ethernet  packet is captured.\n", packet_number);
    ethernet_protocol = (struct ether_header*)packet_content;
    /* ��ȡ��̫��Э������ */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* ��ȡ��̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* ����̫�����ͽ����ж� */
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
        default:
            break;
    }
    printf("Mac Source Address is : \n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Դ��̫����ַ */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Ŀ����̫����ַ */
    printf("**************************************************\n");
    packet_number++; /* ���ݰ��������� */
}
/*
=======================================================================================================================
������
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* Libpcap��� */
    char error_content[PCAP_ERRBUF_SIZE];
    char *net_interface; /* ��·�ӿ� */
    struct bpf_program bpf_filter;
    /* ���˹��� */
    char bpf_filter_string[] = "ip";
    /* ���˹����ַ�������ʱ��ʾ������ֻ�ǲ���IPЭ������ݰ���ͬ��Ҳ����̫�����ݰ��� */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* �������ӿ� */
    pcap_lookupnet(net_interface,  /* ����ӿ� */ &net_ip,  /* �����ַ */ &net_mask,  /* �������� */error_content); /* ������Ϣ
     * */
    /* ��������ַ���������� */
    pcap_handle = pcap_open_live(net_interface,  /* ����ӿ� */
    BUFSIZ,  /* ���ݰ���С */
    1,  /* ����ģʽ */
    0,  /* �ȴ�ʱ�� */
    error_content); /* ������Ϣ */
    /* ������ӿ� */
    pcap_compile(pcap_handle,  /* Libpcap��� */
     &bpf_filter,  /* BPF���˹��� */
    bpf_filter_string,  /* ���˹����ַ��� */
    0,  /* �Ż����� */
    net_ip); /* �����ַ */
    /* ������˹��� */
    pcap_setfilter(pcap_handle,  /* Libpcap��� */ &bpf_filter); /* BPF���˹��� */
    /* ���ù��ɹ��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  /* Libpcap��� */
     - 1,  /* �������ݰ��ĸ������˴�Ϊ-1��ʾ���޲��� */
    ethernet_protocol_packet_callback,  /* �ص����� */
    NULL); /* ���ݸ��ص������Ĳ��� */
    /*
     * ����ѭ�������������ݰ���ע��ص�����ethernet_protocol_callback����������ÿ�����ݰ���Ҫ���ô˻ص��������в���
     */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
