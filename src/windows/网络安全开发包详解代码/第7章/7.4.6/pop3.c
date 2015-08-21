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
下面是分析POP3协议的回调函数
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
                /*POP3客户端和POP3服务器端建立连接 */
                pop3_connection->client.collect++;
                /* POP3客户端接收数据 */
                pop3_connection->server.collect++;
                /* POP3服务器接收数据 */
                pop3_connection->server.collect_urg++;
                /* POP3服务器接收紧急数据 */
                pop3_connection->client.collect_urg++;
                /* POP3客户端接收紧急数据 */
                printf("%sPOP3客户端与POP3服务器建立连接\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* POP3客户端与POP3服务器端连接正常关闭 */
            printf("--------------------------------\n");
            printf("%sPOP3客户端与POP3服务器连接正常关闭\n", address_string);
            return ;
        case NIDS_RESET:
            /* POP3客户端与POP3服务器端连接被RST关闭 */
            printf("--------------------------------\n");
            printf("%sPOP3客户端与POP3服务器连接被REST关闭\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* POP3协议接收到新的数据 */
                char status_code[5];
                struct half_stream *hlf;
                if (pop3_connection->server.count_new_urg)
                {
                    /* POP3服务器接收到新的紧急数据 */
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
                    /* POP3服务器接收到新的紧急数据 */
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
                    /* POP3客户端接收到新的数据 */
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
                        printf("操作成功\n");
                    if (strstr(strncpy(status_code, content, 4), "-ERR"))
                        printf("操作失败\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                    if (strstr(content, "\n\r.\n\r"))
                        printf("数据传输结束\n");
                }
                else
                {
                    /* POP3服务器接收到新的数据 */
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
                        printf("邮件用户名为\n");
                    if (strstr(content, "PASS"))
                        printf("用户密码为\n");
                    if (strstr(content, "STAT"))
                        printf("返回统计资料\n");
                    if (strstr(content, "LIST"))
                        printf("返回邮件数量和大小\n");
                    if (strstr(content, "RETR"))
                        printf("获取邮件\n");
                    if (strstr(content, "DELE"))
                        printf("删除邮件\n");
                    if (strstr(content, "QUIT"))
                        printf("退出连接\n");
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
        printf("出现错误：%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(pop3_protocol_callback);
    /* 注册分析POP3协议的回调函数 */
    nids_run();
    /* 进入循环捕获数据包的状态 */
}
