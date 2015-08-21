#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
UDPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header
{
    unsigned short udp_source_port;
    unsigned short udp_destination_port;
    unsigned short udp_length;
    unsigned short udp_checksum;
};
/*
-----------------------------------------------------------------------------------------------------------------------
ICMPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct icmp_header
{
    unsigned int icmp_type;
    unsigned int icmp_code;
    unsigned char icmp_checksum;
    unsigned char icmp_id;
    unsigned char icmp_sequence;
};
/*
-----------------------------------------------------------------------------------------------------------------------
IPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
    #if defined(WORDS_BIGENDIAN)
        unsigned char ip_version: 4,  /* �汾 */
        ip_header_length: 4; /* �ײ����� */
    #else
        unsigned char ip_header_length: 4, ip_version: 4;
    #endif
    unsigned char ip_tos; /* �������� */
    unsigned short ip_length; /* �ܳ��� */
    unsigned short ip_id; /* ��ʶ */
    unsigned short ip_off; /* ��־��ƫ�� */
    unsigned char ip_ttl; /* ����ʱ�� */
    unsigned char ip_protocol; /* Э������ */
    unsigned short ip_checksum; /* У��� */
    struct in_addr ip_souce_address; /* ԴIP��ַ */
    struct in_addr ip_destination_address; /* Ŀ��IP��ַ */
};
/*
-----------------------------------------------------------------------------------------------------------------------
TCPЭ���ײ�
-----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
    unsigned char tcp_source_port; /* Դ�˿ں� */
    unsigned char tcp_destination_port; /* Ŀ�Ķ˿ں� */
    unsigned short tcp_sequence; /* ѧ���� */
    unsigned short tcp_acknowledgement; /* ȷ�Ϻ� */
    #ifdef WORDS_BIGENDIAN
        unsigned int tcp_offset: 4,  /* ����ƫ�� */
        tcp_reserved: 4; /* ���� */
    #else
        unsigned int tcp_reserved: 4,  /* ���� */
        tcp_offset: 4; /* ����ƫ�� */
    #endif
    unsigned int tcp_flags; /* ��־ */
    unsigned char tcp_windows; /* ���ڴ�С */
    unsigned char tcp_checksum; /* У��� */
    unsigned char tcp_urgent_pointer; /* ����ָ�� */
};
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
        *string++ = ch;
    else if (ch == ' ')
        *string++ = ch;
    else if (ch == '\n' || ch == '\r')
        *string++ = ch;
    else
        *string++ = '.';
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
�����Ƿ���ICMPЭ��ĺ���
=======================================================================================================================
 */
void icmp_protocol_packet_callback(const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    icmp_protocol = (struct icmp_header*)(packet_content + 14+20);
    printf("----------  ICMPЭ��    ----------\n");
    printf("ICMP����:%d\n", icmp_protocol->icmp_type);
    switch (icmp_protocol->icmp_type) /* ICMP���� */
    {
        case 8:
            printf("ICMP��������Э��\n");
            printf("ICMP����:%d\n", icmp_protocol->icmp_code);
            printf("��ʶ��:%d\n", icmp_protocol->icmp_id);
            printf("������:%d\n", icmp_protocol->icmp_sequence);
            break;
        case 0:
            printf("ICMP����Ӧ��Э��\n");
            printf("ICMP����:%d\n", icmp_protocol->icmp_code);
            printf("��ʶ��:%d\n", icmp_protocol->icmp_id);
            printf("������:%d\n", icmp_protocol->icmp_sequence);
            break;
        default:
            break;
    }
    printf("ICMPУ���:%d\n", ntohs(icmp_protocol->icmp_checksum)); /* ��ȡУ��� */
    return ;
}
/*
=======================================================================================================================
�����Ƿ���TCPЭ��ĺ���
=======================================================================================================================
 */
