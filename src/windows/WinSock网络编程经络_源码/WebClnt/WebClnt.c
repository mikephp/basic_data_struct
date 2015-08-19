/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  WebClnt.c
 *
 * PURPOSE :  HTTP �ͻ��˳���, ��ȡ��ҳ.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

/* ���峣�� */
#define HTTP_DEF_PORT     80  /* ���ӵ�ȱʡ�˿� */
#define HTTP_BUF_SIZE   1024  /* �������Ĵ�С   */
#define HTTP_HOST_LEN    256  /* ���������� */

char *http_req_hdr_tmpl = "GET %s HTTP/1.1\r\n"
    "Accept: image/gif, image/jpeg, */*\r\nAccept-Language: zh-cn\r\n"
    "Accept-Encoding: gzip, deflate\r\nHost: %s:%d\r\n"
    "User-Agent: Huiyong's Browser <0.1>\r\nConnection: Keep-Alive\r\n\r\n";


/**************************************************************************
 *
 * ��������: ���������в���, �ֱ�õ�������, �˿ںź��ļ���. �����и�ʽ:
 *           [http://www.baidu.com:8080/index.html]
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [OUT] host, ��������;
 *           [OUT] port, �˿�;
 *           [OUT] file_name, �ļ���;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void http_parse_request_url(const char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* ���������Ŀ�ʼλ�� */
    begin = strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
    
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* �õ��ļ��� */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end)
            strcpy(file_name, file + 1);
    }

    if (colon) /* �õ��˿ں� */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);

        host_end = colon - 1;
    }

    /* �õ�������Ϣ */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
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
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */

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

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = addr;

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
        return -1; 
    }

    /* ���� HTTP ���� */
    send_len = sprintf(data_buf, http_req_hdr_tmpl, argv[1], host, port);
    result = send(http_sock, data_buf, send_len, 0);
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        return -1; 
    }

    file_web = fopen(file_name, "w+");
    
    do /* ������Ӧ�����浽�ļ��� */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            fwrite(data_buf, 1, result, file_web);

            /* ����Ļ����� */
            data_buf[result] = 0;
            printf("%s", data_buf);
        }
    } while(result > 0);

    fclose(file_web);
    closesocket(http_sock);
    WSACleanup();

    return 0;
}
