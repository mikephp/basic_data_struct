/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpProt.h
 *
 * PURPOSE :  FTP Э��ͷ�ļ�, ����Э���еĳ���, ��������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef FTP_PROT_H_
#define FTP_PROT_H_

#define FTP_DATA_PORT   20 /* ���ݶ˿� */
#define FTP_CTRL_PORT   21 /* ���ƶ˿� */

#define FTP_MULTILINE_FLAG  '-'  /* ���б�ʶ�� */
#define FTP_LINE_END   "\r\n"    /* �н�����־ */

/* Ӧ���� */
#define FTP_CODE_100  1 /* �϶���ʼӦ�� */
#define FTP_CODE_200  2 /* �϶����Ӧ�� */
#define FTP_CODE_300  3 /* �϶��м�Ӧ�� */
#define FTP_CODE_400  4 /* ��ʱ��Ӧ�� */
#define FTP_CODE_500  5 /* ���÷�Ӧ�� */

/* FTP ������ */
#define FTPE_SUCESS    0  /* �ɹ� */
#define FTPE_ERROR    -1  /* һ����� */
#define FTPE_NOMEM    -2  /* û���ڴ� */
#define FTPE_INVALID  -3  /* ��Ч���� */
#define FTPE_STATUS   -4  /* ״̬���� */

/* ���� FTP ��״̬ */
#define FTPS_CLOSED    0
#define FTPS_GETHOST   1
#define FTPS_CONNECT   2
#define FTPS_USER      3
#define FTPS_PASSWD    4
#define FTPS_LOGIN     5
#define FTPS_SENDCMD   6

/* �����¼� */
#define FTP_EVENT_ENTER(ev, prev) do {  \
        if (prev) {                     \
            ftp_ctrl.Event[0] = ev;     \
            ftp_ctrl.sEvtCnt = 1;       \
        } else                          \
            ftp_ctrl.Event[ftp_ctrl.sEvtCnt++] = ev; \
    } while (0)

/* ���� FTP ���¼� */
enum ftp_event
{
    EV_HOST,
    EV_CONN,
    EV_USER,
    EV_PASS,
    EV_PORT,
    EV_LIST,
    EV_RETR,
    EV_STOR,
    EV_CWD,
    EV_LDIR,
    EV_QUIT
};

/* FTP Э������ṩ�ĺ������� */
int ftp_do_event(enum ftp_event ev, void *arg1, void *arg2);
int ftp_recv_process(SOCKET soc);
void ftp_close_process(SOCKET soc);
SOCKET ftp_accept_conn(SOCKET soc);
int ftp_file_process(int operation);
int ftp_store_file(SOCKET soc);
int ftp_startup(void);
void ftp_cleanup(int end);

/* ftp log */
FILE *ftp_log_open(char *log_file);
void ftp_log_close(void);
int ftp_log_write(const char *buf, int buf_len);
char *ftp_log_print(const char *fmt, ...);

#endif /* FTP_PROT_H_ */
