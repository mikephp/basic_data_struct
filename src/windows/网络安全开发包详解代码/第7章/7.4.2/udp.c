#include "nids.h"
/*
-----------------------------------------------------------------------------------------------------------------------
������UDPЭ���ײ������ݽṹ
-----------------------------------------------------------------------------------------------------------------------
 */
struct udp_header
{
    unsigned short udp_source_port; /* Դ�˿ں� */
    unsigned short udp_destination_port; /* Ŀ�Ķ˿ں� */
    unsigned short udp_length; /* ���� */
    unsigned short udp_checksum; /* У��� */
};
/*
-----------------------------------------------------------------------------------------------------------------------
������IPЭ���ײ������ݽṹ
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
    unsigned char ip_ttl; /* ����ʱ��  */
    unsigned char ip_protocol; /*Э�� */
    unsigned short ip_checksum; /* У��� */
    struct in_addr ip_souce_address; /* ԴIP��ַ */
    struct in_addr ip_destination_address; /* Ŀ��IP��ַ */
};
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
    {
        *string++ = ch;
    }
    else if (ch == ' ')
    {
        *string++ = ch;
    }
    else if (ch == '\n' || ch == '\r')
    {
        *string++ = ch;
    }
    else
    {
        *string++ = '.';
    }
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
�����Ƿ���UDPЭ��ĺ����Ķ���
=======================================================================================================================
 */
void udp_protocol_packet_callback(u_char *packet_content)
{
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destination_port;
    u_short length;
    udp_protocol = (struct udp_header*)(packet_content + 20);
    /* ��ȡUPDЭ������ */
    source_port = ntohs(udp_protocol->udp_source_port);
    /* ��ȡԴ�˿� */
    destination_port = ntohs(udp_protocol->udp_destination_port);
    /* ��ȡĿ�Ķ˿� */
    length = ntohs(udp_protocol->udp_length);
    /* ��ȡ���� */
    printf("----------  UDPЭ���ײ�    ----------\n");
    printf("Դ�˿�:%d\n", source_port);
    printf("Ŀ�Ķ˿�:%d\n", destination_port);
    switch (destination_port) /* �ж϶˿ںţ�ȷ���ϲ�Э������ */
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
    /* ��ȡУ��� */
}
/*
=======================================================================================================================
������ʵ��IPЭ������ĺ����Ķ���
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
    /* ��ȡIPЭ������ */
    checksum = ntohs(ip_protocol->ip_checksum);
    /* ��ȡУ��� */
    header_length = ip_protocol->ip_header_length *4;
    /* ��ȡ���� */
    tos = ip_protocol->ip_tos;
    /* ��ȡ�������� */
    offset = ntohs(ip_protocol->ip_off);
    /* ��ȡ��־��ƫ�� */
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
    /* ��ȡԴIP��ַ */
    printf("Ŀ��IP��ַ:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    /* ��ȡĿ��IP��ַ */
    switch (ip_protocol->ip_protocol)
    {
        case 17:
            udp_protocol_packet_callback(packet_content);
            break;
            /* ���÷���UDPЭ��ĺ��� */
        default:
            break;
    }
}
/*
=======================================================================================================================
�����ǻص��������˻ص��������ں���nids_register_udp�����б�ע��
=======================================================================================================================
 */
void udp_callback(struct tuple4 *addr, char *buf, int len, struct ip *iph)
{
    int i;
    char content[65535];
    char content_urgent[65535];
    char tcp_content[65535];
    char buffer[1024];
    strcpy(buffer, inet_ntoa(*((struct in_addr*) &(addr->saddr))));
    sprintf(buffer + strlen(buffer), " : %i", addr->source);
    strcat(buffer, " -> ");
    strcat(buffer, inet_ntoa(*((struct in_addr*) &(addr->daddr))));
    sprintf(buffer + strlen(buffer), " : %i", addr->dest);
    strcat(buffer, "\n");
    printf("------------------- BEGIN ---------------------------\n");
    printf("%s", buffer);
    ip_protocol_packet_callback(iph);
    /* ���÷���IPЭ��ĺ��� */
    printf("----------  UDP���ݰ�����  ----------");
    /* ���UDP������������ */
    for (i = 0; i < len; i++)
    {
        if (i % 50 == 0)
        {
            printf("\n");
        }
        printf("%s", char_to_ascii(buf[i]));
    }
    printf("\n");
    printf("------------------- END ---------------------------\n");
    printf("\n");
    return ;
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
        printf("%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_udp(udp_callback); /* ע��ص����� */
    nids_run(); /* ����ѭ���������ݰ�״̬ */
}
