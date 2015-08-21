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
�����Ƿ���TelnetЭ��Ļص�����
=======================================================================================================================
 */
void telnet_protocol_callback(struct tcp_stream *telnet_connection, void **arg)
{
    int i;
    char address_string[1024];
    char content[65535];
    char content_urgent[65535];
    struct tuple4 ip_and_port = telnet_connection->addr;
    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
    strcat(address_string, " <---> ");
    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
    strcat(address_string, "\n");
    switch (telnet_connection->nids_state)
    {
        case NIDS_JUST_EST:
            if (telnet_connection->addr.dest == 23)
            {
                /* Telnet�ͻ��˺�Telnet�������˽������� */
                telnet_connection->client.collect++;
                /* Telnet�ͻ��˽������� */
                telnet_connection->server.collect++;
                /* Telnet�������������� */
                telnet_connection->server.collect_urg++;
                /* Telnet���������ս������� */
                telnet_connection->client.collect_urg++;
                /* Telnet�ͻ��˽��ս������� */
                printf("%sTELNET�ͻ�����TELNET��������������\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* TelnetЭ�����������ر� */
            printf("--------------------------------\n");
            printf("%sTELNET�ͻ�����TELNET���������������ر�\n", address_string);
            return ;
        case NIDS_RESET:
            /* TelnetЭ�����ӱ�RST�ر� */
            printf("--------------------------------\n");
            printf("%sTELNET�ͻ�����TELNET���������ӱ�REST�ر�\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* TelnetЭ�����µ����ݴﵽ */
                struct half_stream *hlf;
                if (telnet_connection->server.count_new_urg)
                {
                    /* Telnet���������յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " urgent---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = telnet_connection->server.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (telnet_connection->client.count_new_urg)
                {
                    /* Telnet�ͻ��˽��յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " <--- urgent ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = telnet_connection->client.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (telnet_connection->client.count_new)
                {
                    /* Telnet�ͻ��˽��յ��µ����� */
                    hlf = &telnet_connection->client;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " <--- ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* ���Telnet�ͻ��˽��յ����µ����� */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                }
                else
                {
                    /* Telnet���������յ��µ����� */
                    hlf = &telnet_connection->server;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " ---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* ���Telnet���������յ����µ����� */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
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
        printf("%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(telnet_protocol_callback);
    /* ע�����TelnetЭ��Ļص����� */
    nids_run();
    /* ����ѭ���������ݰ���״̬ */
}
