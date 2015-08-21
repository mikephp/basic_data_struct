#include <win32/libnet.h>
void main()
{
    libnet_t *l = NULL;
    /* libnet��� */
    libnet_ptag_t protocol_tag;
    /* Э���� */
    char *payload = NULL;
    /* ���� */
    u_short payload_length = 0;
    /* ���س��� */
    char *device = "\\Device\\NPF_{5973DFB0-AD5B-4065-A22B-C61FFB64624F}";
    /*
     * �����豸�ӿڣ��������õ���Windows�µ�һ���ӿڣ����ݸ��ֻ��ӵ����Ͷ���������ͨ���ܶ෽����ã�����ʹ��windump��ngrep��������ü���
     */
    char *destination_ip_str = "192.168.0.3";
    /* Ŀ��IP��ַ�ַ��� */
    char *source_ip_str = "192.168.0.2";
    /* ԴIP��ַ�ַ��� */
    u_long source_ip = 0;
    /* ԴIP��ַ */
    u_long destination_ip = 0;
    /* Ŀ��IP��ַ */
    char errbuf[LIBNET_ERRBUF_SIZE];
    /* ������Ϣ */
    char label[LIBNET_LABEL_SIZE];
    /* libnet����ַ������ */
    int j = 0;
    /* ���ڱ�ǵڼ������ݰ� */
    int i = 0;
    /* ����ѭ�� */
    int packet_length;
    /* ���͵����ݰ��ĳ��� */
    int number = 5;
    /* �������ݰ��ĸ��� */
    for (i = 0; i < number; i++)
    /*
     * ѭ������5�����ݰ���ÿ�����ݰ���libnet����ַ�����ǲ�ͬ��Ҳ����˵��Ψһ��һ��libnet����ַ��������ָʾһ����������ݰ�
     */
    {
        l = libnet_init( /* ��ʼ��libnet */LIBNET_RAW4,  /* libnet���ͣ�Ϊԭʼ�׽���IPv4���� */device,  /* �����豸�ӿ� */errbuf /* ������Ϣ */
        );
        source_ip = libnet_name2addr4(l, source_ip_str, LIBNET_RESOLVE);
        /* ��ԴIP��ַ�ַ�����ʽת��Ϊ�����ֽ�˳������� */
        destination_ip = libnet_name2addr4(l, destination_ip_str, LIBNET_RESOLVE);
        /* ��Ŀ��IP��ַ�ַ�����ʽת��Ϊ�����ֽ�˳������� */
        protocol_tag = libnet_build_icmpv4_echo(
        /* ����ICMP�������ݰ� */
        ICMP_ECHO,
        /* ���ͣ���ʱΪ�������� */
        0,
        /* ���룬Ӧ��Ϊ0 */
        0,
        /* У��ͣ�Ϊ0����ʾ��libnet����Զ����� */
        123,
        /* ��ʶ������ֵΪ123���Լ�������д��ֵ */
        456,
        /* ���кţ���ֵΪ245���Լ�������д��ֵ */
        NULL,
        /* ���أ���ֵΪ�� */
        0,
        /* ���صĳ��ȣ���ֵΪ0 */
        l,
        /* libnet�����Ӧ������libnet_init()�����õ��� */
        0
        /* Э����ǣ���ֵΪ0����ʾ����һ���µ�Э��� */
        );
        protocol_tag = libnet_build_ipv4(
        /* ����IPЭ��� */
        LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_length,
        /* IPЭ���ĳ��� */
        0,
        /* �������������︳ֵΪ0 */
        10,
        /* ��ʶ�������︳ֵΪ10 */
        0,
        /* ƫ�ƣ����︳ֵΪ0 */
        20,
        /* ����ʱ�䣬���︳ֵΪ20 */
        IPPROTO_ICMP,
        /* �ϲ�Э�����ͣ�������ICMPЭ�� */
        0,
        /* У��ͣ�����Ϊ0��ʾ��libnet����У��� */
        source_ip,
        /* ԴIP��ַ */
        destination_ip,
        /* Ŀ��IP��ַ */
        payload,
        /* ���� */
        payload_length,
        /* ���صĳ��� */
        l,
        /* libnet��� */
        0
        /* Э����ǣ�Ϊ0��ʾ����һ���µ�IPЭ��� */
        );
        snprintf(label, sizeof(label) - 1, "test %d", i);
        /* ����һ��libnet����ַ�����ʶ������ָʾһ��libnet��� */
        libnet_cq_add(l, label);
        /*
         * ����һ��libnet�����㵽libnet��������У���������Ϊ��һ����㣬�˾�������label�ַ�����Ψһ��ʶ
         */
    }
    for (l = libnet_cq_head(); libnet_cq_last(); l = libnet_cq_next())
    /* ѭ������libnet������������е�libnet�����Ȼ���� */
    {
        j++;
        /* �ڼ���libnet��� */
        packet_length = libnet_write(l);
        /* ������libnet���l��ʾ�����ݰ� */
        printf("���͵� %d ��%d�ֽڳ��ȵ�ICMP�����������ݰ������Ӧ��libnet���Ϊ\"%s\"\n", j, packet_length, libnet_cq_getlabel(l));
        /* ������͵����ݰ���Ϣ */
    }
    libnet_cq_destroy();
    /* ����libnet������У� ͬʱ������������libnet��������е�ÿ��libnet��� */
}