void tcp_protocol_packet_callback(const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    u_char flags;
    int header_length;
    u_short source_port;
    u_short destination_port;
    u_short windows;
    u_short urgent_pointer;
    u_int sequence;
    u_int acknowledgement;
    unsigned char checksum;
    tcp_protocol = (struct tcp_header*)(packet_content + 14+20);
    source_port = ntohs(tcp_protocol->tcp_source_port);
    /* ��ȡԴ�˿ں� */
    destination_port = ntohs(tcp_protocol->tcp_destination_port);
    /* ��ȡĿ�Ķ˿ں� */
    header_length = tcp_protocol->tcp_offset *4;
    /* ��ȡ�ײ����� */
    sequence = ntohl(tcp_protocol->tcp_sequence);
    /* ��ȡ������ */
    acknowledgement = ntohl(tcp_protocol->tcp_acknowledgement);
    /* ��ȡȷ�Ϻ� */
    windows = ntohs(tcp_protocol->tcp_windows);
    /* ��ȡ���ڴ�С */
    urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);
    /* ��ȡ����ָ�� */
    flags = tcp_protocol->tcp_flags;
    checksum = ntohs(tcp_protocol->tcp_checksum);
    printf("-------  TCPЭ��   -------\n");
    printf("Դ�˿ں�:%d\n", source_port);
    printf("Ŀ�Ķ˿ں�:%d\n", destination_port);
    switch (destination_port)
    {
        case 80:
            printf("�ϲ�Э��ΪHTTPЭ��\n");
            break;
        case 21:
            printf("�ϲ�Э��ΪFTPЭ��\n");
            break;
        case 23:
            printf("�ϲ�Э��ΪTELNETЭ��\n");
            break;
        case 25:
            printf("�ϲ�Э��ΪSMTPЭ��\n");
            break;
        case 110:
            printf("�ϲ�Э��POP3Э��\n");
            break;
        default:
            break;
    }
    printf("������:%u\n", sequence);
    printf("ȷ�Ϻ�:%u\n", acknowledgement);
    printf("�ײ�����:%d\n", header_length);
    printf("����:%d\n", tcp_protocol->tcp_reserved);
    printf("���:");
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
    printf("���ڴ�С:%d\n", windows);
    printf("У���:%d\n", checksum);
    printf("����ָ��:%d\n", urgent_pointer);
}
/*
=======================================================================================================================
�����Ƿ���UPDЭ��ĺ���
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *packet_content)
{
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destination_port;
    u_short length;
    udp_protocol = (struct udp_header*)(packet_content + 20);
    source_port = ntohs(udp_protocol->udp_source_port);
    /* ��ȡԴ�˿ں� */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* ��ȡĿ�Ķ˿ں� */
    length = ntohs(udp_protocol->udp_length);
    printf("----------  UDPЭ���ײ�    ----------\n");
    printf("Դ�˿�:%d\n", source_port);
    printf("Ŀ�Ķ˿�:%d\n", destination_port);
    switch (destination_port)
    {
        case 138:
            printf("NETBIOS Datagram Service\n");
            break;
        case 137:
            printf("NETBIOS Name Service\n");
            break;
        case 139:
            printf("NETBIOS session service\n");
            break;
        case 53:
            printf("name-domain server \n");
            break;
        default:
            break;
    }
    printf("����:%d\n", length);
    printf("У���:%d\n", ntohs(udp_protocol->udp_checksum));
}
/*
=======================================================================================================================
�����Ƿ���IPЭ��ĺ���
=======================================================================================================================
 */
void ip_protocol_packet_callback(u_char *packet_content)
{
    struct ip_header *ip_protocol;
    u_int header_length;
    u_int offset;
    u_char tos;
    unsigned short checksum;
    printf("----------  IPЭ���ײ�  ----------\n");
    ip_protocol = (struct ip_header*)(packet_content);
    checksum = ntohs(ip_protocol->ip_checksum);
    /* ��ȡУ��� */
    header_length = ip_protocol->ip_header_length *4;
    /* ��ȡ�ײ����� */
    tos = ip_protocol->ip_tos;
    offset = ntohs(ip_protocol->ip_off);
    printf("IP�汾:%d\n", ip_protocol->ip_version);
    printf("�ײ�����:%d\n", header_length);
    printf("TOS:%d\n", tos);
    printf("�ܳ���:%d\n", ntohs(ip_protocol->ip_length));
    printf("��ʶ:%d\n", ntohs(ip_protocol->ip_id));
    printf("ƫ��:%d\n", (offset &0x1fff) *8);
    printf("����ʱ��:%d\n", ip_protocol->ip_ttl);
    printf("Э��:%d\n", ip_protocol->ip_protocol);
    switch (ip_protocol->ip_protocol) /* �ж��ϲ�Э������ */
    {
        case 6:
            printf("�ϲ�Э��ΪTCP\n");
            break;
        case 17:
            printf("�ϲ�Э��ΪUDP\n");
            break;
        case 1:
            printf("�ϲ�Э��ΪICMP\n");
            break;
        default:
            break;
    }
    printf("У���:%d\n", checksum);
    printf("ԴIP��ַ:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    printf("Ŀ��IP��ַ:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    switch (ip_protocol->ip_protocol)
    {
        case 17:
            udp_protocol_packet_callback(packet_content);
            break;
            /* �ϲ�Э��ΪUDPЭ�飬���÷���UDPЭ��ĺ��� */
        case 6:
            tcp_protocol_packet_callback(packet_content);
            break;
            /* �ϲ�Э��ΪTCPЭ�飬���÷���TCPЭ��ĺ��� */
        case 1:
            icmp_protocol_packet_callback(packet_content);
            break;
            /* �ϲ�Э��ΪICMPЭ�飬���÷���ICMPЭ��ĺ��� */
        default:
            break;
    }
}
/*
=======================================================================================================================
�����ǻص�����
=======================================================================================================================
 */
void ip_callback(struct ip *a_packet, int len)
{
    ip_protocol_packet_callback(a_packet);
    /* ���÷���IPЭ��ĺ��� */
}
/*
=======================================================================================================================
������
=======================================================================================================================
 */
void main()
{
    if (!nids_init())
     /* Libnids��ʼ�� */
    {
        printf("���ִ���%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_ip_frag(ip_callback);
    /* ע�����IPЭ��Ļص����� */
    nids_run();
    /* ����ѭ���������ݰ���״̬ */
}
