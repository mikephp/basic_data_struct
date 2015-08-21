/* �ļ����֣�get_packet_code.c �����α�д */
#include "pcap.h"
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcap��ͷ�ļ� ;
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
������ARPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct arp_header
{
    u_int16_t arp_hardware_type;
    /* Ӳ������ */
    u_int16_t arp_protocol_type;
    /* Э������ */
    u_int8_t arp_hardware_length;
    /* Ӳ����ַ���� */
    u_int8_t arp_protocol_length;
    /* Э���ַ���� */
    u_int16_t arp_operation_code;
    /* ARP������ */
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
-----------------------------------------------------------------------------------------------------------------------
������IPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        u_int8_t ip_version: 4,
        /* �汾 */
        ip_header_length: 4;
        /* �ײ����� */
    #else
        u_int8_t ip_header_length: 4,
        /* �ײ����� */
        ip_version: 4;
        /* �汾 */
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
������UDPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header_liuwentao
{
    u_int16_t udp_source_port;
    /* Դ�˿ں� */
    u_int16_t udp_destination_port;
    /* Ŀ�Ķ˿ں� */
    u_int16_t udp_length;
    /* ���� */
    u_int16_t udp_checksum;
    /* У��� */
};
/*
-----------------------------------------------------------------------------------------------------------------------
������TCPЭ���ʽ�Ķ���
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
    u_int16_t tcp_source_port;
    /* Դ�˿ں� */
    u_int16_t tcp_destination_port;
    /* Ŀ�Ķ˿ں� */
    u_int32_t tcp_acknowledgement;
    /* ���к� */
    u_int32_t tcp_ack;
    /* ȷ���� */
    #ifdef WORDS_BIGENDIAN
        u_int8_t tcp_offset: 4,
        /* ƫ�� */
        tcp_reserved: 4;
        /* ���� */
    #else
        u_int8_t tcp_reserved: 4,
        /* ���� */
        tcp_offset: 4;
        /* ƫ�� */
    #endif
    u_int8_t tcp_flags;
    /* ��� */
    u_int16_t tcp_windows;
    /* ���ڴ�С */
    u_int16_t tcp_checksum;
    /* У��� */
    u_int16_t tcp_urgent_pointer;
    /* ����ָ�� */
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
    u_int16_t icmp_id;
    /* ��ʶ */
    u_int16_t icmp_sequence;
    /* ���к� */
};
/*
=======================================================================================================================
�����Ƿ���TCPЭ��ĺ�������
=======================================================================================================================
 */
void tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    /* ����TCPЭ����� */
    u_char flags;
    /* ��� */
    int header_length;
    /* �ײ����� */
    u_short source_port;
    /* Դ�˿ں� */
    u_short destination_port;
    /* Ŀ�Ķ˿ں� */
    u_short windows;
    /* ���� */
    u_short urgent_pointer;
    /* ����ָ�� */
    u_int sequence;
    /* ���к� */
    u_int acknowledgement;
    /* ȷ�Ϻ� */
    u_int16_t checksum;
    /* У��� */
    tcp_protocol = (struct tcp_header*)(packet_content + 14+20);
    /* ���TCPЭ���������ݣ�������̫��Э���IPЭ�鲿�� */
    source_port = ntohs(tcp_protocol->tcp_source_port);
    /* ���Դ�˿ں� */
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    /* ���Ŀ�Ķ˿ں� */
    header_length = tcp_protocol->tcp_offset *4;
    /* ����ײ����� */
    sequence = ntohl(tcp_protocol->tcp_acknowledgement);
    /* ������к� */
    acknowledgement = ntohl(tcp_protocol->tcp_ack);
    /* ���ȷ�Ϻ� */
    windows = ntohs(tcp_protocol->tcp_windows);
    /* ��ô��ڴ�С */
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    /* ��ý���ָ�� */
    flags = tcp_protocol->tcp_flags;
    /* ��ñ�� */
    checksum = ntohs(tcp_protocol->tcp_checksum);
    /* ���У��� */
    printf("-------  TCP Protocol  (Transport Layer)  -------\n");
    printf("Source Port:%d\n", source_port);
    printf("Destination Port:%d\n", destination_port);
    switch (destination_port)
    /* ���ݶ˿ں��ж�Ӧ�ò�Э������ */
    {
        case 80:
            printf("HTTP protocol\n");
            break;
            /* �ϲ�Э��ΪHTTPЭ�飬�����ڴ˵��÷���HTTPЭ��ĺ��������߿����Լ�����ʵ�� */
        case 21:
            printf("FTP protocol\n");
            break;
            /* �ϲ�Э��ΪFTPЭ�飬�����ڴ˵��÷���HTTPЭ��ĺ��� */
        case 23:
            printf("TELNET protocol\n");
            break;
            /* �ϲ�Э��ΪTELNETЭ�飬�����ڴ˵��÷���HTTPЭ��ĺ��� */
        case 25:
            printf("SMTP protocol\n");
            break;
            /* �ϲ�Э��ΪSMTPЭ�飬�����ڴ˵��÷���HTTPЭ��ĺ��� */
        case 110:
            printf("POP3 protocol\n");
            break;
            /* �ϲ�Э��ΪPOP3Э�飬�����ڴ˵��÷���HTTPЭ��ĺ��� */
        default:
            break; /* �����Ķ˿ں�������û�з��������߿����ڴ˷��������˿ںŴ����Ӧ�ò�Э�� */
    }
    printf("Sequence Number:%u\n", sequence);
    printf("Acknowledgement Number:%u\n", acknowledgement);
    printf("Header Length:%d\n", header_length);
    printf("Reserved:%d\n", tcp_protocol->tcp_reserved);
    printf("Flags:");
    /* �жϱ�ǵ����� */
    if (flags &0x08)
        printf("PSH ");
    if (flags &0x10)
        printf("ACK ");
    if (flags &0x02)
        printf("SYN ");
    if (flags &0x20)
        printf("URG ");
    if (flags &0x01)
        printf("FIN ");
    if (flags &0x04)
        printf("RST ");
    printf("\n");
    printf("Window Size:%d\n", windows);
    printf("Checksum:%d\n", checksum);
    printf("Urgent pointer:%d\n", urgent_pointer);
}
/*
=======================================================================================================================
������ʵ�ַ���UDPЭ��ĺ�������
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct udp_header_liuwentao *udp_protocol;
    /* UDPЭ����� */
    u_short source_port;
    /* Դ�˿ں� */
    u_short destination_port;
    /* Ŀ�Ķ˿ں� */
    u_short length;
    /* ���� */
    udp_protocol = (struct udp_header_liuwentao*)(packet_content + 14+20);
    /* ���UDPЭ���������ݣ�������̫��Э���IPЭ�鲿�� */
    source_port = ntohs(udp_protocol->udp_source_port);
    /* ���Դ�˿ں� */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* ���Ŀ�Ķ˿ں� */
    length = ntohs(udp_protocol->udp_length);
    /* ��ó��� */
    printf("----------  UDP Protocol  (Transport  Layer)  ----------\n");
    printf("Source port:%d\n", source_port);
    printf("Destination port:%d\n", destination_port);
    switch (destination_port)
    /* ���ݶ˿ں����ж�Ӧ�ò�Э������ */
    {
        case 138:
            printf("NETBIOS Datagram Service\n");
            break;
            /*
             * �˿ں���138����ʾ�ϲ�Э��ΪNETBIOS
             * ���ݱ������ڴ˿��Ե��÷�����Э��ĺ����������Լ���������ʵ�֡�
             */
        case 137:
            printf("NETBIOS Name Service\n");
            break;
            /* �˿ں���137����ʾ�ϲ�Э��ΪNETBIOS ���ַ����ڴ˿��Ե��÷�����Э��ĺ��� */
        case 139:
            printf("NETBIOS session service\n");
            break;
            /* �˿ں���139����ʾ�ϲ�Э��ΪNETBIOS �Ự�����ڴ˿��Ե��÷�����Э��ĺ����� */
        case 53:
            printf("name-domain server \n");
            break;
            /* �˿ں���53����ʾ�ϲ�Э��Ϊ���������ڴ˿��Ե��÷�����Э��ĺ����� */
        default:
            break; /* �����Ķ˿ں��ڴ�û�з��������߿����ڴ˽�һ������ */
    }
    printf("Length:%d\n", length);
    printf("Checksum:%d\n", ntohs(udp_protocol->udp_checksum));
    /* ���У��� */
}
/*
=======================================================================================================================
������ʵ�ַ���ICMPЭ��ĺ����Ķ���
=======================================================================================================================
 */
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    /* ICMPЭ����� */
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    /* ���ICMPЭ���������ݣ�������̫��Э���IPЭ�鲿�� */
    printf("----------  ICMP Protocol  (Transport Layer)  ----------\n");
    printf("ICMP Type:%d\n", icmp_protocol->icmp_type);
    /* ���ICMP���� */
    switch (icmp_protocol->icmp_type)
    /* ���������ж�ICMP���ݰ������� */
    {
        case 8:
            /* ������8����ʾ�ǻ�������ICMP���ݰ� */
            printf("ICMP Echo Request Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* ���ICMP���� */
            printf("Identifier:%d\n", icmp_protocol->icmp_id);
            /* ��ñ�ʶ�� */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* ������к� */
            break;
        case 0:
            /* ����Ϊ0����ʾ�ǻ���Ӧ��ICMP���ݰ� */
            printf("ICMP Echo Reply Protocol \n");
            printf("ICMP Code:%d\n", icmp_protocol->icmp_code);
            /* ���ICMP���� */
            printf("Identifier:%d\n", icmp_protocol->icmp_id);
            /* ��ñ�ʶ�� */
            printf("Sequence Number:%d\n", icmp_protocol->icmp_sequence);
            /* ������к� */
            break;
        default:
            break;
            /* �������͵�ICMP���ݰ��ڴ�û�з��������߿����ڴ�����������͵�ICMP���ݰ��ķ��� */
    }
    printf("ICMP Checksum:%d\n", ntohs(icmp_protocol->icmp_checksum));
    /* ���У��� */
}
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
    /* ������ */
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
    /* ���ARPЭ���������ݣ�������̫��Э�鲿�� */
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
    switch (operation_code) /* ���ݲ������ж�ARPЭ������ */
    {
        case 1:
            printf("ARP Request Protocol\n");
            break;
            /* ������Ϊ1����ʾ��ARP����Э�� */
        case 2:
            printf("ARP Reply Protocol\n");
            break;
            /* ������Ϊ2����ʾ��ARPӦ��Э�� */
        case 3:
            printf("RARP Request Protocol\n");
            break;
            /* ������Ϊ3����ʾ��RARP����Э�� */
        case 4:
            printf("RARP Reply Protocol\n");
            break;
            /* ������Ϊ4����ʾ��RARPӦ��Э�� */
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
    ip_protocol = (struct ip_header*)(packet_content + 14);
    /* ���IPЭ���������ݣ�������̫��Э�鲿�� */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* ���У��� */
    header_length = ip_protocol->ip_header_length *4;
    /* ���IP�ײ����� */
    tos = ip_protocol->ip_tos;
    /* ��÷������� */
    offset = ntohs(ip_protocol->ip_off);
    /* �ƫ�� */
    printf("----------- IP Protocol  (Network Layer)  -----------\n");
    printf("IP Version:%d\n", ip_protocol->ip_version);
    /* ��ð汾�� */
    printf("Header length:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("Total length:%d\n", ntohs(ip_protocol->ip_length));
    /* ����ܳ��� */
    printf("Identification:%d\n", ntohs(ip_protocol->ip_id));
    /* ��ñ�ʶ */
    printf("Offset:%d\n", (offset &0x1fff) *8);
    printf("TTL:%d\n", ip_protocol->ip_ttl);
    /* �������ʱ�� */
    printf("Protocol:%d\n", ip_protocol->ip_protocol);
    /* ���Э������ */
    switch (ip_protocol->ip_protocol)
    /* ����Э�������ж��ϲ�Э������ */
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
    /* У��� */
    printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    /* ���ԴIP��ַ */
    printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* ���Ŀ��IP��ַ */
    switch (ip_protocol->ip_protocol)
    /* �ж��ϲ�Э�����ͣ�Ȼ�������Ӧ�ĺ������з��� */
    {
        case 6:
             /* �ϲ�Э��ΪTCPЭ�� */
            tcp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ���÷���TCPЭ��ĺ�����ע������Ĵ��ݣ���ʾ��������ͬһ���������ݰ� */
        case 17:
             /* �ϲ�Э��ΪUDPЭ�� */
            udp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ���÷���UDPЭ��ĺ�����ע������Ĵ��ݷ�ʽ */
        case 1:
             /* �ϲ�Э��ΪICMPЭ�� */
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ���÷���ICMPЭ��ĺ�����ע������Ĵ��� */
        default:
            break;
            /* �������͵�Э���ڴ�û�з��������߿����ڴ˽�һ���������������Э�� */
    }
}
/*
=======================================================================================================================
�����Ƿ�����̫��Э��ĺ������壬ͬʱ��Ҳ�ǻص�����
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
    printf("The %d  packet is captured.\n", packet_number);
    printf("--------   Ehternet Protocol (Link Layer)    --------\n");
    ethernet_protocol = (struct ether_header*)packet_content;
    /* �����̫��Э���������� */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫������ */
    printf("%04x\n", ethernet_type);
    switch (ethernet_type)
    /* ������̫�������ֶ��ж��ϲ�Э������ */
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
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Դ��̫����ַ */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ���Ŀ����̫����ַ */
    switch (ethernet_type)
    /* ������̫�������ֶ��ж��ϲ�Э�飬Ȼ�������Ӧ�������з��� */
    {
        case 0x0806:
             /* �ϲ���ARPЭ�� */arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ���÷���ARPЭ��ĺ��� */
        case 0x0800:
             /* �ϲ���IPЭ�� */ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
            /* ���÷���IPЭ��ĺ��� */
        default:
            break; /* ������Э���ڴ�û�з��������߿������Ž�һ������ */
    }
    printf("**************************************************\n");
    packet_number++;
}
/*
=======================================================================================================================
�������������Ķ���
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* libpap��� */
    char error_content[PCAP_ERRBUF_SIZE];
    /* �洢�������� */
    char *net_interface;
    /* ����ӿ� */
    struct bpf_program bpf_filter;
    /* BPF���˹��� */
    char bpf_filter_string[] = "";
    /* ���˹����ַ�������ʱΪ�յģ���ʾ�������е��������ݰ��������ǲ����ض����������ݰ� */
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
    /* ע��ص�������ѭ�������������ݰ���Ȼ����ûص������Բ�����������ݰ����з��� */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
