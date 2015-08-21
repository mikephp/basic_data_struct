#include <dnet.h>
/*
=======================================================================================================================
LibdnetÍ·ÎÄ¼þ ;
ÏÂÃæÊÇ¶¨ÒåµÄ»Øµ÷º¯Êý£¬ËüÊµÏÖÁËÏÔÊ¾·À»ðÇ½¹æÔòµÄ¹¦ÄÜ
=======================================================================================================================
 */
int show_firewall_fules(const struct fw_rule *firewall_rule, void *parameter)
{
    char operation[100];
    /* ²Ù×÷£¬±íÊ¾ÊÇ½ûÖ¹»¹ÊÇÔÊÐí */
    char direction[100];
    /* ·½Ïò£¬±íÊ¾ÊÇ½øÈë»¹ÊÇ³öÈ¥ */
    char net_interface[100];
    /* ÍøÂç½Ó¿Ú */
    char source_ip[100];
    /* Ô´IPµØÖ· */
    char destination_ip[100];
    /* Ä¿µÄIPµØÖ· */
    char protocol[16];
    /* ¹æÔòÐ­ÒéÀàÐÍ£¬ÔÚ´ËÖ»·ÖÎöTCPÐ­ÒéºÍUDPÐ­Òé */
    char source_port[16];
    /* Ô´¶Ë¿ÚºÅ */
    char destination_port[16];
    /* Ä¿µÄ¶Ë¿ÚºÅ */
    static int count = 1;
    /* ¹æÔò¸öÊý */
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
         /* ±íÊ¾Ð­ÒéÊÇTCPÐ­Òé */
            sprintf(protocol, "%s ", "tcp");
        if (firewall_rule->fw_proto == 17)
         /* ±íÊ¾Ð­ÒéÊÇUDPÐ­Òé */
            sprintf(protocol, "%s ", "udp");
    }
    if (firewall_rule->fw_proto == 6 || firewall_rule->fw_proto == 17)
    {
        /* ÇóÔ´¶Ë¿ÚºÅ */
        if (firewall_rule->fw_sport[0] == firewall_rule->fw_sport[1])
        /* ±íÊ¾¶Ë¿ÚºÅÖ»ÓÐÒ»¸öÖµ */
        {
            sprintf(source_port, "port = %d", firewall_rule->fw_sport[0]);
        }
        else
        /* ±íÊ¾¶Ë¿ÚºÅÊÇÒ»¸ö·¶Î§£¬ÀýÈç´Ó1025µ½65535 */
            sprintf(source_port, "port %d-%d", firewall_rule->fw_sport[0], firewall_rule->fw_sport[1]);
        /* ÇóÄ¿µÄ¶Ë¿ÚºÅ */
        if (firewall_rule->fw_dport[0] == firewall_rule->fw_dport[1])
        {
            sprintf(destination_port, "port = %d", firewall_rule->fw_dport[0]);
        }
        else
            sprintf(destination_port, "port %d-%d", firewall_rule->fw_dport[0], firewall_rule->fw_dport[1]);
    }
    if (firewall_rule->fw_op == FW_OP_ALLOW)
     /* ÅÐ¶Ï²Ù×÷ÀàÐÍ */
        sprintf(operation, "%s", "pass");
    /* ²Ù×÷ÀàÐÍÊÇÔÊÐí */
    else
        sprintf(operation, "%s", "block");
     / ² Ù × ÷ À à Ð Í Ê Ç ½ û Ö ¹ * / if (firewall_rule->fw_dir == FW_DIR_IN)
         /* ÅÐ¶Ï·½Ïò */sprintf(direction, "%s", "in");
    /* ±íÊ¾½øÈë */
    else
        strcpy(direction, "out");
    /* ±íÊ¾³öÈ¥ */
    sprintf(net_interface, "%s", firewall_rule->fw_device);
    /* »ñµÃÍøÂç½Ó¿Ú */
    if (firewall_rule->fw_src.addr_type)
        sprintf(source_ip, "from %s", addr_ntoa(&firewall_rule->fw_src));
    /* »ñµÃÔ´IPµØÖ· */
    else
        sprintf(source_ip, "%s", "");
    if (firewall_rule->fw_dst.addr_type)
        sprintf(destination_ip, "to %s", addr_ntoa(&firewall_rule->fw_dst));
    /* »ñµÃÄ¿µÄIPµØÖ· */
    else
        sprintf(destination_ip, "");
    printf("µÚ%dÌõ¹æÔò£º\n", count);
    printf("%s %s %s %s %s %s %s %s %s \n", operation,  /* ²Ù×÷ÀàÐÍ */
    direction,  /* ·½Ïò */
    net_interface,  /* ÍøÂç½Ó¿Ú */
    protocol,  /* Ð­ÒéÀàÐÍ */
    source_ip,  /* Ô´IPµØÖ· */
    source_port,  /* Ô´¶Ë¿ÚºÅ */
    destination_ip,  /* Ä¿µÄIPµØÖ· */
    destination_port,  /* Ä¿µÄ¶Ë¿ÚºÅ */
    "");
    /* Êä³öÒ»¸ö¹æÔò */
    count++;
    /* ¹æÔò¸öÊý¼Ó1 */
    return (0);
}
void main()
{
    struct fw_rule *firewall_rule;
    /* ¶¨Òå·À»ðÇ½¹æÔò±äÁ¿ */
    fw_t *firewall_handle;
    /* ¶¨Òå·À»ðÇ½¾ä±ú */
    if ((firewall_handle = fw_open()) == NULL)
     /* ´ò¿ª·À»ðÇ½²Ù×÷ */
        printf("´ò¿ª·À»ðÇ½´íÎó\n");
    fw_loop(firewall_handle,  /* ·À»ðÇ½¾ä±ú */show_firewall_fules,  /* »Øµ÷º¯ÊýµÄÃû×Ö */NULL); /* ´«µÝ¸ø»Øµ÷º¯ÊýµÄ²ÎÊý */
    /* ×¢²á»Øµ÷º¯Êý£¬¶ÔÃ¿¸ö¹æÔòÑ­»·µ÷ÓÃ»Øµ÷º¯Êý£¬Êä³öËùÓÐ¹æÔòÐÅÏ¢ */
    firewall_rule = (struct fw_rule*)malloc(sizeof(struct fw_rule));
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ²Ù×÷ÀàÐÍÎª½ûÖ¹ */
    firewall_rule->fw_dir = FW_DIR_IN;
    /* ·½ÏòÎª½øÈë */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ÍøÂç½Ó¿ÚÎªeth0 */
    firewall_rule->fw_proto = 6;
    /* Ð­ÒéÎª6£¬±íÊ¾TCPÐ­Òé */
    addr_aton("192.168.0.4", &firewall_rule->fw_src);
    /* Ô´IPµØÖ· */
    addr_aton("192.168.0.5", &firewall_rule->fw_dst);
    /*
     * Ä¿µÄIPµØÖ·* fw_add(firewall_handle, firewall_rule);
     * /*Ìí¼Ó¹æÔò
     */
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ²Ù×÷ÀàÐÍÎª½ûÖ¹ */
    firewall_rule->fw_dir = FW_DIR_OUT;
    /* ·½ÏòÎª³öÈ¥ */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ÍøÂç½Ó¿ÚÎªeth0 */
    firewall_rule->fw_proto = 6;
    /* Ð­ÒéÎª6£¬±íÊ¾TCPÐ­Òé */
    addr_aton("192.168.0.5", &firewall_rule->fw_src);
    /* Ô´IPµØÖ· */
    addr_aton("192.168.0.4", &firewall_rule->fw_dst);
    /* Ä¿µÄIPµØÖ· */
    fw_add(firewall_handle, firewall_rule);
    /* Ìí¼Ó¹æÔò */
    firewall_rule->fw_op = FW_OP_BLOCK;
    /* ²Ù×÷ÀàÐÍÎª½ûÖ¹ */
    firewall_rule->fw_dir = FW_DIR_OUT;
    /* ·½ÏòÎª³öÈ¥ */
    strcpy(firewall_rule->fw_device, "eth0");
    /* ÍøÂç½Ó¿ÚÎªeth0 */
    firewall_rule->fw_proto = 6;
    /* Ð­ÒéÎª6±íÊ¾TCPÐ­Òé */
    addr_aton("192.168.0.5", &firewall_rule->fw_src);
    /* Ô´IPµØÖ· */
    addr_aton("192.168.0.4", &firewall_rule->fw_dst);
    /* Ä¿µÄIPµØÖ· */
    fw_delete(firewall_handle, firewall_rule);
    /* É¾³ý¹æÔò */
    fw_close(firewall_handle);
    /* ¹Ø±Õ·À»ðÇ½²Ù×÷ */
}
