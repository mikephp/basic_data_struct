/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndSrv.c
 *
 * PURPOSE :  ʵ�� Message Send Protocol 2 ����������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "MsgSndSrv.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

/* Э�����ֶε�ö��ֵ����Ӧ������ */
static struct field_name msp_info[MSP_TOTAL] = {
    { MSP_VERSION,     "VERSION"    },
    { MSP_RECIP,       "RECIPIENT"  },
    { MSP_RECIP_TERM,  "RECIP-TERM" },
    { MSP_MESSAGE,     "MESSAGE"    },
    { MSP_SENDER,      "SENDER"     },
    { MSP_SEND_TERM,   "SENDER-TERM"},
    { MSP_COOKIE,      "COOKIE"     },
    { MSP_SIGNAT,      "SIGNATURE"  }
};

static SOCKET msp_client_soc[FD_SETSIZE]; /* ��¼���ܵĿͻ����� */
static int client_max;


/**************************************************************************
 *
 * ��������: ���յ��ͻ��˵���������.
 *
 * ����˵��: [IN]  lstn_soc, ���� socket;
 *           [OUT] soc_set, �½��ܵ� socket ��Ҫ����ļ���;
 *
 * �� �� ֵ: �ɹ������½��ܵ� socket, ʧ�ܷ��� INVALID_SOCKET.
 *
 **************************************************************************/
SOCKET msp_new_client(SOCKET lstn_soc, fd_set *soc_set)
{
    int i;
    struct sockaddr_in faddr;
    int addr_len = sizeof(faddr);
    SOCKET acpt_soc;

    acpt_soc = accept(lstn_soc, (struct sockaddr *)&faddr, &addr_len);
    if (acpt_soc == INVALID_SOCKET)
        return INVALID_SOCKET;

    printf("[MSP] accept connection : %s\n", inet_ntoa(faddr.sin_addr));
    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (msp_client_soc[i] == INVALID_SOCKET)
        {
            msp_client_soc[i] = acpt_soc;
            break;
        }
    }

    if (i == FD_SETSIZE) /* �Ѿ����� */
    {
        closesocket(acpt_soc);
        return INVALID_SOCKET;
    }

    if (i > client_max)
        client_max = i;

    FD_SET(acpt_soc, soc_set);

    return acpt_soc;
}

/**************************************************************************
 *
 * ��������: �رտͻ��˵��׽ӿ�������, �׽ӿڴӼ��� soc_set ��ɾ����.
 *
 * ����˵��: [IN]  index, �׽ӿ���ȫ�ֱ���msp_client_soc�ж�Ӧ���±�;
 *           [OUT] soc_set, �׽ӿڴ����������ɾ��;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void msp_close_client(int index, fd_set *soc_set)
{
    SOCKET sock = msp_client_soc[index]; 

    closesocket(sock);
    FD_CLR(sock, soc_set);
    msp_client_soc[index] = msp_client_soc[client_max];
    msp_client_soc[client_max--] = INVALID_SOCKET;
}

/**************************************************************************
 *
 * ��������: ���տͻ��˵�����.
 *
 * ����˵��: [IN]  �ڿͻ� socket �����е�����;
 *           [OUT] buf, �������ݵĻ�����;
 *           [IN]  len, buf �ĳ���;
 *           [OUT] from, �ͻ��˵�ַ;
 *
 * �� �� ֵ: �ɹ����ؽ��յ��ֽ���, ʧ�ܷ��� SOCKET_ERROR.
 *
 **************************************************************************/
int msp_recv_data(int i, char *buf, int len, struct sockaddr_in *from)
{
    int result, size = sizeof(*from);
    SOCKET soc = msp_client_soc[i];

    if (i == 0) /* ���� UDP ����� socket */
        result = recvfrom(soc, buf, len, 0, (struct sockaddr *)from, &size);
    else
        result = recv(soc, buf, len, 0); /* ���� TCP ����� socket */

    return result;
}

/**************************************************************************
 *
 * ��������: ������Ϣ����Э����յ�������.
 *
 * ����˵��: [IN] buf, ���յ������ݻ�����;
 *           [IN] len, buf �ĳ���;
 *
 * �� �� ֵ: �ɹ����� MSG_SUCCESS, ʧ�ܷ���ֵС�� 0.
 *
 **************************************************************************/
int msp_process_request(char *buf, int len)
{
    char *sep, *buf_end = buf + len;
    char *msp_field[MSP_TOTAL] = { NULL };
    int i = MSP_RECIP;

    if (buf[0] != MSG_SEND_VERS) /* ���汾 */
        return MSG_VERSION_ERR;

    /* ���������ֶ�ֵ */
    msp_field[MSP_RECIP] = ++buf;
    sep = memchr(buf, 0, buf_end - buf);

    while (sep && (sep < buf_end))
    {
        msp_field[++i] = sep + 1;

        if (i == MSP_SIGNAT)
            break;

        buf = sep + 1;
        sep = memchr(buf, 0, buf_end - buf);
    }

    if (i != MSP_SIGNAT)
        return MSG_FIELD_ERR;

    /* ����Ļ������յ�����Ϣ */
    printf("________________________________\n");
    for (i = MSP_RECIP; i < MSP_TOTAL; i++)
    {
        printf("%s : %s\n", msp_info[i].name, 
               msp_field[i][0] ? msp_field[i] : "Empty");
    }
    printf("________________________________\n");

    return MSG_SUCCESS;
}

