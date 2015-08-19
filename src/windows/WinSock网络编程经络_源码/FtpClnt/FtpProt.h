/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpProt.h
 *
 * PURPOSE :  FTP 协议头文件, 定义协议中的常量, 函数声明.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef FTP_PROT_H_
#define FTP_PROT_H_

#define FTP_DATA_PORT   20 /* 数据端口 */
#define FTP_CTRL_PORT   21 /* 控制端口 */

#define FTP_MULTILINE_FLAG  '-'  /* 多行标识符 */
#define FTP_LINE_END   "\r\n"    /* 行结束标志 */

/* 应答码 */
#define FTP_CODE_100  1 /* 肯定初始应答 */
#define FTP_CODE_200  2 /* 肯定完成应答 */
#define FTP_CODE_300  3 /* 肯定中间应答 */
#define FTP_CODE_400  4 /* 暂时否定应答 */
#define FTP_CODE_500  5 /* 永久否定应答 */

/* FTP 错误码 */
#define FTPE_SUCESS    0  /* 成功 */
#define FTPE_ERROR    -1  /* 一般错误 */
#define FTPE_NOMEM    -2  /* 没有内存 */
#define FTPE_INVALID  -3  /* 无效参数 */
#define FTPE_STATUS   -4  /* 状态错误 */

/* 定义 FTP 的状态 */
#define FTPS_CLOSED    0
#define FTPS_GETHOST   1
#define FTPS_CONNECT   2
#define FTPS_USER      3
#define FTPS_PASSWD    4
#define FTPS_LOGIN     5
#define FTPS_SENDCMD   6

/* 加入事件 */
#define FTP_EVENT_ENTER(ev, prev) do {  \
        if (prev) {                     \
            ftp_ctrl.Event[0] = ev;     \
            ftp_ctrl.sEvtCnt = 1;       \
        } else                          \
            ftp_ctrl.Event[ftp_ctrl.sEvtCnt++] = ev; \
    } while (0)

/* 定义 FTP 的事件 */
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

/* FTP 协议对外提供的函数声明 */
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
