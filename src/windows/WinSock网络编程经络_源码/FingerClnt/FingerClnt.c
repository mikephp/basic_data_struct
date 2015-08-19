/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FingerClnt.c
 *
 * PURPOSE :  Finger 客户端程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */


#define WM_GETHOST_NOTIFY  (WM_USER + 1)  /* 定义域名查询消息 */
#define WM_SOCKET_NOTIFY   (WM_USER + 11) /* 定义 socket 消息 */

#define FINGER_DEF_PORT     79 /* 侦听的端口 */
#define FINGER_NAME_LEN    256 /* 一般名字缓冲区长度 */
#define FINGER_MAX_BUF    1024 /* 最大的接收缓冲区 */

/* 定义控件的风格 */
#define STATIC_STYLE   (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define BUTTON_STYLE   (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)
#define EDIT_STYLE     (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define EDIT_STYLE_EXT (EDIT_STYLE | ES_MULTILINE | ES_READONLY | \
                        WS_HSCROLL | WS_VSCROLL)

/* 控件的标识, 是控件在数组中的下标 */
#define ID_EDIT_USER    1  /* 用户 */
#define ID_EDIT_HOST    3  /* 主机 */
#define ID_BTN_FINGER   4  /* 查询按钮的 ID */
#define ID_EDIT_LOG     5  /* 日志控件的标识 */

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

struct Finger
{
    HWND   hWnd;        /* 窗口句柄 */
    HANDLE hAsyncHost;  /* 域名查询句柄 */
    SOCKET hSoc;        /* socket 句柄  */
    char cHostEnt[MAXGETHOSTSTRUCT]; /* 域名查询缓冲区 */
    char szUser[FINGER_NAME_LEN];    /* 用户名 */
    char szHost[FINGER_NAME_LEN];    /* 主机 */
};

/* 定义 Finger 程序使用的控件 */
static struct Widget FgrWgt[] = 
{
    /* 用户名 */
    { 1, 1, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("用户:") },
    { 7, 1, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("Alice") },
    /* 主机 */
    { 33, 1, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("主机:") },
    { 38, 1, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("127.0.0.1") },

    /* Finger 按钮 */
    { 64, 1, 12, 2, BUTTON_STYLE, TEXT("button"), TEXT("Finger") },
    
    /* 信息 */
    { 1, 4, 64, 20, EDIT_STYLE_EXT, TEXT("edit"), TEXT("") }
};

/* 定义全局变量 */
static HWND hWndWgt[TABLE_SIZE(FgrWgt)];
static struct Finger gFingerCtrl = { 0, 0, INVALID_SOCKET };

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void LogPrintf(TCHAR *szFormat, ...);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    TCHAR szClassName[] = TEXT("FingerClnt");
    MSG         msg;
    WNDCLASS    wndclass;
    WSADATA     wsaData;
        
    WSAStartup(WINSOCK_VERSION, &wsaData); /* 初始化 */
    
    /* 注册窗口类 */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* 这是窗口过程 */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL; /* 菜单 */
    wndclass.lpszClassName = szClassName; /* 窗口类名 */
    
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Requires Windows NT!"), szClassName, 0);
        return 0;
    }
    
    gFingerCtrl.hWnd = CreateWindow(szClassName,/* 与注册类名相同 */
                        TEXT("Finger Client"),/* 窗口标题 */
                        WS_OVERLAPPEDWINDOW,  /* 窗口风格 */
                        CW_USEDEFAULT,        /* 初始 x 坐标 */
                        CW_USEDEFAULT,        /* 初始 y 坐标 */
                        CW_USEDEFAULT,        /* 初始宽度 */
                        CW_USEDEFAULT,        /* 初始高度 */
                        NULL,                 /* 父窗口句柄 */
                        NULL,                 /* 菜单句柄 */
                        hInstance,            /* 程序实例句柄 */
                        NULL);                /* 程序参数 */
    
    ShowWindow(gFingerCtrl.hWnd, iCmdShow); /* 显示窗口 */
    UpdateWindow(gFingerCtrl.hWnd);         /* 更新窗口 */    
    
    /* 消息循环 */
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (gFingerCtrl.hSoc != INVALID_SOCKET)
        closesocket(gFingerCtrl.hSoc);

    WSACleanup();
    return msg.wParam;
}

