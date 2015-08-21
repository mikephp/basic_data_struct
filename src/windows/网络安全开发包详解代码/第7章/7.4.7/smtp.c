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
下面是分析SMTP协议的回调函数
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
                /* SMTP客户端和SMTP服务器端建立连接 */
                smtp_connection->client.collect++;
                /* SMTP客户端接收数据 */
                smtp_connection->server.collect++;
                /* SMTP服务器接收数据 */
                smtp_connection->server.collect_urg++;
                /* SMTP服务器接收紧急数据 */
                smtp_connection->client.collect_urg++;
                /* SMTP客户端接收紧急数据 */
                printf("%sSMTP发送方与SMTP接收方建立连接\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* SMTP客户端与SMTP服务器连接正常关闭 */
            printf("--------------------------------\n");
            printf("%sSMTP发送方与SMTP接收方连接正常关闭\n", address_string);
            return ;
        case NIDS_RESET:
            /* SMTP客户端与SMTP服务器连接被RST关闭 */
            printf("--------------------------------\n");
            printf("%sSMTP发送方与SMTP接收方连接被REST关闭\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* SMTP协议接收到新的数据 */
                char status_code[4];
                struct half_stream *hlf;
                if (smtp_connection->server.count_new_urg)
                {
                    /* SMTP服务器接收到新的紧急数据 */
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
                    /* SMTP客户端接收到新的紧急数据 */
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
                    /* SMTP客户端接收到新的数据 */
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
                        printf("连接中止\n");
                    if (strstr(strncpy(status_code, content, 3), "250"))
                        printf("操作成功\n");
                    if (strstr(strncpy(status_code, content, 3), "220"))
                        printf("表示服务就绪\n");
                    if (strstr(strncpy(status_code, content, 3), "354"))
                        printf("开始邮件输入，以\".\"结束\n");
                    if (strstr(strncpy(status_code, content, 3), "334"))
                        printf("服务器响应验证\n");
                    if (strstr(strncpy(status_code, content, 3), "235"))
                        printf("认证成功可以发送邮件了\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                }
                else
                {
                    /* SMTP服务器接收到新的数据 */
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
                        printf("HELLO命令\n");
                    if (strstr(content, "QUIT"))
                        printf("退出连接\n");
                    if (strstr(content, "DATA"))
                        printf("开始传输数据\n");
                    if (strstr(content, "MAIL FROM"))
                        printf("发送方邮件地址为\n");
                    if (strstr(content, "RCPT TO"))
                        printf("接收方邮件地址为\n");
                    if (strstr(content, "AUTH"))
                        printf("请求认证\n");
                    if (strstr(content, "LOGIN"))
                        printf("认证机制为LOGIN\n");
                    for (i = 0; i < hlf->count_new; i++)
                    {
                        printf("%s", char_to_ascii(content[i]));
                    }
                    printf("\n");
                    if (strstr(content, "\n."))
                        printf("数据传输结束\n");
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
    nids_register_tcp(smtp_protocol_callback);
    /* 注册分析TCP协议的回调函数 */
    nids_run();
    /* 进入循环捕获数据包状态 */
}
