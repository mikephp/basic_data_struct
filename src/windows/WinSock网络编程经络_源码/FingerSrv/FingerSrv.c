/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FingerSrv.c
 *
 * PURPOSE :  Finger 服务器程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "resource.h"

#pragma comment(lib, "ws2_32.lib") /* WinSock使用的库函数 */

#define WM_SOCKET_NOTIFY   (WM_USER + 11) /* 定义 socket 消息 */
#define FINGER_DEF_PORT    79 /* 侦听的端口 */
#define ID_EDIT_LOG        1  /* 日志控件的标识 */

/* 定义控件的风格 */
#define EDIT_STYLE  (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |\
                     ES_MULTILINE | WS_HSCROLL | WS_VSCROLL)

#define FINGER_LINE_END  "\r\n" /* 行结束符 */
#define FINGER_MAX_BUF    1024  /* 最大的接收缓冲区 */
#define FINGER_MAX_SOC       8  /* 最多可以接受的客户数 */
#define TABLE_SIZE(a) (sizeof a / sizeof a[0])

/* 定义用户信息 */
struct UserInfo
{
    char *szUser;
    char *szFullName;
    char *szMessage;
};

struct UserInfo FingerUser[] = 
{
    { "Alice",  "Alice Joe",   "Welcome! I am on vacation." },
    { "Smith",  "Smith David", "Learn to fly like a bird." },
    { "Rubin",  "Jeff Rubin",  "How are you!" }
};

