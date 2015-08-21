#include "nids.h"
#include "stdio.h"
#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"libnids.lib")
#pragma comment(lib,"ws2_32.lib")
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
�����Ƿ���FTPЭ��Ļص�����
=======================================================================================================================
 */
void ftp_protocol_callback(struct tcp_stream *ftp_connection, void **arg)
{
    int i;
    char address_string[1024];
    char content[65535];
    struct tuple4 ip_and_port = ftp_connection->addr;
    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
    strcat(address_string, " <---> ");
    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
    strcat(address_string, "\n");
    switch (ftp_connection->nids_state)
    {
        case NIDS_JUST_EST:
            if ((ftp_connection->addr.dest == 21) || (ftp_connection->addr.source == 20))
            {
                /* FTP�ͻ��˺�FTP�������˽������� */
                ftp_connection->client.collect++;
                /* FTP�ͻ��˽������� */
                ftp_connection->server.collect++;
                /* FTP�������������� */
                ftp_connection->server.collect_urg++;
                /* FTP���������ս������� */
                ftp_connection->client.collect_urg++;
                /* FTP�ͻ��˽��ս������� */
                if (ftp_connection->addr.dest == 21)
                    printf("%s FTP�ͻ�����FTP������������������\n", address_string);
                if (ftp_connection->addr.source == 20)
                    printf("%s FTP��������FTP�ͻ��˽�����������\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* FTP�ͻ�����FTP�����������������ر� */
            printf("--------------------------------\n");
            if (ftp_connection->addr.dest == 21)
                printf("%sFTP�ͻ�����FTP�������Ŀ������������ر�\n", address_string);
            if (ftp_connection->addr.source == 20)
                printf("%sFTP��������FTP�ͻ��˵��������������ر�\n", address_string);
            return ;
        case NIDS_RESET:
            /* FTP�ͻ�����FTP�����������ӱ�RST�ر� */
            printf("--------------------------------\n");
            if (ftp_connection->addr.source == 20)
                printf("%sFTP��������FTP�ͻ��˵��������ӱ�RESET�ر�\n", address_string);
            if (ftp_connection->addr.dest == 21)
                printf("%sFTP�ͻ�����FTP�������Ŀ������ӱ�REST�ر�\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* FTPЭ�����µ����ݴﵽ */
                struct half_stream *hlf;
                if (ftp_connection->server.count_new_urg)
                {
                    /* FTP���������յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " urgent---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = ftp_connection->server.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (ftp_connection->client.count_new_urg)
                {
                    /* FTP�ͻ��˽��յ��µĽ������� */
                    printf("--------------------------------\n");
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
                    strcat(address_string, " <--- urgent ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    address_string[strlen(address_string) + 1] = 0;
                    address_string[strlen(address_string)] = ftp_connection->client.urgdata;
                    printf("%s", address_string);
                    return ;
                }
                if (ftp_connection->client.count_new)
                {
                    /* FTP�ͻ��˽��յ��µ����� */
                    hlf = &ftp_connection->client;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " <--- ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* ���FTP�ͻ��˽��յ����µ����� */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (ftp_connection->addr.source == 20)
                    {
                        printf("���������Ϊ:\n");
                        for (i = 0; i < hlf->count_new; i++)
                        {
                            printf("%s", char_to_ascii(content[i]));
                        }
                        printf("\n");
                    }
                    else
                    {
                        if (content[0] == '1' || content[0] == '2' || content[0] == '3' || content[0] == '4' || content[0] == '5')
                            printf("FTP��������Ӧ״̬����Ϊ��%c%c%c\n", content[0], content[1], content[2]);
                        if (strncmp(content, "220", 3) == 0)
                            printf("�����ӵ��û��ķ����Ѿ�׼������\n");
                        if (strncmp(content, "110", 3) == 0)
                            printf("�������Ӧ��\n");
                        if (strncmp(content, "120", 3) == 0)
                            printf("��ʾ ������nnn�����ڿ���\n");
                        if (strncmp(content, "125", 3) == 0)
                            printf("��ʾ���������Ѵ򿪣�׼������\n");
                        if (strncmp(content, "150", 3) == 0)
                            printf("��ʾ�ļ�״̬��ȷ�����ڴ���������\n");
                        if (strncmp(content, "200", 3) == 0)
                            printf("��ʾ��������ִ��\n");
                        if (strncmp(content, "202", 3) == 0)
                            printf("��ʾ����δ��ִ�У���վ�㲻֧�ִ�����\n");
                        if (strncmp(content, "211", 3) == 0)
                            printf("��ʾϵͳ״̬��ϵͳ������Ӧ\n");
                        if (strncmp(content, "212", 3) == 0)
                            printf("��ʾĿ¼״̬��Ϣ\n");
                        if (strncmp(content, "213", 3) == 0)
                            printf("��ʾ�ļ�״̬��Ϣ\n");
                        if (strncmp(content, "214", 3) == 0)
                            printf("��ʾ������Ϣ\n");
                        if (strncmp(content, "215", 3) == 0)
                            printf("��ʾ����ϵͳ����\n");
                        if (strncmp(content, "221", 3) == 0)
                            printf("��ʾ���������Ѿ����ر�\n");
                        if (strncmp(content, "225", 3) == 0)
                            printf("��ʾ���������Ѿ��򿪣�û�����ݴ���\n");
                        if (strncmp(content, "226", 3) == 0)
                            printf("��ʾ���������Ѿ��رգ������ļ������ɹ����\n");
                        if (strncmp(content, "227", 3) == 0)
                            printf("��ʾ���뱻��ģ\n");
                        if (strncmp(content, "230", 3) == 0)
                            printf("��ʾ�û��Ѿ���¼\n");
                        if (strncmp(content, "250", 3) == 0)
                            printf("��ʾ�����ļ������Ѿ��ɹ�ִ��\n");
                        if (strncmp(content, "257", 3) == 0)
                            printf("��ʾ����·������\n");
                        if (strncmp(content, "331", 3) == 0)
                            printf("��ʾ�û�����ȷ����Ҫ��������\n");
                        if (strncmp(content, "332", 3) == 0)
                            printf("��ʾ ��¼ʱ��Ҫ�ʻ���Ϣ\n");
                        if (strncmp(content, "350", 3) == 0)
                            printf("��ʾ��������ļ�������Ҫ�����ָ��\n");
                        if (strncmp(content, "421", 3) == 0)
                            printf("��ʾ���񲻿��ã��رտ�������\n");
                        if (strncmp(content, "425", 3) == 0)
                            printf("��ʾ���������Ӳ���ʧ��\n");
                        if (strncmp(content, "426", 3) == 0)
                            printf("��ʾ�ر����ӣ���ֹ����\n");
                        if (strncmp(content, "450", 3) == 0)
                            printf("��ʾ������ļ�����δ��ִ��\n");
                        if (strncmp(content, "451", 3) == 0)
                            printf("��ʾ���������ֹ���б��ش�����\n");
                        if (strncmp(content, "452", 3) == 0)
                            printf("��ʾδִ������Ĳ�����ϵͳ�洢�ռ䲻�� ���ļ�������\n");
                        if (strncmp(content, "500", 3) == 0)
                            printf("��ʾ�﷨�����������\n");
                        if (strncmp(content, "501", 3) == 0)
                            printf("��ʾ�������﷨����\n");
                        if (strncmp(content, "502", 3) == 0)
                            printf("��ʾ����δ��ִ��\n");
                        if (strncmp(content, "503", 3) == 0)
                            printf("��ʾ����˳��������\n");
                        if (strncmp(content, "504", 3) == 0)
                            printf("��ʾ���ڲ�����������������\n");
                        if (strncmp(content, "530", 3) == 0)
                            printf("��ʾδ��¼\n");
                        if (strncmp(content, "532", 3) == 0)
                            printf("��ʾ�洢�ļ���Ҫ�ʻ���Ϣ\n");
                        if (strncmp(content, "550", 3) == 0)
                            printf("��ʾδִ������Ĳ������ļ�������\n");
                        if (strncmp(content, "551", 3) == 0)
                            printf("��ʾ���������ֹ��ҳ������δ֪\n");
                        if (strncmp(content, "552", 3) == 0)
                            printf("��ʾ������ļ�������ֹ�������洢����ռ�\n");
                        if (strncmp(content, "553", 3) == 0)
                            printf("��ʾδִ������Ĳ������ļ������Ϸ�\n");
                        for (i = 0; i < hlf->count_new; i++)
                        {
                            printf("%s", char_to_ascii(content[i]));
                        }
                        printf("\n");
                    }
                }
                else
                {
                    /* FTP���������յ��µ����� */
                    hlf = &ftp_connection->server;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " ---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* ���FTP�������˽��յ����µ����� */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (ftp_connection->addr.source == 20)
                        printf("FTP��������FTP�ͻ��˷�������\n");
                    else
                    {
                        if (strstr(content, "USER"))
                            printf("�û�����Ϊ��USER��:%s\n", content + strlen("USER"));
                        else if (strstr(content, "PASS"))
                            printf("�û�����Ϊ��PASS��:%s\n", content + strlen("PASS"));
                        else if (strstr(content, "PORT"))
                            printf("�˿ڲ���Ϊ��PORT��:%s\n", content + strlen("PORT"));
                        else if (strstr(content, "LIST"))
                            printf("��ʾ�ļ��б�LIST��:%s\n", content + strlen("LIST"));
                        else if (strstr(content, "CWD"))
                            printf("�ı乤��Ŀ¼Ϊ��CWD��:%s\n", content + strlen("CWD"));
                        else if (strstr(content, "TYPE"))
                            printf("����Ϊ��TYPE��:%s\n", content + strlen("TYPE"));
                        else if (strstr(content, "RETR"))
                            printf("��ȡ�ļ�Ϊ��RETR��:%s\n", content + strlen("RETR"));
                        else if (strstr(content, "STOR"))
                            printf("�����ļ�Ϊ��STOR��:%s\n", content + strlen("STOR"));
                        else if (strstr(content, "XRMD"))
                            printf("ɾ��Ŀ¼��XRMD��:%s\n", content + strlen("XRMD"));
                        else if (strstr(content, "QUIT"))
                            printf("�˳���½��QUIT��:%s\n", content + strlen("QUIT"));
                        else
                            printf("FTP�ͻ���ʹ�õ�����Ϊ %c%c%c%c\n", content[0], content[1], content[2], content[3]);
                    }
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
	nids_params.device = "2";
    if (!nids_init())
     /* Libnids��ʼ�� */
    {
        printf("���ִ���%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(ftp_protocol_callback);
    /* ע��ص����� */
    nids_run();
    /* ����ѭ���������ݰ�״̬ */
}
