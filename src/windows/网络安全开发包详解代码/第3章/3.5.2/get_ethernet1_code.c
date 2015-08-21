/* �ļ����֣�get_ethernet1_code.c */
#include <pcap.h>
/*
-----------------------------------------------------------------------------------------------------------------------
Libpcapͷ�ļ�
-----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
/* ��̫��Э������ݽṹ */
{
    u_int8_t ether_dhost[6];
    /* Ŀ����̫����ַ */
    u_int8_t ether_shost[6];
    /* Դ��̫����ַ */
    u_int16_t ether_type;
    /* ��̫������ */
};
void main()
{
    char error_content[PCAP_ERRBUF_SIZE];
    /* ������Ϣ */
    pcap_t *pcap_handle;
    /* Libpcap��� */
    const u_char *packet_content;
    /* ���ݰ����� */
    u_char *mac_string;
    /* ��̫����ַ */
    u_short ethernet_type;
    /* ��̫������ */
    bpf_u_int32 net_mask;
    /* �������� */
    bpf_u_int32 net_ip;
    /* �����ַ */
    char *net_interface;
    /* ����ӿ� */
    struct pcap_pkthdr protocol_header;
    /* ���ݰ���Ϣ */
    struct ether_header *ethernet_protocol;
    /* ��̫��Э����� */
    struct bpf_program bpf_filter;
    /* bpf���˹��� */
    char bpf_filter_string[] = "ip";
    /*
     * ���˹��򣬴˴���ʾֻ����IP���ݰ�����Ȼ��Ҳ����̫�����ݰ�����Ϊ������Ļ���Ϊ��̫��������
     * IP���ݰ��ǻ�����̫��Э��ģ����Բ���IP���ݰ������൱�ڲ�����̫�����ݰ����˴�ֻ��Ϊ���ܹ�������̫�����ݰ����������⡣��Ȼ������ʹ�������Ĺ��˹���ֻҪ�ܲ�����̫��Э����У���Ϊ�������Ŀ�ľ���Ϊ�˲�����̫�����ݰ�
     */
    net_interface = pcap_lookupdev(error_content);
    /* ��ȡ����ӿ� */
    pcap_lookupnet(net_interface,  /* ����ӿ� */ &net_ip,  /* �����ַ */ &net_mask,  /* �������� */error_content); /* ������Ϣ
     * */
    /* ��ȡ�����ַ�������ַ */
    pcap_handle = pcap_open_live(net_interface,  /* ����ӿ� */
    BUFSIZ,  /* ���ݰ���С */
    1,  /* ����ģʽ */
    0,  /* �ȴ�ʱ�� */
    error_content); /* ������Ϣ */
    /* ������ӿ� */
    pcap_compile(pcap_handle,  /* Libpcap��� */
     &bpf_filter,  /* BPF���˹��� */
    bpf_filter_string,  /* ���˹����ַ��� */
    0,  /* �Ż����� */
    net_ip); /* �����ַ */
    /* ����bpf���˹��� */
    pcap_setfilter(pcap_handle,  /* Libpcap��� */ &bpf_filter /* BPF���˹��� */);
    /* ���ù��˹��� */
    if (pcap_datalink(pcap_handle) != DLT_EN10MB)
        return ;
    packet_content = pcap_next(pcap_handle,  /* Libpcap��� */ &protocol_header /* ���ݰ���Ϣ */);
    /*
     * ����һ���������ݰ������ش����ݰ��Ļ��棬�˻���洢�ľ��ǲ��񵽵��������ݰ����ֽ���������������Ҫ�����˻��������
     */
    printf("--------------------*****-----------------------\n");
    printf("Capture a Packet from net_interface :\n");
    printf("%s \n", net_interface);
    /* �������ӿ� */
    printf("Capture Time is:\n");
    printf("%s", ctime((const time_t*) &protocol_header.ts.tv_sec));
    /* ��������ݰ���ʱ����Ϣ */
    printf("Packet Length is:\n");
    printf("%d\n", protocol_header.len);
    /* ��������ݰ��ĳ������� */
    ethernet_protocol = (struct ether_header*)packet_content;
    /*
     * �Ѵ����ݰ�����������͵�ǿ��ת����ʹ�������̫��Э���ʽ���������ͣ�Ȼ��Ϳ��Զ����ĸ����ֶν��з����ˡ�ע��˴���ǿ��ת�����ܣ����Ƕ��������ݰ����з������ص����
     */
    printf("Ethernet type is :\n");
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    /* �����̫�������ͣ�����ʾ�ϲ�Э������ͣ�����·���Э�����͡� */
    printf("%04x\n", ethernet_type);
    /* �����̫������ */
    switch (ethernet_type)
    {
        case 0x0800:
            printf("The network layer is IP protocol\n");
            break;
            /* �����̫��������0x0800�ͱ�ʾ�ϲ�Э������ΪIPЭ�� */
        case 0x0806:
            printf("The network layer is ARP protocol\n");
            break;
            /* �����̫��������0x0806�ͱ�ʾ�ϲ�Э������ΪARPЭ�� */
        case 0x8035:
            printf("The network layer is RARP protocol\n");
            break;
            /* �����̫��������0x8035�ͱ�ʾ�ϲ�Э������ΪRARPЭ�� */
        default:
            break; /* ������̫�����ͣ�������û�з��� */
    }
    printf("Mac Source Address is : \n");
    mac_string = ethernet_protocol->ether_shost;
    /* ���Դ��̫����ַ */
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /*
     * Ҫ����̫����ַ����ת����ʹ������ַ�����ʽ������ʾ������11:11:11:11:11:11����Ϊ������Դ��̫����ַ���ֽ���˳���
     */
    printf("Mac Destination Address is : \n");
    mac_string = ethernet_protocol->ether_dhost;
    /* ���Ŀ����̫����ַ */
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",  *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    /* ͬ���������ַ����ת�� */
    printf("--------------------*****-----------------------\n");
    pcap_close(pcap_handle);
    /* �ر�Libpcap���� */
}
