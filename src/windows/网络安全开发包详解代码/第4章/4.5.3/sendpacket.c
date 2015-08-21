#include "pcap.h"
/*
=======================================================================================================================
WinPcapͷ�ļ�
=======================================================================================================================
 */
void main()
{
    pcap_t *pcap_handle;
    /* winpcap��� */
    char *net_interface;
    /* ����ӿ� */
    char error_content[PCAP_ERRBUF_SIZE];
    /* �洢������Ϣ */
    u_int8_t packet_content[94] = "\x00\x10\x67\x00\xB1\x86\x0D\x0E\x0A\x0D\x00\x00\x08\x00\x45\x00\x00\x50\x00\xF2\x00\x00\x40\x06\xF8\x60\xC0\xA8\x00\x02\xC0\xA8\x00\x03\x00\x63\x00\x50\x01\x01\x01\x01\x02\x02\x02\x02\xA0\x02\x7F\xFF\xB6\xF4\x00\x0A\x03\x03\x0A\x01\x02\x04\x01\x09\x08\x0A\x3F\x3F\x3F\x3F\x00\x00\x00\x00\x00\x00\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39";
    /* Ҫ���͵�ԭʼ���ݰ� */
    net_interface = pcap_lookupdev(error_content);
    /* �������ӿ� */
    pcap_handle = pcap_open_live(net_interface,  /* ����ӿ� */
    94,  /* ���ݰ���С */
    1,  /* ����ģʽ */
    1,  /* �ȴ�ʱ�� */
    error_content); /* ������Ϣ */
    /* ������ӿ� */
    pcap_sendpacket(pcap_handle,  /* winpcap��� */packet_content,  /* ���ݰ��������� */94); /* ���ݰ����� */
    /* �����������ݰ� */
    pcap_close(pcap_handle);
    /* �ر�Winpcap���� */
}