/**************************************************************************
 *
 * 函数功能: 处理 WM_CREATE 消息.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 消息的 WPARAM 参数.
 *           [IN] lParam, 消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FingerOnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{    
    HINSTANCE hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
    int i, iCount = TABLE_SIZE(FgrWgt);    
    int cxChar, cyChar;
    struct Widget *pWgt;

    cxChar = LOWORD(GetDialogBaseUnits());
    cyChar = HIWORD(GetDialogBaseUnits());

    /* 创建控件 */
    for (i = 0; i < iCount; i++)
    {
        pWgt = &FgrWgt[i];
        hWndWgt[i] = CreateWindow(pWgt->szType, pWgt->szTitle, 
            pWgt->iStyle, pWgt->iLeft * cxChar, pWgt->iTop * cyChar, 
            pWgt->iWidth * cxChar, pWgt->iHeigh * cyChar, 
            hWnd, (HMENU) i, hInstance, NULL);
    }
}

/**************************************************************************
 *
 * 函数功能: 处理 Finger 程序的查询命令.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: 处理了命令返回 TRUE, 否则返回 FALSE.
 *
 **************************************************************************/
static BOOL FingerOnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);

    /* 处理 BN_CLICKED, 得到用户输入的信息 */
    if ((wmId == ID_BTN_FINGER) && (wmEvent == BN_CLICKED))
    {
        if (gFingerCtrl.hAsyncHost)
            return TRUE;

        if (gFingerCtrl.hSoc != INVALID_SOCKET)
        {
            closesocket(gFingerCtrl.hSoc);
            gFingerCtrl.hSoc = INVALID_SOCKET;
        }
        
        GetWindowText(hWndWgt[ID_EDIT_USER], gFingerCtrl.szUser, 
            FINGER_NAME_LEN);
        GetWindowText(hWndWgt[ID_EDIT_HOST], gFingerCtrl.szHost, 
            FINGER_NAME_LEN);
        gFingerCtrl.hAsyncHost = WSAAsyncGetHostByName(hWnd, 
            WM_GETHOST_NOTIFY, gFingerCtrl.szHost, 
            gFingerCtrl.cHostEnt, MAXGETHOSTSTRUCT);
        if (gFingerCtrl.hAsyncHost == 0)
            MessageBox(hWnd, TEXT("Get Host Error"), NULL, 0);
        
        return TRUE;
    }

    return FALSE;
}

/**************************************************************************
 *
 * 函数功能: 创建客户端 socket, 与服务器建立连接.
 *
 * 参数说明: [IN] hWnd,  窗口句柄.
 *
 * 返 回 值: 成功返回创建的 socket, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerQuery(HWND hWnd)
{
    struct sockaddr_in soc_addr; /* socket 地址结构 */
    SOCKET soc; /* Finger 的 socket 句柄 */
    int result;
    unsigned long addr;
    struct hostent *host_ent;

    host_ent = (struct hostent *)gFingerCtrl.cHostEnt;
    addr = *(unsigned long *)host_ent->h_addr; /* 网络字节序 */
    
    soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    
    result = WSAAsyncSelect(soc, hWnd, WM_SOCKET_NOTIFY, 
                FD_CONNECT | FD_READ | FD_CLOSE);

    /* 连接的地址和端口 */
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_port = htons(FINGER_DEF_PORT);
    soc_addr.sin_addr.s_addr = addr;

    /* 与服务器建立连接 */
    result = connect(soc, (struct sockaddr *)&soc_addr, sizeof(soc_addr));
    if ((result == SOCKET_ERROR) && 
        (WSAGetLastError() != WSAEWOULDBLOCK))
    {
        closesocket(soc);
        MessageBox(hWnd, TEXT("Can't connect server"), NULL, 0);
        return INVALID_SOCKET;
    }   

    return soc;
}

/**************************************************************************
 *
 * 函数功能: 连接成功, 向服务器发送查询.
 *
 * 参数说明: [IN] clnt_soc,  与客户端连接的 socket 句柄.
 *
 * 返 回 值: 成功返回发送的长度, 失败返回错误码.
 *
 **************************************************************************/
static int FingerOnConnect(SOCKET clnt_soc)
{
    int result;
    char cSendBuf[FINGER_MAX_BUF];

    result = sprintf(cSendBuf, "%s\r\n", gFingerCtrl.szUser);
    result = send(clnt_soc, cSendBuf, result, 0);

    return result;
}

