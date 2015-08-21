/* �ļ����֣�get_arp_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
��������̫��Э���ʽ
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
/* ������IP��ַ��ʽ */
typedef u_int32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr; /* ���IP��ַ */
};
/*
-----------------------------------------------------------------------------------------------------------------------
������ARPЭ���ʽ
-----------------------------------------------------------------------------------------------------------------------
 */
struct arp_header
{
    u_int16_t arp_hardware_type;
    /* Ӳ����ַ���� */
    u_int16_t arp_protocol_type;
    /* Э���ַ���� */
    u_int8_t arp_hardware_length;
    /* Ӳ����ַ���� */
    u_int8_t arp_protocol_length;
    /* Э���ַ���� */
    u_int16_t arp_operation_code;
    /* �������� */
    u_int8_t arp_source_ethernet_address[6];
    /* Դ��̫����ַ */
    u_int8_t arp_source_ip_address[4];
    /* ԴIP��ַ */
    u_int8_t arp_destination_ethernet_address[6];
    /* Ŀ����̫����ַ */
    u_int8_t arp_destination_ip_address[4];
    /* Ŀ��IP��ַ */
};
/*
=======================================================================================================================
������ʵ��ARPЭ������ĺ�������
=======================================================================================================================
 */
void arp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct arp_header *arp_protocol;
    /* ARPЭ����� */
    u_short protocol_type;
    /* Э������ */
    u_short hardware_type;
    /* Ӳ������ */
    u_short operation_code;
    /* �������� */
    u_char *mac_string;
    /* ��̫����ַ */
    struct in_addr source_ip_address;
    /* ԴIP��ַ */
    struct in_addr destination_ip_address;
    /* Ŀ��IP��ַ */
    u_char hardware_length;
    /* Ӳ����ַ���� */
    u_char protocol_length;
    /* Э���ַ���� */
    printf("--------   ARP Protocol (Network Layer)    --------\n");
    arp_protocol = (struct arp_header*)(packet_content + 14);
    /*
     * ���ARPЭ�����ݡ�ע��������Ҫ������̫�����ݲ��֣����ĳ��ȸպ���14���������������14����ָ������14���ֽ�
     */
    hardware_type = ntohs(arp_protocol->arp_hardware_type);
    /* ���Ӳ������ */
    protocol_type = ntohs(arp_protocol->arp_protocol_type);
    /* ���Э������ */
    operation_code = ntohs(arp_protocol->arp_operation_code);
    /* ��ò����� */
    hardware_length = arp_protocol->arp_hardware_length;
    /* ���Ӳ����ַ���� */
    protocol_length = arp_protocol->arp_protocol_length;
    /* ���Э���ַ���� */
    printf("ARP Hardware Type:%d\n", hardware_type);
    printf("ARP Protocol Type:%d\n", protocol_type);
    printf("ARP Hardware Length:%d\n", hardware_length);
    printf("ARP Protocol Length:%d\n", protocol_length);
    printf("ARP Operation:%d\n", operation_code);
    switch (operation_code) /* ���ݲ���������ж���ARPʲô����Э�� */
    {
        case 1:
            printf("ARP Request Protocol\n");
            break;
            /* ��ARP��ѯЭ�� */
        case 2:
            printf("ARP Reply Protocol\n");
            break;
            /* ��ARPӦ��Э�� */
        case 3:
            printf("RARP Request Protocol\n");
            break;
            /* ��RARP��ѯЭ�� */
        case 4:
            printf("RARP Reply Protocol\n");
            break;
            /* ��RARPӦ��Э�� */
        default:
            break;
    }
    printf("Ethernet Source Address is : \n");
    mac_string = arp_protocol->arp_source_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Դ��̫����ַ */
    memcpy((void*) &source_ip_address, (void*) &arp_protocol->arp_source_ip_address, sizeof(struct in_addr));
    printf("Source IP Address:%s\n", inet_ntoa(source_ip_address));
    /* ���ԴIP��ַ */
    printf("Ethernet Destination Address is : \n");
    mac_string = arp_protocol->arp_destination_ethernet_address;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Ŀ����̫����ַ */
    memcpy((void*) &destination_ip_address, (void*) &arp_protocol->arp_destination_ip_address, sizeof(struct in_addr));
    printf("Destination IP Address:%s\n", inet_ntoa(destination_ip_address));
    /* ���Ŀ��IP��ַ */
}
/*
=======================================================================================================================
�����ǻص�������ʵ����̫��Э�����
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* ��̫������ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э����� */
    u_char *mac_string;
    /* ��̫����ַ */
    static int packet_number = 1;
    printf("**************************************************\n");
    printf("The %d  ARP  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* �����̫��Э������ */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* ������̫�������ж��ϲ�Э������ */
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
    switch (ethernet_type)
    {
        case 0x0806:
            arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /*
             * �����̫��������0x0806����ʾ�ϲ�Э����ARPЭ�飬Ӧ�õ��÷���ARPЭ��ĺ�����ע���ʱ�Ĳ������ݣ�ȫ���ǻص������Ĳ��������������ͬһ���������ݰ�������������ȷ��������ݰ�����̫��Э�鲿�֣��ַ������˴����ݰ���ARPЭ�鲿�֡�
             */
        default:
            break;
    }
    printf("**************************************************\n");
    packet_number++;
}
void main()
{
    pcap_t *pcap_handle;
    /* Libpcap��� */
    char error_content[PCAP_ERRBUF_SIZE];
    /* ������Ϣ */
    char *net_interface;
    /* ��·�ӿ� */
    struct bpf_program bpf_filter;
    /* bpf���˹��� */
    char bpf_filter_string[] = "arp";
    /* ���˹����ַ����������ʾ������ֻ�ǲ�������ARPЭ����������ݰ� */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
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
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* ע��ص�������ѭ���������ݰ� */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