/**************************************************************************
 *
 * ��������: ��ͻ��˷���Ӧ��.
 *
 * ����˵��: [IN]  �ڿͻ� socket �����е�����;
 *           [IN]  error, ������;
 *           [IN]  from, �ͻ��˵�ַ;
 *
 * �� �� ֵ: �ɹ����ط��͵��ֽ���, ʧ�ܷ��� SOCKET_ERROR.
 *
 **************************************************************************/
int msp_send_reply(int i, int error, struct sockaddr_in *from)
{    
    int result, msg_len, from_len = sizeof(*from);
    SOCKET client_soc = msp_client_soc[i]; /* ��ͻ������� socket */
    char *msg, *reply_desc[] = { "+ Message is accepted.", 
                                 "- Version is not match.",
                                 "- Message format is wrong.", 
                                 "- Unknow error." };

    msg = reply_desc[-error];
    msg_len = strlen(msg);
    
    if (i == 0) /* ���� UDP �ķ����� */
        result = sendto(client_soc, msg, msg_len, 0, 
                        (struct sockaddr *)from, from_len);
    else 
        result = send(client_soc, msg, msg_len, 0); /* ���� TCP �ķ����� */

    return result;
}


int main(int argc, char **argv)
{
    WSADATA wsa_data;
    SOCKET srv_soc_t = 0, srv_soc_u; /* ��������socket��� */
    struct sockaddr_in srv_addr, clnt_addr;
    unsigned short port = MSG_SEND_PORT;
    int i, result, ready_cnt, addr_len = sizeof(srv_addr);
    char recv_buf[MSG_BUF_SIZE+1];
    SOCKET new_soc, client_soc;
    fd_set read_all, read_set;

    if (argc == 2)
        port = atoi(argv[1]);

    /* ��ʼ���ͻ����ӵ� socket */
    for (i = 0; i < FD_SETSIZE; i++)
        msp_client_soc[i] = INVALID_SOCKET;

    WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ�� WinSock ��Դ */
    srv_soc_t = socket(AF_INET, SOCK_STREAM, 0);/* ���� TCP socket */

    /* ��Ϣ����Э���������ַ */
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    /* �󶨷�������ַ�Ͷ˿� */
    result = bind(srv_soc_t, (struct sockaddr *)&srv_addr, addr_len);
    if (result == SOCKET_ERROR)
    {
        printf("[MSP] fail to bind: %d\n", WSAGetLastError());
        closesocket(srv_soc_t);
        return -1;
    }

    listen(srv_soc_t, SOMAXCONN);
    srv_soc_u = socket(AF_INET, SOCK_DGRAM, 0); /* ���� UDP socket */
    bind(srv_soc_u, (struct sockaddr *)&srv_addr, addr_len);
        
    FD_ZERO(&read_all);
    FD_SET(srv_soc_t, &read_all);
    FD_SET(srv_soc_u, &read_all);
    msp_client_soc[0] = srv_soc_u; /* UDP socket �̶������ڵ�һ��λ�� */

    printf("[MSP] server is running ... ...\n");
    while (1)
    {
        read_set = read_all;
        ready_cnt = select(0, &read_set, NULL, NULL, NULL);
        if (ready_cnt == SOCKET_ERROR)
        {
            printf("[MSP] select error: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(srv_soc_t, &read_set)) /* ��� TCP ������ socket */
        {
            new_soc = msp_new_client(srv_soc_t, &read_all);
            if (--ready_cnt <= 0)
                continue;
        }

        for (i = 0; (i <= client_max) && (ready_cnt > 0); i++)
        {
            client_soc = msp_client_soc[i];
            if (!FD_ISSET(client_soc, &read_set))
                continue;

            /* �������� */
            result = msp_recv_data(i, recv_buf, MSG_BUF_SIZE, &clnt_addr);
            if (result <= 0)
            {
                msp_close_client(i, &read_all);
                --ready_cnt;
                continue;
            }

            result = msp_process_request(recv_buf, result); /* �������� */
            result = msp_send_reply(i, result, &clnt_addr); /* ����Ӧ�� */
            if (result == SOCKET_ERROR)
                msp_close_client(i, &read_all);

            --ready_cnt;
        }        
    }

    /* �ر���ͻ��˵� socket */
    for (i = 0; i <= client_max; i++)
        closesocket(msp_client_soc[i]);

    closesocket(srv_soc_t);
    WSACleanup();

    return 0;
}
