#include "pcap.h"
int main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* ������Ϣ */
    struct pcap_pkthdr protocol_header;
    /* ���ݰ�ͷ */
    pcap_t *pcap_handle;
    /* Libpcap��� */
    struct bpf_program bpf_filter;
    /* bpf���˹��� */
    char bpf_filter_string[] = "";
    /* ���˹��� */
    const u_char *packet_content;
    /* ���ݰ����� */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    char *net_interface;
    /* ����ӿ� */
    net_interface = pcap_lookupdev(error_content);
    /* ��ȡ����ӿ� */
    pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);
    /* ��ȡ�����ַ�������ַ */
    pcap_handle = pcap_open_live(net_interface,  /* ����ӿ� */
    BUFSIZ,  /* ���ݰ���С */
    1,  /* ����ģʽ */
    0,  /* �ȴ�ʱ�� */
    error_content); /* ������Ϣ */
    /* ������ӿ� */
    pcap_compile(pcap_handle,  /* Libpcap��� */
     &bpf_filter,  /* BPF���˹��� */
    bpf_filter_string,  /* BPF���˹����ַ��� */
    0,  /* �Ż����� */
    net_ip); /* �����ַ */
    /* ������˹��� */
    pcap_setfilter(pcap_handle,  /* Libpcap��� */ &bpf_filter); /* BPF���˹��� */
    /* ���ù��˹��� */
    packet_content = pcap_next(pcap_handle,  /* Libpcap��� */ &protocol_header); /* ���ݰ���Ϣ */
    /* ����һ�����ݰ������ش����ݰ������� */
    printf("Capture a packet from : %s\n", net_interface);
    /* �������ӿ����� */
    printf("The packet length is :%d\n", protocol_header.len);
    /* �����������ݰ��ĳ��� */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
