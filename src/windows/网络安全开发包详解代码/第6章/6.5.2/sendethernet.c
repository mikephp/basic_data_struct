#include "dnet.h"
/*
=======================================================================================================================
Libdnetͷ�ļ�
=======================================================================================================================
 */
int main()
{
    eth_t *handle;
    /* ��̫��Э�������� */
    struct eth_hdr *eth;
    /* ��̫��Э��ṹ */
    struct addr addr;
    /* ��ַ */
    u_char content[ETH_LEN_MAX];
    /* �����̫��Э������ */
    int length;
    /* ���� */
    length = 14;
    eth = (struct eth_hdr*)content;
    /* ������̫��Э������ */
    memset(eth, 0, sizeof(*eth));
    eth->eth_type = htons(ETH_TYPE_IP);
    /* ��̫��Э������ */
    addr_aton("11:11:11:11:11:11", &addr);
    /* ��Դ��̫����ַת��Ϊ��������ʽ */
    memcpy(&eth->eth_src, &addr.addr_eth, ETH_ADDR_LEN);
    /* ��̫��Դ��ַ */
    addr_aton("22:22:22:22:22:22", &addr);
    /* ��Ŀ����̫����ַת��Ϊ��������ʽ */
    memcpy(&eth->eth_dst, &addr.addr_eth, ETH_ADDR_LEN);
    /* ��̫��Ŀ�ĵ�ַ */
    handle = eth_open("eth0");
    /* ����̫��Э��������ӿ�����Ϊeth0 */
    eth_send(handle, content, length);
    /* ������̫��Э������ */
    eth_close(handle);
    /* �ر���̫��Э����� */
}
