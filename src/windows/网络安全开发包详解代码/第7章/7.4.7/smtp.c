#include "nids.h"
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
        *string++ = ch;
    else if (ch == ' ')
        *string++ = ch;
    else if (ch == '\n' || ch == '\r')
        *string++ = ch;
    else
        *string++ = '.';
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
�����Ƿ���SMTPЭ��Ļص�����
=======================================================================================================================
 */
void smtp_protocol_callback(struct tcp_stream *smtp_connection, void **arg)
{
    int i;
    char address_string[1024];
    char content[65535];
    char content_urgent[65535];
    struct tuple4 ip_and_port = smtp_connection->addr;
    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
    strcat(address_string, " <---> ");
    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
    strcat(address_string, "\n");
    switch (smtp_connection->nids_state)
    {
        case NIDS_JUST_EST:
            if (smtp_connection->addr.dest == 25)
            {
                /* SMTP�ͻ��˺�SMTP�������˽������� */
                smtp_connection->client.collect++;
                /* SMTP�ͻ��˽������� */
                smtp_connection->server.collect++;
                /* SMTP�������������� */
                smtp_connection->server.collect_urg++;
                /* SMTP���������ս������� */
                smtp_connection->client.collect_urg++;
                /* SMTP�ͻ��˽��ս������� */
                printf("%sSMTP���ͷ���SMTP���շ���������\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* SMTP�ͻ�����SMTP���������������ر� */
            printf("--------------------------------\n");
            printf("%sSMTP���ͷ���SMTP���շ����������ر�\n", address_string);
            return ;
        case NIDS_RESET:
            /* SMTP�ͻ�����SMTP���������ӱ�RST�ر� */
            printf("--------------------------------\n");
            printf("%sSMTP���ͷ���SMTP���շ����ӱ�REST�ر�\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* SMTPЭ����յ��µ����� */
                char status_code[4];
                struct half_stream *hlf;
                if (smtp_connection->server.count_new_urg)
                {
                    /* SMTP���������յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " urgent---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = smtp_connection->server.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (smtp_connection->client.count_new_urg)
                {
                    /* SMTP�ͻ��˽��յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " <--- urgent ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = smtp_connection->client.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (smtp_connection->client.count_new)
                {
                    /* SMTP�ͻ��˽��յ��µ����� */
                    hlf = &smtp_connection->client;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " <--- ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (strstr(strncpy(status_code, content, 3), "221"))
                        printf("������ֹ\n");
                    if (strstr(strncpy(status_code, content, 3), "250"))
                        printf("�����ɹ�\n");
                    if (strstr(strncpy(status_code, content, 3), "220"))
                        printf("��ʾ�������\n");
                    if (strstr(strncpy(status_code, content, 3), "354"))
                        printf("��ʼ�ʼ����룬��\".\"����\n");
                    if (strstr(strncpy(status_code, content, 3), "334"))
                        printf("��������Ӧ��֤\n");
                    if (strstr(strncpy(status_code, content, 3), "235"))
                        printf("��֤�ɹ����Է����ʼ���\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                }
                else
                {
                    /* SMTP���������յ��µ����� */
                    hlf = &smtp_connection->server;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " ---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (strstr(content, "EHLO"))
                        printf("HELLO����\n");
                    if (strstr(content, "QUIT"))
                        printf("�˳�����\n");
                    if (strstr(content, "DATA"))
                        printf("��ʼ��������\n");
                    if (strstr(content, "MAIL FROM"))
                        printf("���ͷ��ʼ���ַΪ\n");
                    if (strstr(content, "RCPT TO"))
                        printf("���շ��ʼ���ַΪ\n");
                    if (strstr(content, "AUTH"))
                        printf("������֤\n");
                    if (strstr(content, "LOGIN"))
                        printf("��֤����ΪLOGIN\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                    if (strstr(content, "\n."))
                        printf("���ݴ������\n");
                }
            }
        default:
            break;
    }
    return ;
}
/*
=======================================================================================================================
������
=======================================================================================================================
 */
void main()
{
    if (!nids_init())
     /* Libnids��ʼ�� */
    {
        printf("%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(smtp_protocol_callback);
    /* ע�����TCPЭ��Ļص����� */
    nids_run();
    /* ����ѭ���������ݰ�״̬ */
}
