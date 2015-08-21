#include "dnet.h"
int main()
{
    struct ip_hdr *ip;
    /* IPЭ��ͷ�����ݽṹ */
    struct udp_hdr *udp;
    /* UDPЭ��ͷ�����ݽṹ */
    ip_t *handle;
    /* IPЭ�������� */
    u_char content[IP_LEN_MAX];
    /* UDPЭ���������ݣ����л�����IPЭ��ͷ���� */
    int length = 20+8;
    /* ���ݰ����ȣ�����IPЭ��ͷ���Ⱥ�UDPЭ�鳤�� */
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
    ip->ip_id = htons(11);
    /* IP��ʶ */
    ip->ip_off = 0;
    /* ƫ���� */
    ip->ip_ttl = IP_TTL_MAX;
    /* TTL����ʱ�� */
    ip->ip_p = 17;
    /* IP�ϲ�Э������ */
    ip->ip_sum = 0;
    /* У��� */
    addr_aton(source_ip, &addr);
    /* ԴIP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    ip->ip_src = addr.addr_ip;
    /* ԴIP��ַ */
    addr_aton(destination_ip, &addr);
    /* ��Ŀ��IP��ַ���ַ�����ʽת��Ϊ��������ʽ */
    ip->ip_dst = addr.addr_ip;
    /* Ŀ��IP��ַ */
    ip->ip_len = htons(length);
    /* IP���� */
    ip_checksum(content, length);
    /* ����У��� */
    udp = (struct udp_hdr*)(content + 20);
    /* ����UDPЭ�����ݣ�����IPЭ��ͷ������Ӧ�ü�20 */
    memset(udp, 0, sizeof(*udp));
    udp->uh_sport = htons(81);
    /* Դ�˿ں� */
    udp->uh_dport = htons(91);
    /* Ŀ�Ķ˿ں� */
    udp->uh_ulen = htons(8);
    /* ���� */
    handle = ip_open();
    /* ��IPЭ����� */
    ip_send(handle, content, length);
    /* ����UDPЭ�����ݣ���Ҳ�ǻ���ԭʼ�׽���IP���͵� */
    ip_close(handle);
    /* �ر�IPЭ����� */
}
