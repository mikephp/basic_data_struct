#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ�
=======================================================================================================================
 */
int main()
{
    struct ip_hdr *ip;
    /* IPЭ��ͷ�����ݽṹ */
    struct tcp_hdr *tcp;
    /* TCPЭ��ͷ�����ݽṹ */
    ip_t *handle;
    /* IPЭ�������� */
    u_char content[IP_LEN_MAX];
    /* TCP���ݰ����ݣ�����IPЭ��ͷ���� */
    int length = 20+20;
    /* �������ȣ�����IPЭ��ͷ���Ⱥ�TCPЭ��ͷ���� */
    struct addr addr;
    /* ��ַ */
    char *destination_ip = "192.168.0.3";
    /* Ŀ��IP��ַ */
    char *source_ip = "192.168.0.2";
    /* ԴIP��ַ */
    ip = (struct ip_hdr*)content;
    /* ����IPЭ��ͷ */
    ip->ip_hl = 5;
    /* IP���� */
    ip->ip_v = 4;
    /* IP�汾�� */
    ip->ip_tos = 0;
    /* �������� */
    ip->ip_id = 0x11;
    /* IP��ʶ */
    ip->ip_off = 0;
    /* IPƫ���� */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL����ʱ�� */
    ip->ip_p = 6;
    /* IP�ϲ�Э�����ͣ�6����TCPЭ�� */
    ip->ip_sum = 0;
    /* ����У��� */
    addr_aton(source_ip, &addr);
    /* ��ԴIP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    ip->ip_src = addr.addr_ip;
    /* ԴIP��ַ */
    addr_aton(destination_ip, &addr);
    /* ��Ŀ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    ip->ip_dst = addr.addr_ip;
    /* Ŀ��IP��ַ */
    ip->ip_len = htons(length);
    /* ���� */
    tcp = (struct tcp_hdr*)(content + 20);
    /* ����TCPЭ�����ݣ�����IPЭ�����ݣ�����Ӧ�ü�20 */
    memset(tcp, 0, sizeof(*tcp));
    tcp->th_sport = htons(81);
    /* Դ�˿ں� */
    tcp->th_dport = htons(91);
    /* Ŀ�Ķ˿ں� */
    tcp->th_seq = htonl(111);
    /* ���к� */
    tcp->th_ack = 0;
    /* ȷ�Ϻ� */
    tcp->th_off = 5;
    /* ƫ�� */
    tcp->th_flags = TH_SYN;
    /* ���Ʊ�� */
    tcp->th_win = TCP_WIN_MAX;
    /* ���ڴ�С */
    tcp->th_urp = 0;
    /* ����ָ�� */
    ip_checksum(content, length);
    /* �����У��� */
    handle = ip_open();
    /* ��IPЭ����� */
    ip_send(handle, content, length);
    /* ����TCP���ݰ������ǻ���ԭʼ�׽���IP���й���� */
    ip_close(handle);
    /* �ر�IPЭ����� */
}
