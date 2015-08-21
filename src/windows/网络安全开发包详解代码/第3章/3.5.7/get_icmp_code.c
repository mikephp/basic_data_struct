/* �ļ����֣�get_icmp_code.c */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
��������̫��Э���ʽ�Ķ���
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
/* ������IP��ַ��ʽ�Ķ��� */
typedef u_int32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};
/*
-----------------------------------------------------------------------------------------------------------------------
������IPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        u_int8_t ip_version: 4,
        /* �汾�� */
        ip_header_length: 4;
        /* �ײ����� */
    #else
        u_int8_t ip_header_length: 4,
        /* �ײ����� */
        ip_version: 4;
        /* �汾�� */
    #endif
    u_int8_t ip_tos;
    /* �������� */
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
-----------------------------------------------------------------------------------------------------------------------
������ICMPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct icmp_header
{
    u_int8_t icmp_type;
    /* ICMP���� */
    u_int8_t icmp_code;
    /* ICMP���� */
    u_int16_t icmp_checksum;
    /* У��� */
    u_int16_t icmp_id_lliiuuwweennttaaoo;
    /* ��ʶ�� */
    u_int16_t icmp_sequence;
    /* ���к� */
};
/*
=======================================================================================================================
������ʵ�ַ���ICMPЭ��ĺ�������
=======================================================================================================================
 */
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    /* ICMPЭ����� */
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    /* ��ȡICMPЭ���������ݣ�������̫����IPЭ�鲿�� */
    printf("----------  ICMP Protocol  (Transport Layer)  ----------\n");
    printf("ICMP Type:%d\n", icmp_protocol->icmp_type);
    /* ���ICMP���� */
    switch (icmp_protocol->icmp_type) /* ����ICMP���ͽ����ж� */
    {
        case 8:
            /* ����Ϊ8����ʾ�ǻ��������� */
            printf("ICMP Echo Request Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* ���ICMP���� */
            printf("Identifier:%d\n", icmp_protocol->icmp_id_lliiuuwweennttaaoo);
            /* ��ñ�ʶ�� */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* ������к� */
            break;
        case 0:
            /* ����Ϊ0����ʾ�ǻ���Ӧ���� */
            printf("ICMP Echo Reply Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* ���ICMP���� */
            printf("Identifier:%d\n", icmp_protocol->icmp_id_lliiuuwweennttaaoo);
            /* ��ñ�ʶ�� */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* ������к� */
            break;
        default:
            break;
            /* ����Ϊ����ֵ��������û�з��� */
    }
    printf("ICMP Checksum:%d\n", ntohs(icmp_protocol->icmp_checksum));
    /* ���У��� */
}

/*
=======================================================================================================================
������ʵ�ַ���IPЭ��ĺ�������
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct ip_header *ip_protocol;
    /* IPЭ����� */
    u_int header_length;
    /* �ײ����� */
    u_int offset;
    /* ƫ�� */
    u_char tos;
    /* �������� */
    u_int16_t checksum;
    /* У��� */
    printf("----------  IP Protocol  (Network Layer)  ----------\n");
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* ���IPЭ���������ݣ�������̫��Э�鲿�� */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* ���У��� */
    header_length = ip_protocol->ip_header_length *4;
    /* ����׶����� */
    tos = ip_protocol->ip_tos;
    /* ��÷������� */
    offset = ntohs(ip_protocol->ip_off);
    /* ���ƫ�� */
    printf("IP Version:%d\n", ip_protocol->ip_version);
    /* ��ð汾 */
    printf("Header length:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("Total length:%d\n", ntohs(ip_protocol->ip_length));
    /* ����ܳ��� */
    printf("Identification:%d\n", ntohs(ip_protocol->ip_id));
    printf("Offset:%d\n", (offset &0x1fff) *8);
    printf("TTL:%d\n", ip_protocol->ip_ttl);
    /* ���TTL */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* ���Э������ */
    switch (ip_protocol->ip_protocol) /* �ж�Э������ */
    {
        case 6:
            printf("The Transport Layer Protocol is TCP\n");
            break;
            /* �ϲ�Э��ΪTCPЭ�� */
        case 17:
            printf("The Transport Layer Protocol is UDP\n");
            break;
            /* �ϲ�Э��ΪUDPЭ�� */
        case 1:
            printf("The Transport Layer Protocol is ICMP\n");
            break;
            /* �ϲ�Э��ΪICMPЭ�� */
        default:
            break;
    }
    printf("Header checksum:%d\n", checksum);
    printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* ���ԴIP��ַ */
    printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* ���Ŀ��IP��ַ */
    switch (ip_protocol->ip_protocol)
    {
        case 1:
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /*
             * ����ϲ�Э��ΪICMPЭ�飬�͵��÷���ICMPЭ��ĺ�����ע���ʱ�Ĳ���������ʽ������ʾ�����Ķ�����ͬһ���������ݰ�
             */
        default:
            break;
    }
}

/*
=======================================================================================================================
������ʵ�ַ�����̫��Э��ĺ������壬Ҳ�ǻص�����
=======================================================================================================================
 */
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    u_short ethernet_type;
    /* ��̫������ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э�� */
    u_char *mac_string;
    /* ��̫����ַ */
    static int packet_number = 1;
    printf("**************************************************\n");
    printf("The %d  ICMP  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* �����̫��Э���������� */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type) /* ������̫�����ͽ����ж� */
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
            /* �ϲ�Э��ΪIPЭ�� */
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
            /* �ϲ�Э��ΪARPЭ�� */
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
            /* �ϲ�Э��ΪRARPЭ�� */
        default:
            break;
    }
    printf("Mac Source Address is : \n");
    mac_string = ethernet_protocol->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Դ��̫����ַ */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Ŀ����̫����ַ */
    switch (ethernet_type)
    {
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ����ϲ�Э����IPЭ�飬�͵��÷���IPЭ��ĺ��� */
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
    /* �洢������Ϣ */
    char *net_interface;
    /* ����ӿ� */
    struct bpf_program bpf_filter;
    /* BPF���˹��� */
    char bpf_filter_string[] = "icmp";
    /* ���˹����ַ�������ʱ��ʾ������ֻ����ICMP�������ݰ� */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* �������ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* ��������ַ���������� */
    pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);
    /* ������ӿ� */
    pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
    /* ������˹��� */
    pcap_setfilter(pcap_handle, &bpf_filter);
    /* ���ù��˹��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    pcap_loop(pcap_handle,  - 1, ethernet_protocol_packet_callback, NULL);
    /* ע��ص�������ѭ�������������ݰ���Ȼ����ûص��������������ݰ����д��� */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
