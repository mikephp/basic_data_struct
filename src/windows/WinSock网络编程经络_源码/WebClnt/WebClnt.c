/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  WebClnt.c
 *
 * PURPOSE :  HTTP 客户端程序, 获取网页.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

/* 定义常量 */
#define HTTP_DEF_PORT     80  /* 连接的缺省端口 */
#define HTTP_BUF_SIZE   1024  /* 缓冲区的大小   */
#define HTTP_HOST_LEN    256  /* 主机名长度 */

char *http_req_hdr_tmpl = "GET %s HTTP/1.1\r\n"
    "Accept: image/gif, image/jpeg, */*\r\nAccept-Language: zh-cn\r\n"
    "Accept-Encoding: gzip, deflate\r\nHost: %s:%d\r\n"
    "User-Agent: Huiyong's Browser <0.1>\r\nConnection: Keep-Alive\r\n\r\n";


/**************************************************************************
 *
 * 函数功能: 解析命令行参数, 分别得到主机名, 端口号和文件名. 命令行格式:
 *           [http://www.baidu.com:8080/index.html]
 *
 * 参数说明: [IN]  buf, 字符串指针数组;
 *           [OUT] host, 保存主机;
 *           [OUT] port, 端口;
 *           [OUT] file_name, 文件名;
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void http_parse_request_url(const char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* 查找主机的开始位置 */
    begin = strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
    
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* 得到文件名 */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end)
            strcpy(file_name, file + 1);
    }

    if (colon) /* 得到端口号 */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);

        host_end = colon - 1;
    }

    /* 得到主机信息 */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
    struct hostent *host_ent;
    
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE];
    char host[HTTP_HOST_LEN] = "127.0.0.1";
    unsigned short port = HTTP_DEF_PORT;
    unsigned long addr;
    char file_name[HTTP_HOST_LEN] = "index.html";
    FILE *file_web;

    if (argc != 2)
    {
        printf("[Web] input : %s http://www.test.com[:8080]/index.html", argv[0]);
        return -1;
    }

    http_parse_request_url(argv[1], host, &port, file_name);
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */

    addr = inet_addr(host);
    if (addr == INADDR_NONE)
    {
        host_ent = gethostbyname(host);
        if (!host_ent)
        {
            printf("[Web] invalid host\n");
            return -1;
        }
        
        memcpy(&addr, host_ent->h_addr_list[0], host_ent->h_length);
    }

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = addr;

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
        return -1; 
    }

    /* 发送 HTTP 请求 */
    send_len = sprintf(data_buf, http_req_hdr_tmpl, argv[1], host, port);
    result = send(http_sock, data_buf, send_len, 0);
    if (result == SOCKET_ERROR) /* 发送失败 */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        return -1; 
    }

    file_web = fopen(file_name, "w+");
    
    do /* 接收响应并保存到文件中 */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            fwrite(data_buf, 1, result, file_web);

            /* 在屏幕上输出 */
            data_buf[result] = 0;
            printf("%s", data_buf);
        }
    } while(result > 0);

    fclose(file_web);
    closesocket(http_sock);
    WSACleanup();

    return 0;
}
