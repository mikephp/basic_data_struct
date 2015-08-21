/* �ļ����֣�get_ip_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcapͷ�ļ� ;
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
    in_addr_t s_addr;
};
/*
-----------------------------------------------------------------------------------------------------------------------
������IPЭ���ʽ
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #ifdef WORDS_BIGENDIAN
        u_int8_t ip_version: 4,  /* IPЭ��汾 */
        ip_header_length: 4; /* IPЭ���ײ����� */
    #else
        u_int8_t ip_header_length: 4, ip_version: 4;
    #endif
    u_int8_t ip_tos;
    /* TOS�������� */
    u_int16_t ip_length;
    /* �ܳ��� */
    u_int16_t ip_id;
    /* ��ʶ */
    u_int16_t ip_off;
    /* ƫ�� */
    u_int8_t ip_ttl;
    /* ����ʱ�� */
    u_int8_t ip_protocol;
    /* Э������ */
    u_int16_t ip_checksum;
    /* У��� */
    struct in_addr ip_souce_address;
    /* ԴIP��ַ */
    struct in_addr ip_destination_address;
    /* Ŀ��IP��ַ */
};
/*
=======================================================================================================================
������ʵ��IP���ݰ������ĺ�������
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct ip_header *ip_protocol;
    /* IPЭ����� */
    u_int header_length;
    /* ���� */
    u_int offset;
    /* ƫ�� */
    u_char tos;
    /* �������� */
    u_int16_t checksum;
    /* У��� */
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* ���IPЭ���������ݣ�ȥ����̫��ͷ */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* ���У��� */
    header_length = ip_protocol->ip_header_length *4;
    /* ��ó��� */
    tos = ip_protocol->ip_tos;
    /* ���TOS */
    offset = ntohs(ip_protocol->ip_off);
    /* ���ƫ���� */
    printf("----------- IP Protocol  (Network Layer)  -----------\n");
    printf("IP Version:%d\n", ip_protocol->ip_version);
    printf("Header length:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("Total length:%d\n", ntohs(ip_protocol->ip_length));
    /* ����ܳ��� */
    printf("Identification:%d\n", ntohs(ip_protocol->ip_id));
    /* ��ñ�ʶ */
    printf("Offset:%d\n", (offset &0x1fff) *8);
    printf("TTL:%d\n", ip_protocol->ip_ttl);
    /* ���TTL */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* ���Э������ */
    switch (ip_protocol->ip_protocol) /* �ж�Э�����͵�ֵ */
    {
        case 6:
            printf("The Transport Layer Protocol is TCP\n");
            break;
            /* ���Э������Ϊ6����ʾ�ϲ�Э��ΪTCPЭ�� */
        case 17:
            printf("The Transport Layer Protocol is UDP\n");
            break;
            /* ���Э������Ϊ17����ʾ�ϲ�Э��ΪUDPЭ�� */
        case 1:
            printf("The Transport Layer Protocol is ICMP\n");
            break;
            /* ���Э������Ϊ1����ʾ���ϲ�Э��ΪICMPЭ�� */
        default:
            break;
    }
    printf("Header checksum:%d\n", checksum);
    printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* ���ԴIP��ַ */
    printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* ���Ŀ��IP��ַ */
}
/*
=======================================================================================================================
�����ǻص�������ʵ�ֲ���ͷ�����̫��Э��
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* ��̫������ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э����� */
    u_char *mac_string;
    static int packet_number = 1;
    printf("**************************************************\n");
    printf("The %d IP  packet is captured.\n", packet_number);
    printf("-------------   Ehternet Protocol (Link Layer)      ------------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* �����̫��Э���������� */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* �ж���̫�����͵�ֵ */
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
            /* ���Ϊ0x0800����ʾ�ϲ�Э��ΪIPЭ�� */
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
            /* ���Ϊ0x0806����ʾ�ϲ�Э��ΪARPЭ�� */
        case 0x8035:
            printf("The network layer is RARP potocol\n");
            break;
            /* ���Ϊ0x8035����ʾ�ϲ�Э��ΪRARPЭ�� */
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
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /*
             * ����ϲ�Э����IPЭ�飬�͵��÷���IPЭ��ĺ�����IPЭ����з�����ע�⣬��ʱ���ݵĲ��������ص������Ĳ�����һ���ģ���ʾ����ͬһ�����ݰ�
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
    /* ����ӿ� */
    struct bpf_program bpf_filter;
    /* bpf���˹��� */
    char bpf_filter_string[] = "ip";
    /* ���˹����ַ�������ʱ��ʾ������ֻ�ǲ���IPЭ����������ݰ� */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* �������ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* �����·��ַ������ */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);
    /* ������ӿ� */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* ����BPF���˹��� */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* ���ù��˹��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* ע��ص�������ѭ���������ݰ� */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
