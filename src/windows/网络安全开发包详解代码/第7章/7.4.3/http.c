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
    {
        *string++ = ch;
    }
    else if (ch == ' ')
    {
        *string++ = ch;
    }
    else if (ch == '\n' || ch == '\r')
    {
        *string++ = ch;
    }
    else
    {
        *string++ = '.';
    }
    *string = 0;
    return ascii_string;
}
/*
=======================================================================================================================
����ĺ����Ƕ���������յ����ݽ��з���
=======================================================================================================================
 */
void parse_client_data(char content[], int number)
{
    char temp[1024];
    char str1[1024];
    char str2[1024];
    char str3[1024];
    int i;
    int k;
    int j;
    char entity_content[1024];
    if (content[0] != 'H' && content[1] != 'T' && content[2] != 'T' && content[3] != 'P')
    {
        printf("ʵ������Ϊ��������\n");
        for (i = 0; i < number; i++)
        {
            printf("%s", char_to_ascii(content[i]));
        }
        printf("\n");
    }
    else
    {
        for (i = 0; i < strlen(content); i++)
        {
            if (content[i] != '\n')
            {
                k++;
                continue;
            }
            for (j = 0; j < k; j++)
                temp[j] = content[j + i - k];
            temp[j] = '\0';
            if (strstr(temp, "HTTP"))
            {
                printf("״̬��Ϊ��");
                printf("%s\n", temp);
                sscanf(temp, "%s %s %s", str1, str2);
                printf("HTTPЭ��Ϊ:%s\n", str1);
                printf("״̬����Ϊ:%s\n", str2);
            }
            if (strstr(temp, "Date"))
            {
                printf("��ǰ��ʱ��Ϊ��Date��:%s\n", temp + strlen("Date:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Server"))
            {
                printf("������Ϊ��Server��:%s\n", temp + strlen("Server:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Cache-Control"))
            {
                printf("�������Ϊ��Cache-Control��:%s\n", temp + strlen("Cache-Control:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Expires"))
            {
                printf("��Դ����Ϊ��Expires��:%s\n", temp + strlen("Expires:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Last-Modified"))
            {
                printf("���һ���޸ĵ�ʱ��Ϊ��Last-Modified��:%s\n", temp + strlen("Last-Modified:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "ETag"))
            {
                printf("EtagΪ��ETag��:%s\n", temp + strlen("Etag:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Accept-Ranges"))
            {
                printf("Accept-Ranges��Accept-Ranges��:%s\n", temp + strlen("Accept-Ranges:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Content-Length"))
            {
                printf("���ݳ���Ϊ��Content-Length��:%s\n", temp + strlen("Content-Length:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Connection"))
            {
                printf("����״̬Ϊ��Connection��:%s\n", temp + strlen("Connection:"));
                printf("%s\n", temp);
            }
            if (strstr(temp, "Content-Type"))
            {
                printf("��������Ϊ��Content-Type��:%s\n", temp + strlen("Content-Type:"));
                printf("%s\n", temp);
            }
            /* ��ȡʵ������ */
            if ((content[i] == '\n') && (content[i + 1] == '\r'))
            {
                if (i + 3 == strlen(content))
                {
                    printf("��ʵ������\n");
                    break;
                }
                for (j = 0; j < number - i - 3; j++)
                    entity_content[j] = content[i + 3+j];
                entity_content[j] = '\0';
                printf("ʵ������Ϊ��\n");
                for (i = 0; i < j; i++)
                {
                    printf("%s", char_to_ascii(entity_content[i]));
                }
                printf("\n");
                break;
            }
            k = 0;
        }
    }
}
/*
=======================================================================================================================
����ĺ����Ƕ�WEB���������յ������ݽ��з���
=======================================================================================================================
 */
void parse_server_data(char content[], int number)
{
    char temp[1024];
    char str1[1024];
    char str2[1024];
    char str3[1024];
    int i;
    int k;
    int j;
    char entity_content[1024];
    for (i = 0; i < strlen(content); i++)
    {
        if (content[i] != '\n')
        {
            k++;
            continue;
        }
        for (j = 0; j < k; j++)
            temp[j] = content[j + i - k];
        temp[j] = '\0';
        if (strstr(temp, "GET"))
        {
            printf("������Ϊ��");
            printf("%s\n", temp);
            sscanf(temp, "%s %s %s", str1, str2, str3);
            printf("ʹ�õ�����Ϊ:%s\n", str1);
            printf("��õ���ԴΪ:%s\n", str2);
            printf("HTTPЭ������Ϊ:%s\n", str3);
        }
        if (strstr(temp, "Accept:"))
        {
            printf("���յ��ļ�������Accept:��:%s\n", temp + strlen("Accept:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Referer"))
        {
            printf("ת�Ƶ�ַΪ��Referer��:%s\n", temp + strlen("Referer:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Accept-Language"))
        {
            printf("ʹ�õ�����Ϊ��Accept-Language��:%s\n", temp + strlen("Accept-Language:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Accept-Encoding"))
        {
            printf("���յı��뷽ʽΪ��Accept-Encoding��:%s\n", temp + strlen("Accept-Encoding:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "If-Modified-Since"))
        {
            printf("�ϴ��޸�ʱ��Ϊ��If-Modified-Since��:%s\n", temp + strlen("If-Modified-Since:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "If-None-Match"))
        {
            printf("If-None-MatchΪ��If-Modified-Since��:%s\n", temp + strlen("If-None-Match:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "User-Agent"))
        {
            printf("�û����������ϢΪ��User-Agent��:%s\n", temp + strlen("User-Agent:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Host"))
        {
            printf("���ʵ�����Ϊ��Host��:%s\n", temp + strlen("Host:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Connection"))
        {
            printf("����״̬Ϊ��Connection��:%s\n", temp + strlen("Connection:"));
            printf("%s\n", temp);
        }
        if (strstr(temp, "Cookie"))
        {
            printf("CookieΪ��Cookie��:%s\n", temp + strlen("Cookie:"));
            printf("%s\n", temp);
        }
        /* ��ȡʵ������ */
        if ((content[i] == '\n') && (content[i + 1] == '\r') && (content[i + 2] == '\n'))
        {
            if (i + 3 == strlen(content))
            {
                printf("��ʵ������\n");
                break;
            }
            for (j = 0; j < strlen(content) - i - 3; j++)
                entity_content[j] = content[i + 3+j];
            entity_content[j] = '\0';
            printf("ʵ������Ϊ��\n");
            printf("%s", entity_content);
            printf("\n");
            break;
        }
        k = 0;
    }
}
/*
=======================================================================================================================
�����ǻص�������ʵ�ֶ�HTTPЭ��ķ���
=======================================================================================================================
 */
void http_protocol_callback(struct tcp_stream *tcp_http_connection, void **param)
{
    char address_content[1024];
    char content[65535];
    char content_urgent[65535];
    struct tuple4 ip_and_port = tcp_http_connection->addr;
    strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_content + strlen(address_content), " : %i", ip_and_port.source);
    strcat(address_content, " <----> ");
    strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_content + strlen(address_content), " : %i", ip_and_port.dest);
    strcat(address_content, "\n");
    if (tcp_http_connection->nids_state == NIDS_JUST_EST)
    {
        if (tcp_http_connection->addr.dest != 80)
         /* ֻ����HTTPЭ�����ݰ� */
        {
            return ;
        }
        tcp_http_connection->client.collect++; /* ������������� */
        tcp_http_connection->server.collect++; /* WEB�������˽������� */
        printf("\n\n\n==============================================\n");
        printf("%s ��������...\n", address_content);
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_CLOSE)
    {
        printf("--------------------------------\n");
        printf("%s���������ر�...\n", address_content);
        /* ���������ر� */
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_RESET)
    {
        printf("--------------------------------\n");
        printf("%s���ӱ�RST�ر�...\n", address_content);
        /* ���ӱ�RST�ر� */
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_DATA)
    {
        struct half_stream *hlf;
        if (tcp_http_connection->client.count_new)
         /* ������������� */
        {
            hlf = &tcp_http_connection->client;
            /* hlft��ʾ��������յ����� */
            strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
            sprintf(address_content + strlen(address_content), ":%i", ip_and_port.source);
            strcat(address_content, " <---- ");
            strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
            sprintf(address_content + strlen(address_content), ":%i", ip_and_port.dest);
            strcat(address_content, "\n");
            printf("\n");
            printf("%s", address_content);
            printf("�������������...\n");
            printf("\n");
            memcpy(content, hlf->data, hlf->count_new);
            content[hlf->count_new] = '\0';
            parse_client_data(content, hlf->count_new);
            /* ������������յ����� */
        }
        else
        {
            hlf = &tcp_http_connection->server;
            /* hlf��ʾWeb��������TCP���Ӷ� */
            strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
            sprintf(address_content + strlen(address_content), " : %i", ip_and_port.source);
            strcat(address_content, " ----> ");
            strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
            sprintf(address_content + strlen(address_content), ":%i", ip_and_port.dest);
            strcat(address_content, "\n");
            printf("\n");
            printf("%s", address_content);
            printf("��������������...\n");
            printf("\n");
            memcpy(content, hlf->data, hlf->count_new);
            content[hlf->count_new] = '\0';
            parse_server_data(content, hlf->count_new);
            /* ����WEB���������յ����� */
        }
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
    nids_register_tcp(http_protocol_callback);
    /* ע��ص����� */
    nids_run(); /* ����ѭ���������ݰ�״̬ */
}
