/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpClnt.h
 *
 * PURPOSE :  FTP 客户端程序头文件, 定义常量, 数据结构, 函数声明.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef FTP_CLNT_H_
#define FTP_CLNT_H_

/* 定义域名查询和 socket 消息 */
#define WM_GETHOST_NOTIFY  (WM_USER + 1)
#define WM_SOCKET_NOTIFY   (WM_USER + 11)

#define FTP_EVENT_CNT     4 /* 事件的个数 */
#define FTP_NAME_LEN    128 /* 名字长度 */
#define FTP_BUF_LEN    1024 /* 接收缓冲区长度 */
#define MAX_LOAD_STR    128 /* 字符串最大长度 */

/* 进行何种操作 */
#define FTP_OP_RETR      1
#define FTP_OP_STOR      2

/* 控件的标识 */
#define ID_EDIT_HOST     1 /* 主机 */
#define ID_EDIT_PORT     3 /* 端口 */
#define ID_EDIT_USER     5 /* 用户名 */
#define ID_EDIT_PASS     7 /* 密码 */

#define ID_BTN_CONN      8 /* 连接按钮的 ID */
#define ID_BTN_DISC      9 /* 断连按钮的 ID */
#define ID_DIR_LOCAL    10 /* 本地目录标签 ID */
#define ID_SRV_PATH     13 /* 远端目录 ID */
#define ID_FILE_LOCAL   14 /* 本地文件列表索引 */
#define ID_FILE_REMOTE  15 /* 远端文件列表索引 */
#define ID_EDIT_TRACE   16 /* 信息索引 */

/* 定义控件的风格 */
#define STATIC_STYLE  (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define BUTTON_STYLE  (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)
#define LIST_STYLE    (WS_CHILD | WS_VISIBLE | LBS_STANDARD)
#define EDIT_STYLE    (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define EDIT_STYLE_EX (EDIT_STYLE | ES_MULTILINE | WS_HSCROLL | \
                       WS_VSCROLL)

/* 列表框属性 */
#define DIRATTR (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | \
                 DDL_SYSTEM | DDL_DIRECTORY | DDL_ARCHIVE  | \
                 DDL_DRIVES)

#define TABLE_SIZE(a) (sizeof a / sizeof a[0])

/* 控件的属性结构 */
struct Widget
{
    int iLeft;      /* 左上角的 x 坐标 */
    int iTop;       /* 左上角的 y 坐标 */
    int iWidth;     /* 宽度 */
    int iHeigh;     /* 高度 */
    int iStyle;     /* 控件的风格 */
    TCHAR *szType;  /* 控件类型: button, edit etc. */
    TCHAR *szTitle; /* 控件上显示的文字 */
};

/* FTP 相关信息的结构 */
struct ftp_ctrl_s
{
    int iInit;  /* 是否已经初始化 */
    int iStatus;               /* FTP 的状态 */
    char cHost[FTP_NAME_LEN];  /* 主机名 */
    char cPath[FTP_NAME_LEN];  /* 当前路径 */
    char cUser[FTP_NAME_LEN];  /* 用户名 */
    char cPass[FTP_NAME_LEN];  /* 密码 */
    unsigned short sPort;      /* 目的端口, 网络字节序 */
    short sEvtCnt;             /* 事件的个数 */
    enum ftp_event Event[FTP_EVENT_CNT]; /* 要处理的事件队列 */
    enum ftp_event DataOp; /* 数据操作 */

    /* 与应用程序相关的变量 */
    HINSTANCE hInst;/* 应用程序实例句柄 */
    HWND hWnd;      /* 应用程序窗口句柄 */
    TCHAR szTitle[MAX_LOAD_STR]; /* 应用程序标题 */
    TCHAR szClass[MAX_LOAD_STR]; /* 窗口类名 */
    WNDPROC ListProc;   /* ListBox 旧的窗口过程 */

    /* Socket 相关变量 */
    SOCKET hCtrlSoc; /* 控制套接口 */
    SOCKET hDataSoc; /* 数据套接口 */
    SOCKET hLstnSoc; /* 侦听套接口 */
    char cCmdBuf[FTP_BUF_LEN];  /* 命令缓冲区 */
    char cRecvBuf[FTP_BUF_LEN]; /* 接收缓冲区 */
    int iRecvLen;
    char HostEnt[MAXGETHOSTSTRUCT]; /* 域名查询缓冲区 */
    HANDLE hAsyncHost;  /* 域名查询句柄  */
    HANDLE hFile;       /* 数据文件句柄 */
    
    /* 鼠标拖动有关的变量 */
    int iSourID;    /* 源窗口 ID */
    int iDestID;    /* 目的窗口 ID */
    int iLBtnDown;  /* 左鼠标键按下 */
    int iMouseMove; /* 鼠标移动 */
    int iLBtnUp;    /* 左鼠标键抬起 */
    TCHAR cSelFile[MAX_PATH];  /* 鼠标选择的文件 */
};

/* 函数声明 */
void FtpRemoteListAdd(char *szFile);
void FtpListAddFile(int iLocal);
void FtpPrintf(TCHAR *szFormat, ...);
void FtpEnableConnWindow(int bEnable);

struct ftp_ctrl_s ftp_ctrl;

#endif /* FTP_CLNT_H_ */
