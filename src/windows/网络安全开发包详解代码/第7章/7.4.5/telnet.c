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
下面是分析Telnet协议的回调函数
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
                /* Telnet客户端和Telnet服务器端建立连接 */
                telnet_connection->client.collect++;
                /* Telnet客户端接收数据 */
                telnet_connection->server.collect++;
                /* Telnet服务器接收数据 */
                telnet_connection->server.collect_urg++;
                /* Telnet服务器接收紧急数据 */
                telnet_connection->client.collect_urg++;
                /* Telnet客户端接收紧急数据 */
                printf("%sTELNET客户端与TELNET服务器建立连接\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* Telnet协议连接正常关闭 */
            printf("--------------------------------\n");
            printf("%sTELNET客户端与TELNET服务器连接正常关闭\n", address_string);
            return ;
        case NIDS_RESET:
            /* Telnet协议连接被RST关闭 */
            printf("--------------------------------\n");
            printf("%sTELNET客户端与TELNET服务器连接被REST关闭\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* Telnet协议有新的数据达到 */
                struct half_stream *hlf;
                if (telnet_connection->server.count_new_urg)
                {
                    /* Telnet服务器接收到新的紧急数据 */
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
                    /* Telnet客户端接收到新的紧急数据 */
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
                    /* Telnet客户端接收到新的数据 */
                    hlf = &telnet_connection->client;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " <--- ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* 输出Telnet客户端接收到的新的数据 */
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
                    /* Telnet服务器接收到新的数据 */
                    hlf = &telnet_connection->server;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " ---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* 输出Telnet服务器接收到的新的数据 */
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
主函数
=======================================================================================================================
 */
void main()
{
    if (!nids_init())
     /* Libnids初始化 */
    {
        printf("%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(telnet_protocol_callback);
    /* 注册分析Telnet协议的回调函数 */
    nids_run();
    /* 进入循环捕获数据包的状态 */
}
