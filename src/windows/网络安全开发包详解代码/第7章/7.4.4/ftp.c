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
下面是分析FTP协议的回调函数
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
                /* FTP客户端和FTP服务器端建立连接 */
                ftp_connection->client.collect++;
                /* FTP客户端接收数据 */
                ftp_connection->server.collect++;
                /* FTP服务器接收数据 */
                ftp_connection->server.collect_urg++;
                /* FTP服务器接收紧急数据 */
                ftp_connection->client.collect_urg++;
                /* FTP客户端接收紧急数据 */
                if (ftp_connection->addr.dest == 21)
                    printf("%s FTP客户端与FTP服务器建立控制连接\n", address_string);
                if (ftp_connection->addr.source == 20)
                    printf("%s FTP服务器与FTP客户端建立数据连接\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* FTP客户端与FTP服务器端连接正常关闭 */
            printf("--------------------------------\n");
            if (ftp_connection->addr.dest == 21)
                printf("%sFTP客户端与FTP服务器的控制连接正常关闭\n", address_string);
            if (ftp_connection->addr.source == 20)
                printf("%sFTP服务器与FTP客户端的数据连接正常关闭\n", address_string);
            return ;
        case NIDS_RESET:
            /* FTP客户端与FTP服务器端连接被RST关闭 */
            printf("--------------------------------\n");
            if (ftp_connection->addr.source == 20)
                printf("%sFTP服务器与FTP客户端的数据连接被RESET关闭\n", address_string);
            if (ftp_connection->addr.dest == 21)
                printf("%sFTP客户端与FTP服务器的控制连接被REST关闭\n", address_string);
            return ;
        case NIDS_DATA:
            {
                /* FTP协议有新的数据达到 */
                struct half_stream *hlf;
                if (ftp_connection->server.count_new_urg)
                {
                    /* FTP服务器接收到新的紧急数据 */
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
                    /* FTP客户端接收到新的紧急数据 */
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
                    /* FTP客户端接收到新的数据 */
                    hlf = &ftp_connection->client;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " <--- ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* 输出FTP客户端接收到的新的数据 */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (ftp_connection->addr.source == 20)
                    {
                        printf("传输的数据为:\n");
                        for (i = 0; i < hlf->count_new; i++)
                        {
                            printf("%s", char_to_ascii(content[i]));
                        }
                        printf("\n");
                    }
                    else
                    {
                        if (content[0] == '1' || content[0] == '2' || content[0] == '3' || content[0] == '4' || content[0] == '5')
                            printf("FTP服务器响应状态代码为：%c%c%c\n", content[0], content[1], content[2]);
                        if (strncmp(content, "220", 3) == 0)
                            printf("新连接的用户的服务已经准备就绪\n");
                        if (strncmp(content, "110", 3) == 0)
                            printf("启动标记应答\n");
                        if (strncmp(content, "120", 3) == 0)
                            printf("表示 服务在nnn分钟内可用\n");
                        if (strncmp(content, "125", 3) == 0)
                            printf("表示数据连接已打开，准备传送\n");
                        if (strncmp(content, "150", 3) == 0)
                            printf("表示文件状态正确，正在打开数据连接\n");
                        if (strncmp(content, "200", 3) == 0)
                            printf("表示命令正常执行\n");
                        if (strncmp(content, "202", 3) == 0)
                            printf("表示命令未被执行，此站点不支持此命令\n");
                        if (strncmp(content, "211", 3) == 0)
                            printf("表示系统状态或系统帮助响应\n");
                        if (strncmp(content, "212", 3) == 0)
                            printf("表示目录状态信息\n");
                        if (strncmp(content, "213", 3) == 0)
                            printf("表示文件状态信息\n");
                        if (strncmp(content, "214", 3) == 0)
                            printf("表示帮助信息\n");
                        if (strncmp(content, "215", 3) == 0)
                            printf("表示名字系统类型\n");
                        if (strncmp(content, "221", 3) == 0)
                            printf("表示控制连接已经被关闭\n");
                        if (strncmp(content, "225", 3) == 0)
                            printf("表示数据连接已经打开，没有数据传输\n");
                        if (strncmp(content, "226", 3) == 0)
                            printf("表示数据连接已经关闭，请求文件操作成功完成\n");
                        if (strncmp(content, "227", 3) == 0)
                            printf("表示进入被动模\n");
                        if (strncmp(content, "230", 3) == 0)
                            printf("表示用户已经登录\n");
                        if (strncmp(content, "250", 3) == 0)
                            printf("表示请求文件操作已经成功执行\n");
                        if (strncmp(content, "257", 3) == 0)
                            printf("表示创建路径名字\n");
                        if (strncmp(content, "331", 3) == 0)
                            printf("表示用户名正确，需要输入密码\n");
                        if (strncmp(content, "332", 3) == 0)
                            printf("表示 登录时需要帐户信息\n");
                        if (strncmp(content, "350", 3) == 0)
                            printf("表示对请求的文件操作需要更多的指令\n");
                        if (strncmp(content, "421", 3) == 0)
                            printf("表示服务不可用，关闭控制连接\n");
                        if (strncmp(content, "425", 3) == 0)
                            printf("表示打开数据连接操作失败\n");
                        if (strncmp(content, "426", 3) == 0)
                            printf("表示关闭连接，中止传输\n");
                        if (strncmp(content, "450", 3) == 0)
                            printf("表示请求的文件操作未被执行\n");
                        if (strncmp(content, "451", 3) == 0)
                            printf("表示请求操作中止，有本地错误发生\n");
                        if (strncmp(content, "452", 3) == 0)
                            printf("表示未执行请求的操作，系统存储空间不足 ，文件不可用\n");
                        if (strncmp(content, "500", 3) == 0)
                            printf("表示语法错误，命令错误\n");
                        if (strncmp(content, "501", 3) == 0)
                            printf("表示参数的语法错误\n");
                        if (strncmp(content, "502", 3) == 0)
                            printf("表示命令未被执行\n");
                        if (strncmp(content, "503", 3) == 0)
                            printf("表示命令顺序发生错误\n");
                        if (strncmp(content, "504", 3) == 0)
                            printf("表示由于参数而发生错误命令\n");
                        if (strncmp(content, "530", 3) == 0)
                            printf("表示未登录\n");
                        if (strncmp(content, "532", 3) == 0)
                            printf("表示存储文件需要帐户信息\n");
                        if (strncmp(content, "550", 3) == 0)
                            printf("表示未执行请求的操作，文件不可用\n");
                        if (strncmp(content, "551", 3) == 0)
                            printf("表示请求操作中止，页面类型未知\n");
                        if (strncmp(content, "552", 3) == 0)
                            printf("表示请求的文件操作中止，超出存储分配空间\n");
                        if (strncmp(content, "553", 3) == 0)
                            printf("表示未执行请求的操作，文件名不合法\n");
                        for (i = 0; i < hlf->count_new; i++)
                        {
                            printf("%s", char_to_ascii(content[i]));
                        }
                        printf("\n");
                    }
                }
                else
                {
                    /* FTP服务器接收到新的数据 */
                    hlf = &ftp_connection->server;
                    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                    strcat(address_string, " ---> ");
                    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                    sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                    strcat(address_string, "\n");
                    printf("--------------------------------\n");
                    printf("%s", address_string);
                    /* 输出FTP服务器端接收到的新的数据 */
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    if (ftp_connection->addr.source == 20)
                        printf("FTP服务器向FTP客户端发送数据\n");
                    else
                    {
                        if (strstr(content, "USER"))
                            printf("用户名字为（USER）:%s\n", content + strlen("USER"));
                        else if (strstr(content, "PASS"))
                            printf("用户密码为（PASS）:%s\n", content + strlen("PASS"));
                        else if (strstr(content, "PORT"))
                            printf("端口参数为（PORT）:%s\n", content + strlen("PORT"));
                        else if (strstr(content, "LIST"))
                            printf("显示文件列表（LIST）:%s\n", content + strlen("LIST"));
                        else if (strstr(content, "CWD"))
                            printf("改变工作目录为（CWD）:%s\n", content + strlen("CWD"));
                        else if (strstr(content, "TYPE"))
                            printf("类型为（TYPE）:%s\n", content + strlen("TYPE"));
                        else if (strstr(content, "RETR"))
                            printf("获取文件为（RETR）:%s\n", content + strlen("RETR"));
                        else if (strstr(content, "STOR"))
                            printf("保存文件为（STOR）:%s\n", content + strlen("STOR"));
                        else if (strstr(content, "XRMD"))
                            printf("删除目录（XRMD）:%s\n", content + strlen("XRMD"));
                        else if (strstr(content, "QUIT"))
                            printf("退出登陆（QUIT）:%s\n", content + strlen("QUIT"));
                        else
                            printf("FTP客户端使用的命令为 %c%c%c%c\n", content[0], content[1], content[2], content[3]);
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
主函数
=======================================================================================================================
 */
void main()
{
	nids_params.device = "2";
    if (!nids_init())
     /* Libnids初始化 */
    {
        printf("出现错误：%s\n", nids_errbuf);
        exit(1);
    }
    nids_register_tcp(ftp_protocol_callback);
    /* 注册回调函数 */
    nids_run();
    /* 进入循环捕获数据包状态 */
}