/**************************************************************************
 *
 * 函数功能: 处理 socket 上的读事件通知.
 *
 * 参数说明: [IN] clnt_soc,  socket 句柄.
 *
 * 返 回 值: 成功返回接收的数据长度, 失败返回错误码.
 *
 **************************************************************************/
static int FingerOnRead(SOCKET clnt_soc)
{
    int result, buflen = FINGER_MAX_BUF - 1;
    char cBuf[FINGER_MAX_BUF];

    /* 接收数据 */
    result = recv(clnt_soc, cBuf, buflen, 0);
    if (result <= 0)
    {
        closesocket(clnt_soc); /* 最好加 gFingerCtrl.hSoc = INVALID_SOCKET; */
        return result;
    }

    cBuf[result] = 0;
    LogPrintf(TEXT("%s\r\n"), cBuf);

    return result;
}

/**************************************************************************
 *
 * 函数功能: socket 事件处理.
 *
 * 参数说明: [IN] wParam, socket 句柄.
 *           [IN] lParam, 低 16 位是网络事件, 高 16 位是错误码.
 *
 * 返 回 值: void.
 * 
 **************************************************************************/
static void FingerOnSocketNotify(WPARAM wParam, LPARAM lParam)
{
    int iResult = 0;
    WORD wEvent, wError;

    wEvent = WSAGETSELECTEVENT(lParam); /* LOWORD */
    wError = WSAGETSELECTERROR(lParam); /* HIWORD */

    switch (wEvent)
    {
    case FD_CONNECT:
        if (wError)
        {
            LogPrintf(TEXT("FD_CONNECT error #%i"), wError);
            return;
        }
        FingerOnConnect(wParam);
        break;

    case FD_READ:
        if (wError)
        {
            LogPrintf(TEXT("FD_READ error #%i"), wError);
            return;
        }
        FingerOnRead(wParam);
        break;

    case FD_CLOSE:
        closesocket(wParam);
        gFingerCtrl.hSoc = INVALID_SOCKET;
        break;
    }
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                         WPARAM wParam, LPARAM lParam)
{
    int cyChar, cxClient, cyClient;
    int iError;
    struct Widget *pWgt;
    
    switch (message)
    {
    case WM_CREATE:
        FingerOnCreate(hWnd, wParam, lParam);
        return 0;

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        cyChar = HIWORD(GetDialogBaseUnits());
        pWgt = &FgrWgt[ID_EDIT_LOG];
        
        MoveWindow(hWndWgt[ID_EDIT_LOG], pWgt->iLeft, 
            pWgt->iTop * cyChar, cxClient, cyClient, FALSE);

        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_COMMAND:
        if (FingerOnCommand(hWnd, wParam, lParam))
            return 0;
        break;

    case WM_GETHOST_NOTIFY:
        iError = WSAGETASYNCERROR(lParam);
        if (iError || wParam != (WPARAM)gFingerCtrl.hAsyncHost)
        {
            gFingerCtrl.hAsyncHost = 0;
            MessageBox(hWnd, TEXT("Get Host Result Error"), NULL, 0);
            return 0; /* 发生错误 */
        }
        gFingerCtrl.hAsyncHost = 0;
        gFingerCtrl.hSoc = FingerQuery(hWnd);
        return 0;
        
    case WM_SOCKET_NOTIFY:
        FingerOnSocketNotify(wParam, lParam);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

/**************************************************************************
 *
 * 函数功能: 在日志窗口中显示详细的交互信息.
 *
 * 参数说明: [IN] szFormat, 打印数据的格式信息.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void LogPrintf(TCHAR *szFormat, ...)
{
    int iBufLen = 0, iIndex;
    TCHAR szBuffer[FINGER_MAX_BUF];
    HWND hWndLog = hWndWgt[ID_EDIT_LOG];
    va_list pVaList;

    va_start(pVaList, szFormat);

#ifdef UNICODE
    iBufLen = _vsnwprintf(szBuffer, FINGER_MAX_BUF, szFormat, pVaList);
#else
    iBufLen = _vsnprintf(szBuffer, FINGER_MAX_BUF, szFormat, pVaList);
#endif

    va_end(pVaList);

    iIndex = GetWindowTextLength(hWndLog);

    SendMessage(hWndLog, EM_SETSEL, (WPARAM)iIndex, (LPARAM)iIndex);
    SendMessage(hWndLog, EM_REPLACESEL, FALSE, (LPARAM)szBuffer);
    SendMessage(hWndLog, EM_SCROLLCARET, 0, 0);
}