/* 定义全局变量 */
static HWND hWndLog;       /* 输出日志信息的窗口句柄 */
static SOCKET hLstnSoc;    /* 侦听 socket 句柄 */
static SOCKET hClntSoc[FINGER_MAX_SOC]; /* 客户端的 socket 句柄 */

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SOCKET FingerListenSoc(HWND hWnd, unsigned short port);
static void FingerOnSocketNotify(WPARAM wParam, LPARAM lParam);
static void LogPrintf(TCHAR *szFormat, ...);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    TCHAR szClassName[] = TEXT("FingerSrv");
    HWND        hWnd;
    MSG         msg;
    WNDCLASS    wndclass;
    WSADATA     wsaData;
    int i;

    WSAStartup(WINSOCK_VERSION, &wsaData); /* 初始化 */
    memset(hClntSoc,INVALID_SOCKET,FINGER_MAX_SOC*sizeof(SOCKET));

    /* 注册窗口类 */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* 这是窗口过程 */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = (LPCSTR)IDR_FINGER_SRV; /* 菜单 */
    wndclass.lpszClassName = szClassName; /* 窗口类名 */

    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("TRequires Windows NT!"), szClassName, 0);
        return 0;
    }

    hWnd = CreateWindow(szClassName,          /* 与注册的类名相同 */
                        TEXT("Finger Server"),/* 窗口标题 */
                        WS_OVERLAPPEDWINDOW,  /* 窗口风格 */
                        CW_USEDEFAULT,        /* 初始 x 坐标 */
                        CW_USEDEFAULT,        /* 初始 y 坐标 */
                        CW_USEDEFAULT,        /* 初始宽度 */
                        CW_USEDEFAULT,        /* 初始高度 */
                        NULL,                 /* 父窗口句柄 */
                        NULL,                 /* 菜单句柄 */
                        hInstance,            /* 程序实例句柄 */
                        NULL);                /* 程序参数 */

    ShowWindow(hWnd, iCmdShow); /* 显示窗口 */
    UpdateWindow(hWnd);         /* 更新窗口 */

    /* 消息循环 */
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* 关闭 socket */
    for (i = 0; i < FINGER_MAX_SOC; i++)
    {
        if (hClntSoc[i] != INVALID_SOCKET)
            closesocket(hClntSoc[i]);
    }
    closesocket(hLstnSoc);
    WSACleanup();

    return msg.wParam;
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                         WPARAM wParam, LPARAM lParam)
{
    int cxClient, cyClient;
    int wmId, wmEvent;

    switch (message)
    {
    case WM_CREATE:
        hWndLog = CreateWindow(TEXT("edit"), NULL, EDIT_STYLE, 
                    0, 0, 0, 0, hWnd, (HMENU)ID_EDIT_LOG,
                    ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        return 0;

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        MoveWindow(hWndLog, 0, 0, cxClient, cyClient, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
        wmId    = LOWORD(wParam); 
        wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case IDM_START:
            hLstnSoc = FingerListenSoc(hWnd, FINGER_DEF_PORT);
            if (hLstnSoc == INVALID_SOCKET)
                MessageBox(hWnd, TEXT("Listen error"), TEXT("Finger"), 0);
            return 0;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_SOCKET_NOTIFY:
        FingerOnSocketNotify(wParam, lParam);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

/**************************************************************************
 *
 * 函数功能: 创建侦听 socket.
 *
 * 参数说明: [IN] hWnd,  窗口句柄.
 *           [IN] port, 侦听的端口.
 *
 * 返 回 值: 成功返回创建的 socket, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerListenSoc(HWND hWnd, unsigned short port)
{
    struct sockaddr_in soc_addr; /* socket 地址结构 */
    SOCKET lstn_soc; /* 侦听 socket 句柄 */
    int result;

    /* 创建侦听 socket */
    lstn_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = WSAAsyncSelect(lstn_soc, hWnd, WM_SOCKET_NOTIFY, 
                            FD_ACCEPT | FD_READ | FD_CLOSE);

    /* 由系统来分配地址 */
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_port = htons(port);
    soc_addr.sin_addr.s_addr = INADDR_ANY;

    /* 绑定 socket */
    result = bind(lstn_soc, (struct sockaddr *)&soc_addr, sizeof(soc_addr));
    if (result == SOCKET_ERROR)
    {
        closesocket(lstn_soc);
        return INVALID_SOCKET;
    }

    result = listen(lstn_soc, SOMAXCONN); /* 侦听来自客户端的连接 */
    if (result == SOCKET_ERROR)
    {
        closesocket(lstn_soc);
        return INVALID_SOCKET;
    }

    LogPrintf(TEXT("Finger server is running ...\r\n"));
    return lstn_soc;
}

/**************************************************************************
 *
 * 函数功能: 接受客户的连接.
 *
 * 参数说明: [IN] lstn_soc,  侦听 socket 句柄.
 *
 * 返 回 值: 成功返回接受的 socket 句柄, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerOnAccept(SOCKET lstn_soc)
{
    struct sockaddr_in soc_addr; /* socket 地址结构 */
    int i, addr_len = sizeof(soc_addr);	/* 地址长度 */
    SOCKET data_soc;

    /* 接受新的连接 */
    data_soc = accept(lstn_soc, (struct sockaddr *)&soc_addr, &addr_len);
    if (data_soc == INVALID_SOCKET)
    {
        LogPrintf(TEXT("accept error: %i.\r\n"), WSAGetLastError());
        return INVALID_SOCKET;
    }

    for (i = 0; i < FINGER_MAX_SOC; i++)
    {
        if (hClntSoc[i] == INVALID_SOCKET)
        {
            hClntSoc[i] = data_soc;
            break;
        }
    }

    return data_soc;
}

/**************************************************************************
 *
 * 函数功能: 处理客户的请求.
 *
 * 参数说明: [IN] clnt_soc,  与客户端连接的 socket 句柄.
 *
 * 返 回 值: 成功返回 TRUE, 失败返回 FALSE.
 *
 **************************************************************************/
static int FingerOnRead(SOCKET clnt_soc)
{
    int i, j, result, buflen = FINGER_MAX_BUF - 1;
    int iFind = 0, iCount = TABLE_SIZE(FingerUser);
    char cBuf[FINGER_MAX_BUF], cSendBuf[FINGER_MAX_BUF], *pEnd;
    struct UserInfo *pUser;

    /* 查找客户端对应的 socket 句柄 */
    for (i = 0; i < FINGER_MAX_SOC; i++)
    {
        if (hClntSoc[i] == clnt_soc)
            break;
    }

    if (i == FINGER_MAX_SOC)
        return FALSE;
    
    result = recv(clnt_soc, cBuf, buflen, 0);  /* 接收数据 */
    if (result <= 0)
    {
        closesocket(clnt_soc);
        hClntSoc[i] = INVALID_SOCKET;
        return FALSE;
    }

    cBuf[result] = 0;
    LogPrintf(TEXT("recv >: %s\r\n"), cBuf);

    /* 搜索用户名结尾的 "\r\n" */
    pEnd = strstr(cBuf, FINGER_LINE_END);
    if ((pEnd != NULL) && (pEnd != cBuf))
        *pEnd = 0; /*结尾的 "\r\n" 换成 0, 便于查找 */

    for (j = 0; j < iCount; j++) /* 查找用户信息 */
    {
        pUser = &FingerUser[j];
        
        if (strcmp(cBuf, FINGER_LINE_END) == 0) /* 所有用户 */
            buflen = sprintf(cSendBuf, "%s\r\n", pUser->szUser);
        else if (strcmp(cBuf, pUser->szUser) == 0) /* 特定用户 */
            buflen = sprintf(cSendBuf, "%s: %s, %s\r\n", pUser->szUser,
                pUser->szFullName, pUser->szMessage);
        else
            continue;

        iFind++;
        result = send(clnt_soc, cSendBuf, buflen, 0);
        if (result > 0)
            LogPrintf(TEXT("send <: %s\r\n"), cSendBuf);
    }

    if (!iFind)
        send(clnt_soc, "The user is not found.\r\n", 24, 0);

    closesocket(clnt_soc);
    hClntSoc[i] = INVALID_SOCKET;

    return TRUE;
}

/**************************************************************************
 *
 * 函数功能: 处理关闭事件.
 *
 * 参数说明: [IN] clnt_soc,  与客户端连接的 socket 句柄.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FingerOnClose(SOCKET clnt_soc)
{
    int i;

    /* 查找客户端对应的 socket 句柄 */
    for (i = 0; i < FINGER_MAX_SOC; i++)
    {
        if (hClntSoc[i] == clnt_soc)
        {
            closesocket(clnt_soc);
            hClntSoc[i] = INVALID_SOCKET;
            break;
        }
    }
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
    case FD_READ:
        if (wError)
        {
            LogPrintf(TEXT("FD_READ error #%i."), wError);
            return;
        }
        FingerOnRead(wParam);
        break;

    case FD_ACCEPT:
        if (wError || (wParam != hLstnSoc))
        {
            LogPrintf(TEXT("FD_ACCEPT error #%i."), wError);
            return;
        }
        FingerOnAccept(wParam);
        break;

    case FD_CLOSE:
        FingerOnClose(wParam);
        break;
    }
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