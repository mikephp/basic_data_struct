#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ�
=======================================================================================================================
 */
int main()
{
    struct ip_hdr *ip;
    /* IPЭ������ݽṹ */
    ip_t *handle;
    /* IPЭ�������� */
    u_char buf[IP_LEN_MAX];
    /* ���棬�����洢IPЭ������ */
    int length = 20;
    /* IPЭ�����ݳ��� */
    struct addr addr;
    /* ��ַ */
    char *destination_ip = "192.168.0.3";
    /* IPĿ�ĵ�ַ */
    char *source_ip = "192.168.0.2";
    /* IPԴ��ַ */
    ip = (struct ip_hdr*)buf;
    /* ��buf��ʾ�Ļ�������ת��ΪIP��Э���ʽ */
    ip->ip_hl = 5;
    /* IP�ײ����� */
    ip->ip_v = 4;
    /* IP�汾�� */
    ip->ip_tos = 0;
    /* IP��TOS */
    ip->ip_id = 0x11;
    /* IP��ʶ���� */
    ip->ip_off = 0;
    /* ƫ���� */
    ip->ip_ttl = IP_TTL_MAX;
    /* IP����ʱ�� */
    ip->ip_p = 6;
    /* IP�ϲ�Э������ */
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
    ip_checksum(buf, length);
    /* ����У��� */
    handle = ip_open();
    /* ��IPЭ�����������IPЭ�������� */
    ip_send(handle, buf, length);
    /* ����IPЭ������ */
    ip_close(handle);
    /* �ر�IPЭ����� */
}
