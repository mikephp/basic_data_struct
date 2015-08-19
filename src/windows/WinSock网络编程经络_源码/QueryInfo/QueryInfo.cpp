/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  QueryInfo.cpp
 *
 * PURPOSE :  查询主机, 服务, 协议信息.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "wsock32") /* WinSock 使用的库函数 */

/* 查询消息 */
#define WM_QUERYINFO_MSG    (WM_USER + 5)

/* 程序中使用的常量 */
#define STR_MAX_LEN         64
#define QUERY_MAX_LINE      4
#define QUERY_SHOW_X        20
#define QUERY_SHOW_Y        20

/* 定义要查询的信息 */
#define QUERY_HOST_NAME     "www.google.com"
#define QUERY_SERV_NAME     "telnet"
#define QUERY_PROTO_NAME    "tcp"

/* 查询的类型 */
enum QueryType
{
    QUERY_HOST,
    QUERY_SERV,
    QUERY_PROTO
};

typedef struct QueryInfo
{
    HINSTANCE hInstance;                      /* 程序实例句柄 */
    HANDLE    hQuery;                         /* 查询函数返回的异步句柄 */
    char      chQueryInfo[MAXGETHOSTSTRUCT];  /* 保存查询结果缓冲区 */
    char      chShowBuffer[MAXGETHOSTSTRUCT]; /* 显示查询结果缓冲区 */
    char      *pShowLine[QUERY_MAX_LINE];     /* 显示的行缓冲区 */
    int       nLine;                          /* 显示的行数 */
    int       nType;      /* 当前的查询类型, 值为 enum QueryType */
} QUERYINFO;


/*************************************************************************
 *  全局变量
 *************************************************************************/
QUERYINFO g_QueryInfo;

/* 函数的前向声明 */
ATOM QueryRegisterClass(HINSTANCE hInstance, TCHAR *szWindowClass);
BOOL QueryInitInstance(HINSTANCE hInstance, TCHAR *szWindowClass, int nCmdShow);
LRESULT CALLBACK QueryWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;
    WSADATA wsaData;
    TCHAR szWindowClass[STR_MAX_LEN];

    WSAStartup(MAKEWORD(2,0), &wsaData); /* 初始化 */

    g_QueryInfo.hQuery = 0;

    /* 装载类名称字符串 */
    LoadString(hInstance, IDC_QUERYINFO, szWindowClass, STR_MAX_LEN);
    QueryRegisterClass(hInstance, szWindowClass);

    /* 应用程序初始化 */
    if (!QueryInitInstance(hInstance, szWindowClass, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_QUERYINFO);

    /* 主消息循环 */
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    WSACleanup();   /* 释放 WinSock 资源 */
    return msg.wParam;
}

/*************************************************************************
 * FUNCTION : 注册窗口类.
 *
 * PARAMETER: 
 *  hInstance, 应用程序实例句柄.
 *	szWindowClass, 类名字.
 *
 * RETURN   : 注册成功，返回这个窗口类型的标识号，可以用标识号进行创建窗口，
 *            查找窗口和注销窗口等; 失败返回的值是 0.
 *************************************************************************/
ATOM QueryRegisterClass(HINSTANCE hInstance, TCHAR *szWindowClass)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)QueryWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_QUERYINFO);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = (LPCSTR)IDC_QUERYINFO;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx(&wcex);
}

/*************************************************************************
 * FUNCTION : 保存实例句柄并创建主窗口.
 *
 * PARAMETER: 
 *  hInstance, 应用程序实例句柄.
 *	szWindowClass, 类名字.
 *  nCmdShow, 是否显示.
 *
 * RETURN   : 成功返回 TRUE; 失败返回 FALSE.
 *************************************************************************/
