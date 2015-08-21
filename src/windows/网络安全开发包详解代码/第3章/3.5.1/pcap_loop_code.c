/* �ļ����֣�pcap_loop_code.c */
#include "pcap.h"
/*
=======================================================================================================================
Libpcapͷ�ļ� ;
�����ǻص������Ķ��壬�ڴ˴�ʵ�ֵĹ����Ǽ�¼���񵽵��������ݰ��ĸ���
=======================================================================================================================
 */
void packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    static int packet_number = 1;
    /* ��̬�ֲ��������������񵽵Ĵ�����ݰ��ĸ��� */
    printf("The %d packet is captured.\n", packet_number);
    /* ����������ݰ��ĸ��� */
    packet_number++;
    /* �������� */
} void main()
{
    pcap_t *pcap_handle;
    /* Libpcap��� */
    char error_content[PCAP_ERRBUF_SIZE];
    /* �����洢������Ϣ */
    char *net_interface;
    /* �����洢����ӿ� */
    struct bpf_program bpf_filter;
    /* bpf���˹��� */
    char bpf_filter_string[] = "ip";
    /* ���˹����ַ�����ʽ */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    net_interface = pcap_lookupdev(error_content);
    /* ��ȡ��·�ӿ� */
    pcap_lookupnet(net_interface,  /* ����ӿ� */ &net_ip,  /* �����ַ */ &net_mask,  /* �������� */error_content); /* ������Ϣ
     * */
    /* ��ȡ�����ַ������ */
    pcap_handle = pcap_open_live(net_interface,  /* ����ӿ� */
    BUFSIZ,  /* ���ݰ���С */
    1,  /* ����ģʽ */
    0,  /* �ȴ�ʱ�� */
    error_content); /* ������Ϣ */
    /* ������ӿ� */
    pcap_compile(pcap_handle,  /* ����ӿ� */
     &bpf_filter,  /* BPF���˹��� */
    bpf_filter_string,  /* ���˹����ַ��� */
    0,  /* �Ż����� */
    net_ip); /* �����ַ */
    /* ������˹��� */
    pcap_setfilter(pcap_handle,  /* Libpcap��� */ &bpf_filter); /* BPF���˹��� */
    /* ���ù��˹��� */
    pcap_loop(pcap_handle,  /* Libpcap��� */10,  /* �������ݰ��ĸ��� */packet_callback,  /* �ص����� */NULL); /* ���ݸ��ص������Ĳ���
     * */
    /*
     * ע��ص�����packet_callback������Ȼ��ѭ�������������ݰ���ÿ����һ�����ݰ��͵��ûص��������д��������ﲶ�����ݰ��ĸ�����10�����Ե�����10���������ݰ���ʱ����˳�ѭ�������������Ϊ-1���ͱ�ʾ����ѭ����
     */
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
