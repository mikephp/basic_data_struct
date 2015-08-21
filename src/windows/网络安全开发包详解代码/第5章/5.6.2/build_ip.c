#include <win32/libnet.h>
/*
=======================================================================================================================
������ͷ�ļ�
=======================================================================================================================
 */
void main()
{
    int packet_size;
    /* ��������ݰ���С */
    libnet_t *l;
    /* libnet��� */
    char *device = NULL;
    /* �豸���� */
    char *destination_ip_str = "192.168.0.2";
    /* Ŀ��IP��ַ�ַ�������������ָ������һ���Ϸ���IP��ַ */
    char *source_ip_str = "192.168.0.3";
    /* ԴIP��ַ�ַ�������������ָ������һ���Ϸ���IP��ַ */
    u_char hardware_source[6] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    };
    /* ԴӲ����ַ������ָ�������MAC��ַ */
    u_char hardware_destination[6] =
    {
        0x06, 0x05, 0x04, 0x03, 0x02, 0x01
    };
    /* Ŀ��Ӳ����ַ������ָ�������MAC��ַ */
    u_long destination_ip;
    /* Ŀ��IP��ַ */
    u_long source_ip;
    /* ԴIP��ַ */
    char error_buffer[LIBNET_ERRBUF_SIZE];
    /* ��Ŵ�����Ϣ */
    libnet_ptag_t ip_protocol_tag = 0;
    /* IPЭ����Э���� */
    libnet_ptag_t ethernet_protocol_tag = 0;
    /* ��̫��Э����Э���� */
    u_short protocol = IPPROTO_UDP;
    /* ��ʾIP��һ���Э�����ͣ���������UDPЭ�� */
    u_char payload[255] = "";
    /* ���ص����ݣ���ʼֵΪ�� */
    u_long payload_size = 0;
    /*
     * ���صĳ��ȣ���ʼֵΪ0 ;
     * ��libnet���г�ʼ������
     */
    l = libnet_init(LIBNET_LINK,  /* libnet���� */device,  /* �����豸 */error_buffer);
    /* ������Ϣ */
    destination_ip = libnet_name2addr4(l, destination_ip_str, LIBNET_RESOLVE);
    /* ��Ŀ��IP��ַ�ַ�������ת��Ϊ����˳�����ֵ */
    source_ip = libnet_name2addr4(l, source_ip_str, LIBNET_RESOLVE);
    /* ��ԴIP��ַ�ַ�������ת��Ϊ����˳�����ֵ */
    strncpy(payload, "test", sizeof(payload) - 1);
    /* ���츺�ص����ݣ���ʱ������Ϊ"test"��ֻ�ǲ��� */
    payload_size = strlen(payload);
    /* ���㸺�����ݵĳ��� */
    ip_protocol_tag = libnet_build_ipv4(
    /* ����IPЭ��� */
    LIBNET_IPV4_H + payload_size,
    /* IPЭ�����������ȣ���������IPЭ��ͷ���ϸ��صĳ��� */
    0,
    /* ����������������Ϊ0 */
    242,
    /* IP��� */
    0,
    /* IP����ָ�� */
    64,
    /* ����ʱ�� */
    protocol,
    /* �ϲ�Э������ */
    0,
    /* У��ͣ���ʱΪ0����ʾ��libnet�Զ����� */
    source_ip,
    /* ԴIP��ַ */
    destination_ip,
    /* Ŀ��IP��ַ */
    payload,
    /* �������� */
    payload_size,  /* �������ݵĴ�С */l,
    /* libnet�������libnet_init()�������ɵ� */
    ip_protocol_tag /* IPЭ����ǣ���ʱΪ0����ʾ�����µ�IPЭ��� */
    );
    ethernet_protocol_tag = libnet_build_ethernet(
    /* ����һ����̫��Э��� */
    hardware_destination,
    /* ��̫��Ŀ�ĵ�ַ */
    hardware_source,
    /* ��̫��Դ��ַ */
    ETHERTYPE_IP,
    /* �ϲ�Э�����ͣ������������IPЭ�� */
    NULL,
    /* ���أ�����Ϊ�� */
    0,
    /* ���ش�С */
    l,
    /* libnet��� */
    ethernet_protocol_tag /* ��̫��Э����ǣ���ʱΪ0����ʾ�����µ���̫��Э���
     * */
    );
    packet_size = libnet_write(l);
    /* �������ݰ� */
    printf("����һ��%d�ֽڵ�IP���ݰ�\n", packet_size);
    libnet_destroy(l);
    /* ����libnet */
}
