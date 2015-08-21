#include <dnet.h>
/*
=======================================================================================================================
Libdnetͷ�ļ� ;
�����Ƕ���Ļص���������ʵ������ʾ����ǽ����Ĺ���
=======================================================================================================================
 */
int show_firewall_fules(const struct fw_rule *firewall_rule, void *parameter)
{
    char operation[100];
    /* ��������ʾ�ǽ�ֹ�������� */
    char direction[100];
    /* ���򣬱�ʾ�ǽ��뻹�ǳ�ȥ */
    char net_interface[100];
    /* ����ӿ� */
    char source_ip[100];
    /* ԴIP��ַ */
    char destination_ip[100];
    /* Ŀ��IP��ַ */
    char protocol[16];
    /* ����Э�����ͣ��ڴ�ֻ����TCPЭ���UDPЭ�� */
    char source_port[16];
    /* Դ�˿ں� */
    char destination_port[16];
    /* Ŀ�Ķ˿ں� */
    static int count = 1;
    /* ������� */
    operation[0] = '\0';
    direction[0] = '\0';
    net_interface[0] = '\0';
    source_ip[0] = '\0';
    destination_ip[0] = '\0';
    source_port[0] = '\0';
    destination_port[0] = '\0';
    if (firewall_rule->fw_proto == 0)
        protocol[0] = '\0';
    else
    {
        if (firewall_rule->fw_proto == 6)
         /* ��ʾЭ����TCPЭ�� */
            sprintf(protocol, "%s ", "tcp");
        if (firewall_rule->fw_proto == 17)
         /* ��ʾЭ����UDPЭ�� */
            sprintf(protocol, "%s ", "udp");
    }
    if (firewall_rule->fw_proto == 6 || firewall_rule->fw_proto == 17)
    {
        /* ��Դ�˿ں� */
        if (firewall_rule->fw_sport[0] == firewall_rule->fw_sport[1])
        /* ��ʾ�˿ں�ֻ��һ��ֵ */
        {
            sprintf(source_port, "port = %d", firewall_rule->fw_sport[0]);
        }
        else
        /* ��ʾ�˿ں���һ����Χ�������1025��65535 */
            sprintf(source_port, "port %d-%d", firewall_rule->fw_sport[0], firewall_rule->fw_sport[1]);
        /* ��Ŀ�Ķ˿ں� */
        if (firewall_rule->fw_dport[0] == firewall_rule->fw_dport[1])
        {
            sprintf(destination_port, "port = %d", firewall_rule->fw_dport[0]);
        }
        else
            sprintf(destination_port, "port %d-%d", firewall_rule->fw_dport[0], firewall_rule->fw_dport[1]);
    }
    if (firewall_rule->fw_op == FW_OP_ALLOW)
     /* �жϲ������� */
        sprintf(operation, "%s", "pass");
    /* �������������� */
    else
        sprintf(operation, "%s", "block");
     / � � � � � � � � � � � � � � * / if (firewall_rule->fw_dir == FW_DIR_IN)
         /* �жϷ��� */sprintf(direction, "%s", "in");
    /* ��ʾ���� */
    else
        strcpy(direction, "out");
    /* ��ʾ��ȥ */
    sprintf(net_interface, "%s", firewall_rule->fw_device);
    /* �������ӿ� */
    if (firewall_rule->fw_src.addr_type)
        sprintf(source_ip, "from %s", addr_ntoa(&firewall_rule->fw_src));
    /* ���ԴIP��ַ */
    else
        sprintf(source_ip, "%s", "");
    if (firewall_rule->fw_dst.addr_type)
        sprintf(destination_ip, "to %s", addr_ntoa(&firewall_rule->fw_dst));
    /* ���Ŀ��IP��ַ */
    else
        sprintf(destination_ip, "");
    printf("��%d������\n", count);
    printf("%s %s %s %s %s %s %s %s %s \n", operation,  /* �������� */
    direction,  /* ���� */
    net_interface,  /* ����ӿ� */
    protocol,  /* Э������ */
    source_ip,  /* ԴIP��ַ */
    source_port,  /* Դ�˿ں� */
    destination_ip,  /* Ŀ��IP��ַ */
    destination_port,  /* Ŀ�Ķ˿ں� */
    "");
    /* ���һ������ */
    count++;
    /* ���������1 */
    return (0);
}
void main()
{
    struct fw_rule *firewall_rule;
    /* �������ǽ������� */
    fw_t *firewall_handle;
    /* �������ǽ��� */
    if ((firewall_handle = fw_open()) == NULL)
     /* �򿪷���ǽ���� */
        printf("�򿪷���ǽ����\n");
    fw_loop(firewall_handle,  /* ����ǽ��� */show_firewall_fules,  /* �ص����������� */NULL); /* ���ݸ��ص������Ĳ��� */
    /* ע��ص���������ÿ������ѭ�����ûص�������������й�����Ϣ */
    firewall_rule = (struct fw_rule*)malloc(sizeof(struct fw_rule));
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ��������Ϊ��ֹ */
    firewall_rule->fw_dir = FW_DIR_IN;
    /* ����Ϊ���� */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ����ӿ�Ϊeth0 */
    firewall_rule->fw_proto = 6;
    /* Э��Ϊ6����ʾTCPЭ�� */
    addr_aton("192.168.0.4", &firewall_rule->fw_src);
    /* ԴIP��ַ */
    addr_aton("192.168.0.5", &firewall_rule->fw_dst);
    /*
     * Ŀ��IP��ַ* fw_add(firewall_handle, firewall_rule);
     * /*��ӹ���
     */
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ��������Ϊ��ֹ */
    firewall_rule->fw_dir = FW_DIR_OUT;
    /* ����Ϊ��ȥ */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ����ӿ�Ϊeth0 */
    firewall_rule->fw_proto = 6;
    /* Э��Ϊ6����ʾTCPЭ�� */
    addr_aton("192.168.0.5", &firewall_rule->fw_src);
    /* ԴIP��ַ */
    addr_aton("192.168.0.4", &firewall_rule->fw_dst);
    /* Ŀ��IP��ַ */
    fw_add(firewall_handle, firewall_rule);
    /* ��ӹ��� */
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ��������Ϊ��ֹ */
    firewall_rule->fw_dir = FW_DIR_OUT;
    /* ����Ϊ��ȥ */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ����ӿ�Ϊeth0 */
    firewall_rule->fw_proto = 6;
    /* Э��Ϊ6��ʾTCPЭ�� */
    addr_aton("192.168.0.5", &firewall_rule->fw_src);
    /* ԴIP��ַ */
    addr_aton("192.168.0.4", &firewall_rule->fw_dst);
    /* Ŀ��IP��ַ */
    fw_delete(firewall_handle, firewall_rule);
    /* ɾ������ */
    fw_close(firewall_handle);
    /* �رշ���ǽ���� */
}