BOOL QueryInitInstance(HINSTANCE hInstance, TCHAR *szWindowClass, int nCmdShow)
{
   HWND hWnd;
   TCHAR szTitle[STR_MAX_LEN]; /* 标题栏文本 */

   LoadString(hInstance, IDS_APP_TITLE, szTitle, STR_MAX_LEN);
   
   g_QueryInfo.hInstance = hInstance; /* 保存实例句柄 */

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
                       0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/*************************************************************************
 * FUNCTION : 根据用户的选择进行信息查询.
 *
 * PARAMETER: 
 *  hWnd, 窗口句柄.
 *	nType, 查询类型.
 *
 * RETURN   : void.
 *************************************************************************/
void QueryInformation(HWND hWnd, int nType)
{
    HANDLE hQuery;

    g_QueryInfo.nType = nType;

    if (g_QueryInfo.hQuery)
    {
        WSACancelAsyncRequest(g_QueryInfo.hQuery);
    }

    switch (nType)
    {
    case QUERY_HOST:
        hQuery = WSAAsyncGetHostByName(hWnd, WM_QUERYINFO_MSG, QUERY_HOST_NAME, 
            g_QueryInfo.chQueryInfo, MAXGETHOSTSTRUCT);
        break;

    case QUERY_SERV:
        hQuery = WSAAsyncGetServByName(hWnd, WM_QUERYINFO_MSG, QUERY_SERV_NAME, NULL, 
            g_QueryInfo.chQueryInfo, MAXGETHOSTSTRUCT);
        break;

    case QUERY_PROTO:
        hQuery = WSAAsyncGetProtoByName(hWnd, WM_QUERYINFO_MSG, QUERY_PROTO_NAME, 
            g_QueryInfo.chQueryInfo, MAXGETHOSTSTRUCT);
        break;

    default:
        return;
    }

    if (hQuery == 0)
    {
        MessageBox(hWnd, "Query Error?", " Query", MB_OK);
    }
    else
    {
        g_QueryInfo.hQuery = hQuery;
    }
}

/*************************************************************************
 * FUNCTION : 打印查询的主机信息.
 *
 * PARAMETER: 
 *  pQuery, 查询结果.
 *	pResult, 按一定格式保存查询结果的缓冲区, 为了在窗口上显示.
 *
 * RETURN   : 返回显示的行数.
 *************************************************************************/
int QueryHostPrint(char *pQuery, char *pResult)
{
    struct hostent	*hostent_p;
    struct in_addr addr;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    hostent_p = (struct hostent *)pQuery;
    length = sprintf(dest, "official name: %s", hostent_p->h_name); /* 输出正式主机名 */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);

    /* 输出主机的所有别名 */
    if (*hostent_p->h_aliases)
    {
        length = sprintf(dest, "alias:");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += length;
    }

    for(list_p = hostent_p->h_aliases; *list_p != NULL; list_p++)
    {
        length = sprintf(dest, "  %s;",*list_p);
        dest += length;
    }

    dest += 1; /* 结尾的 0 */

    /* 根据地址类型，将地址打出来 */	
    switch(hostent_p->h_addrtype) 
    {
    case AF_INET:
        length = sprintf(dest, "address type: AF_INET");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += (length + 1);
        break;

    case AF_INET6:
        length = sprintf(dest, "address type: AF_INET6");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += (length + 1);
        break;

    default:
        length = sprintf(dest, "address type is unknown");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += (length + 1);
        return -1; 
    }

    length = sprintf(dest, "address: ");
    g_QueryInfo.pShowLine[i++] = dest;
    dest += length;

    for(list_p = hostent_p->h_addr_list; *list_p != NULL; list_p++)
    {
        memcpy(&addr.S_un.S_addr, *list_p, hostent_p->h_length);
        length = sprintf(dest, "  %s;",inet_ntoa(addr));
        dest += length;
    }

    g_QueryInfo.nLine = i;	/* 记录行数 */

    return i;
}

/*************************************************************************
 * FUNCTION : 打印查询的服务信息.
 *
 * PARAMETER: 
 *  pQuery, 查询结果.
 *	pResult, 按一定格式保存查询结果的缓冲区, 为了在窗口上显示.
 *
 * RETURN   : 返回显示的行数.
 *************************************************************************/
int QueryServPrint(char *pQuery, char *pResult)
{
    struct servent	*servent_p;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    servent_p = (struct servent *)pQuery;
    length = sprintf(dest, "official name: %s", servent_p->s_name); /* 输出正式主机名 */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1); /* 1 是结尾的 0 */

    /* 输出主机的所有别名 */
    if (*servent_p->s_aliases)
    {
        length = sprintf(dest, "alias:");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += length;
    }

    for(list_p = servent_p->s_aliases; *list_p != NULL; list_p++)
    {
        length = sprintf(dest, "  %s;",*list_p);
        dest += length;
    }

    dest += 1; /* 结尾的 0 */

    length = sprintf(dest, "port: %d", ntohs(servent_p->s_port)); /* 输出端口   */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);	/* 1 是结尾的 0 */

    length = sprintf(dest, "protocol: %s", servent_p->s_proto);   /* 输出协议名 */
    g_QueryInfo.pShowLine[i++] = dest;

    g_QueryInfo.nLine = i; /* 记录行数 */

    return i;
}

/*************************************************************************
 * FUNCTION : 打印查询的协议信息.
 *
 * PARAMETER: 
 *  pQuery, 查询结果.
 *	pResult, 按一定格式保存查询结果的缓冲区, 为了在窗口上显示.
 *
 * RETURN   : 返回显示的行数.
 *************************************************************************/
