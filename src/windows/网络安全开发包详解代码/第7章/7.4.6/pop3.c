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
�����Ƿ���POP3Э��Ļص�����
=======================================================================================================================
 */
void pop3_protocol_callback(struct tcp_stream *pop3_connection, void **arg)
{
    int i;
    char address_string[1024];
    char content[65535];
    char content_urgent[65535];
    struct tuple4 ip_and_port = pop3_connection->addr;
    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
    strcat(address_string, " <---> ");
    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
    strcat(address_string, "\n");
    switch (pop3_connection->nids_state)
    {
        case NIDS_JUST_EST:
            if (pop3_connection->addr.dest == 110)
            {
                /*POP3�ͻ��˺�POP3�������˽������� */
                pop3_connection->client.collect++;
                /* POP3�ͻ��˽������� */
                pop3_connection->server.collect++;
                /* POP3�������������� */
                pop3_connection->server.collect_urg++;
                /* POP3���������ս������� */
                pop3_connection->client.collect_urg++;
                /* POP3�ͻ��˽��ս������� */
                printf("%sPOP3�ͻ�����POP3��������������\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* POP3�ͻ�����POP3�����������������ر� */
            printf("--------------------------------\n");
            printf("%sPOP3�ͻ�����POP3���������������ر�\n", address_string);
            return ;
        case NIDS_RESET:
            /* POP3�ͻ�����POP3�����������ӱ�RST�ر� */
            printf("--------------------------------\n");
            printf("%sPOP3�ͻ�����POP3���������ӱ�REST�ر�\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* POP3Э����յ��µ����� */
                char status_code[5];
                struct half_stream *hlf;
                if (pop3_connection->server.count_new_urg)
                {
                    /* POP3���������յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " urgent---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = pop3_connection->server.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (pop3_connection->client.count_new_urg)
                {
                    /* POP3���������յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " <--- urgent ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = pop3_connection->client.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (pop3_connection->client.count_new)
                {
                    /* POP3�ͻ��˽��յ��µ����� */
                    hlf = &pop3_connection->client;
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
                    if (strstr(strncpy(status_code, content, 4), "+OK"))
                        printf("�����ɹ�\n");
                    if (strstr(strncpy(status_code, content, 4), "-ERR"))
                        printf("����ʧ��\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                    if (strstr(content, "\n\r.\n\r"))
                        printf("���ݴ������\n");
                }
                else
                {
                    /* POP3���������յ��µ����� */
                    hlf = &pop3_connection->server;
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
                    if (strstr(content, "USER"))
                        printf("�ʼ��û���Ϊ\n");
                    if (strstr(content, "PASS"))
                        printf("�û�����Ϊ\n");
                    if (strstr(content, "STAT"))
                        printf("����ͳ������\n");
                    if (strstr(content, "LIST"))
                        printf("�����ʼ������ʹ�С\n");
                    if (strstr(content, "RETR"))
                        printf("��ȡ�ʼ�\n");
                    if (strstr(content, "DELE"))
                        printf("ɾ���ʼ�\n");
                    if (strstr(content, "QUIT"))
                        printf("�˳�����\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
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
        printf("���ִ���%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(pop3_protocol_callback);
    /* ע�����POP3Э��Ļص����� */
    nids_run();
    /* ����ѭ���������ݰ���״̬ */
}
