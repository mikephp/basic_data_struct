/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpClnt.h
 *
 * PURPOSE :  FTP �ͻ��˳���ͷ�ļ�, ���峣��, ���ݽṹ, ��������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef FTP_CLNT_H_
#define FTP_CLNT_H_

/* ����������ѯ�� socket ��Ϣ */
#define WM_GETHOST_NOTIFY  (WM_USER + 1)
#define WM_SOCKET_NOTIFY   (WM_USER + 11)

#define FTP_EVENT_CNT     4 /* �¼��ĸ��� */
#define FTP_NAME_LEN    128 /* ���ֳ��� */
#define FTP_BUF_LEN    1024 /* ���ջ��������� */
#define MAX_LOAD_STR    128 /* �ַ�����󳤶� */

/* ���к��ֲ��� */
#define FTP_OP_RETR      1
#define FTP_OP_STOR      2

/* �ؼ��ı�ʶ */
#define ID_EDIT_HOST     1 /* ���� */
#define ID_EDIT_PORT     3 /* �˿� */
#define ID_EDIT_USER     5 /* �û��� */
#define ID_EDIT_PASS     7 /* ���� */

#define ID_BTN_CONN      8 /* ���Ӱ�ť�� ID */
#define ID_BTN_DISC      9 /* ������ť�� ID */
#define ID_DIR_LOCAL    10 /* ����Ŀ¼��ǩ ID */
#define ID_SRV_PATH     13 /* Զ��Ŀ¼ ID */
#define ID_FILE_LOCAL   14 /* �����ļ��б����� */
#define ID_FILE_REMOTE  15 /* Զ���ļ��б����� */
#define ID_EDIT_TRACE   16 /* ��Ϣ���� */

/* ����ؼ��ķ�� */
#define STATIC_STYLE  (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define BUTTON_STYLE  (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)
#define LIST_STYLE    (WS_CHILD | WS_VISIBLE | LBS_STANDARD)
#define EDIT_STYLE    (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define EDIT_STYLE_EX (EDIT_STYLE | ES_MULTILINE | WS_HSCROLL | \
                       WS_VSCROLL)

/* �б������ */
#define DIRATTR (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | \
                 DDL_SYSTEM | DDL_DIRECTORY | DDL_ARCHIVE  | \
                 DDL_DRIVES)

#define TABLE_SIZE(a) (sizeof a / sizeof a[0])

/* �ؼ������Խṹ */
struct Widget
{
    int iLeft;      /* ���Ͻǵ� x ���� */
    int iTop;       /* ���Ͻǵ� y ���� */
    int iWidth;     /* ��� */
    int iHeigh;     /* �߶� */
    int iStyle;     /* �ؼ��ķ�� */
    TCHAR *szType;  /* �ؼ�����: button, edit etc. */
    TCHAR *szTitle; /* �ؼ�����ʾ������ */
};

/* FTP �����Ϣ�Ľṹ */
struct ftp_ctrl_s
{
    int iInit;  /* �Ƿ��Ѿ���ʼ�� */
    int iStatus;               /* FTP ��״̬ */
    char cHost[FTP_NAME_LEN];  /* ������ */
    char cPath[FTP_NAME_LEN];  /* ��ǰ·�� */
    char cUser[FTP_NAME_LEN];  /* �û��� */
    char cPass[FTP_NAME_LEN];  /* ���� */
    unsigned short sPort;      /* Ŀ�Ķ˿�, �����ֽ��� */
    short sEvtCnt;             /* �¼��ĸ��� */
    enum ftp_event Event[FTP_EVENT_CNT]; /* Ҫ������¼����� */
    enum ftp_event DataOp; /* ���ݲ��� */

    /* ��Ӧ�ó�����صı��� */
    HINSTANCE hInst;/* Ӧ�ó���ʵ����� */
    HWND hWnd;      /* Ӧ�ó��򴰿ھ�� */
    TCHAR szTitle[MAX_LOAD_STR]; /* Ӧ�ó������ */
    TCHAR szClass[MAX_LOAD_STR]; /* �������� */
    WNDPROC ListProc;   /* ListBox �ɵĴ��ڹ��� */

    /* Socket ��ر��� */
    SOCKET hCtrlSoc; /* �����׽ӿ� */
    SOCKET hDataSoc; /* �����׽ӿ� */
    SOCKET hLstnSoc; /* �����׽ӿ� */
    char cCmdBuf[FTP_BUF_LEN];  /* ������� */
    char cRecvBuf[FTP_BUF_LEN]; /* ���ջ����� */
    int iRecvLen;
    char HostEnt[MAXGETHOSTSTRUCT]; /* ������ѯ������ */
    HANDLE hAsyncHost;  /* ������ѯ���  */
    HANDLE hFile;       /* �����ļ���� */
    
    /* ����϶��йصı��� */
    int iSourID;    /* Դ���� ID */
    int iDestID;    /* Ŀ�Ĵ��� ID */
    int iLBtnDown;  /* ���������� */
    int iMouseMove; /* ����ƶ� */
    int iLBtnUp;    /* ������̧�� */
    TCHAR cSelFile[MAX_PATH];  /* ���ѡ����ļ� */
};

/* �������� */
void FtpRemoteListAdd(char *szFile);
void FtpListAddFile(int iLocal);
void FtpPrintf(TCHAR *szFormat, ...);
void FtpEnableConnWindow(int bEnable);

struct ftp_ctrl_s ftp_ctrl;

#endif /* FTP_CLNT_H_ */
