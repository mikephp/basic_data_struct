/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndClnt.c
 *
 * PURPOSE :  ʵ�� Message Send Protocol 2 �ͻ��˳���.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

#define MSG_SEND_VERS         'B' /* �汾     */
#define MSG_TERMINAL    "Console" /* �ն����� */
#define MSG_SEND_PORT         18  /* ��Ϣ����Э��ȱʡ�˿�*/
#define MSG_BUF_SIZE         512  /* ����������󳤶�    */
#define MSG_EACH_SIZE        160  /* ÿһ������󳤶�    */
#define MSG_COOKIE_LEN        15  /* Cookie ����С����   */

enum msg_status /* �����״̬ */
{
    MSG_RECIP,
    MSG_CONTENT,
    MSG_SENDER,
    MSG_TOTAL
};

/**************************************************************************
 *
 * ��������: ���ݵ�ǰ�����ں�ʱ��, ���� COOKIE.
 *
 * ����˵��: [OUT] buf, �������ݵĻ�����;
 *           [IN] len, buf �ĳ���;
 *
 * �� �� ֵ: �ɹ����ػ����������ݵĳ���, ʧ�ܷ��� 0.
 *
 **************************************************************************/
int msp_make_cookie(char *buf, int len)
{
    int result;
    time_t t;
    struct tm *time_p;

    if (len < MSG_COOKIE_LEN)
        return 0;
    
    t = time(NULL);
    time_p = localtime(&t);
    result = sprintf(buf, "%04d%02d%02d%02d%02d%02d", time_p->tm_year+1900,
                     time_p->tm_mon + 1, time_p->tm_mday, time_p->tm_hour, 
                     time_p->tm_min, time_p->tm_sec);

    return result;
}

/**************************************************************************
 *
 * ��������: �����û������벢�������������Ϣ.
 *
 * ����˵��: [IN] msg_soc, socket ������;
 *           [IN] prompt, �Ƿ���ʾ�˵�ѡ����Ϣ;
 *
 * �� �� ֵ: �ɹ����ط��͵����ݳ���, ʧ�ܷ���ֵС�ڵ��� 0.
 *
 **************************************************************************/
int msp_input_send(SOCKET msg_soc, int prompt)
{
    int offset, result, menu, status = MSG_RECIP;
    int term_len = strlen(MSG_TERMINAL) + 1;
    char send_buf[MSG_BUF_SIZE], *msg = send_buf;
    
    /* ���û�ѡ���Ƿ����? */
    if (prompt)
    {
        printf("\n------- MENU -------\n");
        printf("*  1 Send Message  *\n");
        printf("*  2 Bye           *\n");
        printf("--------------------\n");
        scanf("%d", &menu);

        if (menu != 1)
            return 0;

        fflush(stdin);
    }

    *msg++ = MSG_SEND_VERS; /* �汾 */
    
    printf("\nTo : ");
    while (fgets(msg, MSG_EACH_SIZE, stdin) != NULL)
    {
        offset = strlen(msg);
        *(msg + offset - 1) = 0; /* ȥ����β�� '\r' */

        switch (status)
        {
        case MSG_RECIP:            
            strcpy(msg + offset, MSG_TERMINAL);
            offset += term_len; /* �ն����ĳ���, ���������� */
            printf("Message : ");
            break;

        case MSG_CONTENT:
            printf("From : ");
            break;

        case MSG_SENDER:
            strcpy(msg + offset, MSG_TERMINAL);
            offset += term_len; /* �ն����ĳ���, ���������� */

            /* ���� COOKIE */
            msg += offset;
            result = msp_make_cookie(msg, send_buf + MSG_BUF_SIZE - msg);
            msg += (result + 1);
            *msg = 0; /* SIGNATURE Ϊ�� */
            offset = 1;
            break;

        default:
            break;
        }

        msg += offset;
        if (++status == MSG_TOTAL)
            break;
    }

    /* �������� */
    result = msg - send_buf;
    result = send(msg_soc, send_buf, result, 0);
    if (result == SOCKET_ERROR)
        printf("[MSP] Fail to send %d\n", WSAGetLastError());

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET msg_soc = 0; /* �ͻ��� socket ��� */
    struct sockaddr_in srv_addr;   /* �ͻ��˵�ַ */
    unsigned short port = MSG_SEND_PORT;
    int result = 0, ready, addr_len = sizeof(srv_addr);
    u_long non_block = 1;
    char data_buf[MSG_BUF_SIZE], *ip_addr = "127.0.0.1";
    fd_set read_all, read_set, write_all, *write_fds = NULL;
    struct timeval timeval = { 60, 0 };

    /* ���������в��� */
    switch (argc)
    {
    case 3:
        port = atoi(argv[2]);
    case 2:
        ip_addr = argv[1];
        break;
    }

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ�� WinSock ��Դ */
    msg_soc = socket(AF_INET, SOCK_STREAM, 0);
    result = ioctlsocket(msg_soc, FIONBIO, &non_block);

    /* ���� socket ���� */
    FD_ZERO(&read_all);
    FD_ZERO(&write_all);
    FD_SET(msg_soc, &read_all);

    /* ��дҪ���ӵķ����� */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    result = connect(msg_soc, (struct sockaddr *)&srv_addr, addr_len);
    if (result == 0) /* ���ӳɹ� */
        write_fds = NULL;
    else if (WSAGetLastError() == WSAEWOULDBLOCK)
    {
        write_fds = &write_all;
        FD_SET(msg_soc, &write_all);
    }
    else
    {
        closesocket(msg_soc);
        return -1;
    }

    while (1)
    {
        read_set = read_all;
        ready = select(0, &read_set, write_fds, NULL, &timeval);
        if (ready <= 0) /* �����ʱ */
        {
            printf("[MSP] Select error.\n");
            break;
        }

        /* ���ӳɹ��� socket �ǿ�д�� */
        if (write_fds && FD_ISSET(msg_soc, write_fds))
        {
            msp_input_send(msg_soc, 0);
            write_fds = NULL;
        }

        if (FD_ISSET(msg_soc, &read_set)) /* socket �����ݿ��Զ� */
        {
            result = recv(msg_soc, data_buf, MSG_BUF_SIZE, 0);
            if (result > 0)
            {
                data_buf[result] = 0;
                printf("[MSP] Receive : %s\n", data_buf);
            }
            else
                break;

            result = msp_input_send(msg_soc, 1); /* �������벢���� */
            if (result <= 0)
                break;
        }
    }

    closesocket(msg_soc);
    WSACleanup();
    
    return 0;
}