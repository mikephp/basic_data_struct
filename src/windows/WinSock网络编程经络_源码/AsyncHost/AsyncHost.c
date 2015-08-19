/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  AsyncHost.c
 *
 * PURPOSE :  异步域名查询程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

/* 定义域名查询消息 */
#define WM_GETHOST_NOTIFY  (WM_USER + 1)

/* 定义控件的风格 */
#define STATIC_STYLE  (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define EDIT_STYLE    (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define BUTTON_STYLE  (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)

/* 控件的标识, 是控件在数组中的下标 */
#define ID_EDIT_HOST     1 /* 主机 */
#define ID_BTN_QUERY     2 /* 查询按钮的 ID */

#define HOST_BUF_LEN  1024 /* 缓冲区长度 */
#define HOST_POS_X      60 /* 输出信息左上角 x 位置 */
#define HOST_POS_Y      80 /* 输出信息左上角 y 位置 */

#define TABLE_SIZE(a)   (sizeof a / sizeof a[0])

/* 控件的属性结构 */
struct Widget
{
    int iLeft;      /* 左上角的 x 坐标 */
    int iTop;       /* 左上角的 y 坐标 */
    int iWidth;     /* 宽度 */
    int iHeigh;     /* 高度 */
    int iStyle;     /* 控件的风格 */
    TCHAR *szType;  /* 控件类型: static, button, edit. */
    TCHAR *szTitle; /* 控件上显示的文字 */
};

struct QueryHost
{
    HANDLE hAsyncHost; /* 域名查询句柄 */
    char cHostEnt[MAXGETHOSTSTRUCT]; /* 域名查询缓冲区 */
    TCHAR szInfoBuf[HOST_BUF_LEN];   /* 输出信息的缓冲区 */
};

/* 定义域名查询程序使用的控件 */
static struct Widget HostWgt[] = 
{
    /* 主机标签 */
    { 1, 2, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("主机:") },
    /* 主机名称 */
    { 7, 2, 40, 2, EDIT_STYLE,   TEXT("edit"), TEXT("") },
    /* 查询按钮 */
    { 50, 2, 12, 2, BUTTON_STYLE, TEXT("button"), TEXT("查询") }
};

/* 全局变量 */
static struct QueryHost gQureyHost;
static HWND hWndWgt[TABLE_SIZE(HostWgt)];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    TCHAR szClassName[] = TEXT("AsyncHost");
    HWND        hWnd;
    MSG         msg;
    WNDCLASS    wndclass;
    WSADATA     wsa_data;
    
    WSAStartup(MAKEWORD(2,0), &wsa_data);

    /* 注册窗口类 */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* 这是窗口过程 */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL; /* 没有菜单 */
    wndclass.lpszClassName = szClassName; /* 窗口类名 */
    
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Requires Windows NT!"), szClassName, 0);
        return 0;
    }

    hWnd = CreateWindow(szClassName,          /* 与注册的类名相同 */
                        TEXT("Query Host"),   /* 窗口标题 */
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

    WSACleanup();
    return msg.wParam;
}

void HostPrint(char *pHostEnt, TCHAR *szBuf)
{
    int result;
    char **list_p;
    struct hostent *ent_p = (struct hostent *)pHostEnt;
    struct in_addr addr;
    TCHAR *pOut = szBuf;

    /* 输出正式主机名 */
    result = wsprintf(pOut, TEXT("official name: %s\r\n"), ent_p->h_name);
    pOut += result;

    /* 输出主机的所有别名 */
    if (*ent_p->h_aliases)
    {
        result = wsprintf(pOut, TEXT("alias:\r\n"));
        pOut += result;
    }

    for(list_p = ent_p->h_aliases; *list_p != NULL; list_p++)
    {
        result = wsprintf(pOut, TEXT("    %s\r\n"),*list_p);
        pOut += result;
    }
    
    /* 根据地址类型, 将地址打印出来 */	
    switch(ent_p->h_addrtype) 
    {
    case AF_INET:
        result = wsprintf(pOut, TEXT("address type: AF_INET\r\n"));
        break; 

    case AF_INET6:
        result = wsprintf(pOut, TEXT("address type: AF_INET6\r\n"));
         break;

    default: 
        result = wsprintf(pOut, TEXT("address type is unknown\r\n"));
        return; 
    }

    pOut += result;
    result = wsprintf(pOut, TEXT("address:\r\n"));
    for(list_p = ent_p->h_addr_list; *list_p != NULL; list_p++) 
    {
        memcpy(&addr.S_un.S_addr, *list_p, ent_p->h_length);
        pOut += result;
        result = wsprintf(pOut, "    %s\r\n",inet_ntoa(addr));
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                         WPARAM wParam, LPARAM lParam)
{
    HDC         hDC;
    PAINTSTRUCT ps;
    RECT        rect;
    int i, cxChar, cyChar, iCount = TABLE_SIZE(HostWgt);
    int wmId, wmEvent, iError;
    struct Widget *pWgt;
    char szHost[HOST_BUF_LEN];
    
    switch (message)
    {
    case WM_CREATE:
        cxChar = LOWORD(GetDialogBaseUnits());
        cyChar = HIWORD(GetDialogBaseUnits());

        for (i = 0; i < iCount; i++)
        {
            pWgt = &HostWgt[i];
            hWndWgt[i] = CreateWindow(pWgt->szType, pWgt->szTitle,
                pWgt->iStyle, pWgt->iLeft * cxChar, pWgt->iTop * cyChar,
                pWgt->iWidth * cxChar, pWgt->iHeigh * cyChar, hWnd, 
                (HMENU) i, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        }
        /* 初始化 */
        SetWindowText(hWndWgt[ID_EDIT_HOST], TEXT("www.google.com"));
        return 0;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        if (gQureyHost.hAsyncHost)
        {
            GetClientRect(hWnd, &rect);

            rect.left = HOST_POS_X;
            rect.top = HOST_POS_Y;
            /* 显示信息 */
            DrawText(hDC, gQureyHost.szInfoBuf, -1, &rect, DT_LEFT);
        }

        EndPaint(hWnd, &ps);
        return 0;

    case WM_COMMAND:
        wmId    = LOWORD(wParam); 
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case ID_BTN_QUERY: /* 查询 */
            if (wmEvent != BN_CLICKED)
                break;

            /* 处理 BN_CLICKED, 得到用户输入的信息 */
            GetWindowText(hWndWgt[ID_EDIT_HOST], szHost, 
                          HOST_BUF_LEN);

            gQureyHost.hAsyncHost = WSAAsyncGetHostByName(hWnd, 
                           WM_GETHOST_NOTIFY, szHost, 
                           gQureyHost.cHostEnt, MAXGETHOSTSTRUCT);
            if (gQureyHost.hAsyncHost == 0)
                MessageBox(hWnd, TEXT("Get Host Error"), NULL, 0);
            break;

        default:
            return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
        }
        return 0;

    case WM_GETHOST_NOTIFY:
        iError = WSAGETASYNCERROR(lParam);
        if (iError || wParam != (WPARAM)gQureyHost.hAsyncHost)
        {
            MessageBox(hWnd, TEXT("Get Host Result Error"), NULL, 0);
            gQureyHost.hAsyncHost = 0;
            return 0; /* 发生错误 */
        }

        HostPrint(gQureyHost.cHostEnt, gQureyHost.szInfoBuf);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}