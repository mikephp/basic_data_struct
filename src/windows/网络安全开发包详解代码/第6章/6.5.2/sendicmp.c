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
    struct icmp_hdr *icmp;
    /* ICMPЭ������ݽṹ */
    ip_t *handle;
    /* IPЭ�������� */
    u_char content[IP_LEN_MAX];
    /* ICMP���ݵ����� */
    int length = 20+8;
    /* ICMP���ݰ����ܳ��ȣ�Ӧ����IPЭ��ͷ��ICMPЭ�� */
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
    /* ƫ���� */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL����ʱ�� */
    ip->ip_p = 1;
    /* IP�ϲ�Э�����ͣ�1����ICMPЭ�� */
    ip->ip_sum = 0;
    /* У��� */
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
    icmp = (struct icmp_hdr*)(content + 20);
    /* ����ICMPЭ�����ݣ�Ӧ������IPЭ�����ݣ�����Ӧ�ü�20�ֽ��� */
    memset(icmp, 0, sizeof(*icmp));
    icmp->icmp_type = ICMP_ECHO;
    /*
     * ICMP���ͣ���ʱΪICMP_ECHO����ʾ��������ICMP���ݰ������Ҫ�����������͵�ICMP���ݰ�������Ҫ�޸���������͡�
     * icmp->icmp_code = 0;
     * /*ICMP���룬����ICMP����һ�����ICMP���ݵ�����
     */
    ip_checksum(content, length);
    /* ����У��� */
    handle = ip_open();
    /* ��IPЭ����� */
    ip_send(handle, content, length);
    /*
     * ����IPЭ�����ݰ���ע�������ﻹ�ǵ���ͬһ������������IPЭ�����ݰ�������ʱIP�ĸ�����һ��ICMPЭ�飬����ʵ�ʷ��͵���һ��ICMP���ݰ�����ΪICMPЭ���ǻ���IPЭ��ġ�
     */
    ip_close(handle);
    /* �ر�IPЭ�������� */
}