int QueryProtoPrint(char *pQuery, char *pResult)
{
    struct protoent	*protoent_p;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    protoent_p = (struct protoent *)pQuery;
    /* 输出正式主机名 */
    length = sprintf(dest, "official name: %s", protoent_p->p_name); 
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);	/* 1 是结尾的 0 */

    /* 输出主机的所有别名 */
    if (*protoent_p->p_aliases)
    {
        length = sprintf(dest, "alias:");
        g_QueryInfo.pShowLine[i++] = dest;
        dest += length;
    }

    for(list_p = protoent_p->p_aliases; *list_p != NULL; list_p++) 
    {
        length = sprintf(dest, "  %s;",*list_p);
        dest += length;
    }

    dest += 1; /* 结尾的 0 */

    /* 输出协议名 */
    length = sprintf(dest, "protocol number: %d", protoent_p->p_proto);   
    g_QueryInfo.pShowLine[i++] = dest;

    g_QueryInfo.nLine = i;  /* 记录行数 */

    return i;
}

/*************************************************************************
 * FUNCTION : 在窗口上显示查询信息.
 *
 * PARAMETER: 
 *  hWnd, 窗口句柄.
 *	hDC, 显示设备句柄.
 *
 * RETURN   : void.
 *************************************************************************/
void QueryResultShow(HWND hWnd, HDC hDC)
{
    int i, cyChar, nBufLen;
    TEXTMETRIC tm;

    if (g_QueryInfo.hQuery == 0)
    {
        return;
    }

    GetTextMetrics(hDC, &tm);
    cyChar = (tm.tmHeight + tm.tmExternalLeading) * 2;

    for (i = 0; i < g_QueryInfo.nLine; i++)
    {
        nBufLen = strlen(g_QueryInfo.pShowLine[i]);
        TextOut(hDC, QUERY_SHOW_X, QUERY_SHOW_Y * 2 + cyChar * i, g_QueryInfo.pShowLine[i], nBufLen);
    }
}

/*************************************************************************
 * FUNCTION : 处理用户选择的菜单命令.
 *
 * PARAMETER: 
 *  hWnd, 应用程序窗口句柄.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : 0.
 *************************************************************************/
LRESULT CALLBACK OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    wmId    = LOWORD(wParam);
    wmEvent = HIWORD(wParam);

    /* 处理菜单命令 */
    switch (wmId)
    {
    case IDM_ABOUT:
        DialogBox(g_QueryInfo.hInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
        break;
    case IDM_EXIT:
        DestroyWindow(hWnd);
        break;

    case IDM_QUERY_HOST:
        QueryInformation(hWnd, QUERY_HOST);
        break;

    case IDM_QUERY_SERV:
        QueryInformation(hWnd, QUERY_SERV);
        break;

    case IDM_QUERY_PROTO:
        QueryInformation(hWnd, QUERY_PROTO);
        break;

    case IDM_QUERY_CANCEL:
        if (g_QueryInfo.hQuery)
        {
            WSACancelAsyncRequest(g_QueryInfo.hQuery);
        }
        break;
    }

    return 0;
}

/*************************************************************************
 * FUNCTION : 处理主机, 服务, 协议查询消息.
 *
 * PARAMETER: 
 *  hWnd, 应用程序窗口句柄.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : void.
 *************************************************************************/
void OnQueryInfoMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rectClient;
    WORD wError;

    wError = WSAGETASYNCERROR(lParam);

    if (wError != 0)
    {
        MessageBox(hWnd, "Query Service Error?", "Query Service", MB_OK);
        return;
    }

    GetClientRect(hWnd, &rectClient);

    if (g_QueryInfo.hQuery == 0)
    {
        return;
    }

    switch (g_QueryInfo.nType)
    {
    case QUERY_HOST:
        QueryHostPrint(g_QueryInfo.chQueryInfo, g_QueryInfo.chShowBuffer);
        break;

    case QUERY_SERV:
        QueryServPrint(g_QueryInfo.chQueryInfo, g_QueryInfo.chShowBuffer);
        break;

    case QUERY_PROTO:
        QueryProtoPrint(g_QueryInfo.chQueryInfo, g_QueryInfo.chShowBuffer);
        break;

    default:
        return;
    }

    InvalidateRect(hWnd, &rectClient, TRUE);
}

/*************************************************************************
 * FUNCTION : 窗口处理过程, 处理该窗口的消息.
 *
 * PARAMETER: 
 *  hWnd, 应用程序窗口句柄.
 *	message, 消息名.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : 成功返回 TRUE; 失败返回 FALSE.
 *************************************************************************/
LRESULT CALLBACK QueryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR szHello[STR_MAX_LEN];
    LoadString(g_QueryInfo.hInstance, IDS_HELLO, szHello, STR_MAX_LEN);

    switch (message)
    {
    case WM_COMMAND:
        OnCommandMsg(hWnd, wParam, lParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        QueryResultShow(hWnd, hdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_QUERYINFO_MSG:
        OnQueryInfoMsg(hWnd, wParam, lParam);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

/*************************************************************************
 * FUNCTION : About 对话框处理过程.
 *
 * PARAMETER: 
 *  hWnd, 对话框窗口句柄.
 *	message, 消息名.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : 成功返回 TRUE; 失败返回 FALSE.
 *************************************************************************/
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }

    return FALSE;
}
