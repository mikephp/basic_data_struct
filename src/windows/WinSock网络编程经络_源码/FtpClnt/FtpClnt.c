/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpClnt.c
 *
 * PURPOSE :  FTP 程序入口, 用户界面.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "resource.h"
#include "FtpClnt.h"
#include "FtpProt.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

/* 定义 FTP 程序使用的控件 */
static struct Widget FtpWgt[] = 
{
    /* 主机 */
    { 1, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("主机:") },
    { 7, 0, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("127.0.0.1") },
    /* 端口 */
    { 32, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("端口:") },
    { 38, 0, 8, 2, EDIT_STYLE,   TEXT("edit"), TEXT("21") },
    /* 用户 */
    { 47, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("用户:") },
    { 52, 0, 16, 2, EDIT_STYLE,   TEXT("edit"), TEXT("helen") },
    /* 密码 */
    { 69, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("密码:") },
    { 75, 0, 20, 2, EDIT_STYLE,   TEXT("edit"), TEXT("1234") },
    /* 连接 */
    { 96, 0, 10, 2, BUTTON_STYLE, TEXT("button"), TEXT("连接") },
    /* 断连 */
    { 107, 0, 10, 2, BUTTON_STYLE, TEXT("button"), TEXT("断连") },

    /* 本地目录 */
    { 1, 3, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("本地:") },
    { 7, 3, 12, 2, EDIT_STYLE,   TEXT("edit"), TEXT("C:\\") },
    /* 远端目录 */
    { 20, 3, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("远端:") },
    { 26, 3, 12, 2, EDIT_STYLE,   TEXT("edit"), TEXT("/") },

    /* 文件列表 */
    { 1, 5, 24,  20, LIST_STYLE,   TEXT("listbox"), NULL },
    { 25, 5, 24, 20, LIST_STYLE,   TEXT("listbox"), NULL },

    /* 信息 */
    { 1, 25, 64, 20, EDIT_STYLE_EX, TEXT("edit"), TEXT("") }
};


/* 全局变量 */
static HWND hWndWgt[TABLE_SIZE(FtpWgt)];
struct ftp_ctrl_s ftp_ctrl;

/* 函数声明 */
static ATOM FtpRegisterClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK ListProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    ftp_startup(); /* 初始化 FTP 协议 */

    /* 初始化全局字符串 */
    LoadString(hInstance, IDS_APP_TITLE, ftp_ctrl.szTitle, MAX_LOAD_STR);
    LoadString(hInstance, IDC_FTPCLNT, ftp_ctrl.szClass, MAX_LOAD_STR);

    FtpRegisterClass(hInstance);
    InitInstance(hInstance, nCmdShow); /* 应用程序初始化 */
    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FTPCLNT);

    /* 消息循环 */
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ftp_cleanup(TRUE); /* 释放 FTP 协议资源 */
    return msg.wParam;
}

/**************************************************************************
 *
 * 函数功能: 注册 FTP 程序的窗口类.
 *
 * 参数说明: [IN] hInstance, 应用程序实例句柄.
 *
 * 返 回 值: 函数成功时返回值不等于0, 它唯一标识了被注册的类; 失败时返回0.
 *
 **************************************************************************/
static ATOM FtpRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_FTPCLNT);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = (LPCSTR)IDC_FTPCLNT;
    wcex.lpszClassName  = ftp_ctrl.szClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx(&wcex);
}

/**************************************************************************
 *
 * 函数功能: 创建并显示应用程序窗口.
 *
 * 参数说明: [IN] hInstance, 应用程序实例句柄.
 *           [IN] nCmdShow, 窗口显示方式, 如正常、最大化、最小化、隐藏等.
 *
 * 返 回 值: 函数成功时返回 TRUE; 失败时返回 FALSE.
 *
 **************************************************************************/
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    ftp_ctrl.hWnd = CreateWindow(ftp_ctrl.szClass, ftp_ctrl.szTitle, 
                     WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 
                     CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ftp_ctrl.hInst = hInstance; /* 保存应用程序实例句柄 */
    ShowWindow(ftp_ctrl.hWnd, nCmdShow);
    UpdateWindow(ftp_ctrl.hWnd);

    return TRUE;
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
static void FtpOnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
    TCHAR szPath[MAX_PATH + 1];
    HWND hWndLst;
    int i, iCount = TABLE_SIZE(FtpWgt);
    int cxChar, cyChar;
    struct Widget *pWgt;

    cxChar = LOWORD(GetDialogBaseUnits());
    cyChar = HIWORD(GetDialogBaseUnits());

    GetCurrentDirectory(MAX_PATH + 1, szPath);
    FtpWgt[ID_DIR_LOCAL + 1].szTitle = szPath;

    for (i = 0; i < iCount; i++)
    {
        pWgt = &FtpWgt[i];
        hWndWgt[i] = CreateWindow(pWgt->szType, pWgt->szTitle,
            pWgt->iStyle,
            pWgt->iLeft * cxChar, pWgt->iTop * cyChar, 
            pWgt->iWidth * cxChar, pWgt->iHeigh * cyChar, 
            hWnd, (HMENU) i, hInstance, NULL);
    }

    hWndLst = hWndWgt[ID_FILE_LOCAL];
    ftp_ctrl.ListProc = (WNDPROC)SetWindowLong(hWndWgt[ID_FILE_LOCAL], 
        GWL_WNDPROC, (LPARAM)ListProc);
    SetWindowLong(hWndWgt[ID_FILE_REMOTE], GWL_WNDPROC, 
        (LPARAM)ListProc);

    SendMessage(hWndLst, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
    FtpEnableConnWindow(TRUE);
}

/**************************************************************************
 *
 * 函数功能: 得到窗口客户区大小.
 *
 * 参数说明: [IN] hWnd, 父窗口句柄.
 *           [IN] hSubWnd, 子窗口句柄.
 *           [OUT] rt, 子窗口的大小.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpGetClientRect(HWND hWnd, HWND hSubWnd, RECT *rt)
{
    POINT pt;

    GetWindowRect(hSubWnd, rt);

    pt.x = rt->left;
    pt.y = rt->top;

    ScreenToClient(hWnd, &pt);

    rt->left = pt.x;
    rt->top = pt.y;

    pt.x = rt->right;
    pt.y = rt->bottom;

    ScreenToClient(hWnd, &pt);

    rt->right = pt.x;
    rt->bottom = pt.y;
}

/**************************************************************************
 *
 * 函数功能: 处理窗口改变大小消息, 调整子窗口的位置及大小.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] cx, 窗口宽度.
 *           [IN] cy, 窗口高度.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpOnSize(HWND hWnd, int cx, int cy)
{
    int i, cxHalf, cxOffset, iRight, iIndex;
    int iWidth, iHeight, cxChar, cyChar;
    RECT rtWidget;

    cxChar = LOWORD(GetDialogBaseUnits());
    cyChar = HIWORD(GetDialogBaseUnits());
    cxOffset = cxChar;
    cxHalf = cx / 2;

    for (i = 0; i < 4; i += 2)
    {        
        iIndex = ID_DIR_LOCAL + i;

        /* 目录说明 */
        FtpGetClientRect(hWnd, hWndWgt[iIndex], &rtWidget);
        
        iWidth = rtWidget.right - rtWidget.left;
        iHeight= rtWidget.bottom - rtWidget.top;
        rtWidget.left = cxOffset;
        MoveWindow(hWndWgt[iIndex], rtWidget.left, rtWidget.top, 
                   iWidth, iHeight, FALSE);

        iRight = rtWidget.left + iWidth;
        iIndex += 1;

        /* 目录 */
        FtpGetClientRect(hWnd, hWndWgt[iIndex], &rtWidget);
        iWidth = cxHalf - cxChar * 7;
        rtWidget.left = iRight;
        iHeight= rtWidget.bottom - rtWidget.top;
        MoveWindow(hWndWgt[iIndex], rtWidget.left, rtWidget.top, 
                   iWidth, iHeight, FALSE);

        cxOffset += cxHalf;
    }

    cxOffset = cxChar;
    for (i = 0; i < 2; i++)
    {
        iIndex = ID_FILE_LOCAL + i;
        
        /* 文件列表 */
        FtpGetClientRect(hWnd, hWndWgt[iIndex], &rtWidget);
        iWidth = cxHalf - cxChar;
        iHeight= rtWidget.bottom - rtWidget.top;
        rtWidget.left = cxOffset;
        MoveWindow(hWndWgt[iIndex], rtWidget.left, rtWidget.top, 
                   iWidth, iHeight, FALSE);

        cxOffset += cxHalf;
    }

    FtpGetClientRect(hWnd, hWndWgt[iIndex], &rtWidget);

    iIndex = ID_EDIT_TRACE;
    FtpGetClientRect(hWnd, hWndWgt[ID_FILE_LOCAL], &rtWidget);
    iHeight = rtWidget.bottom + cyChar;
    MoveWindow(hWndWgt[iIndex], rtWidget.left, iHeight, 
               cx - cxChar, cy - iHeight, FALSE);
}

/**************************************************************************
 *
 * 函数功能: 用户单击连接按钮处理.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpOnCmdConn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    char cBuff[8], *szPath;
    unsigned short sPort;
    int iResult, wmEvent = HIWORD(wParam); 

    if (wmEvent != BN_CLICKED)
        return;

    /* 处理 BN_CLICKED, 得到用户输入的信息 */
    GetWindowText(hWndWgt[ID_EDIT_HOST], ftp_ctrl.cHost,FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_USER], ftp_ctrl.cUser, FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_PASS], ftp_ctrl.cPass, FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_PORT], cBuff, 8);

    /* 得到服务器上的目录 */
    strcpy(ftp_ctrl.cPath, "/");
    szPath = strchr(ftp_ctrl.cHost, '/');
    if (szPath)
    {
        *szPath = 0;
        if (*(szPath+1) != 0)
            strcpy(ftp_ctrl.cPath, szPath+1);
    }

    sPort = atoi(cBuff);
    ftp_ctrl.sPort = htons((u_short)(sPort == 0 ? FTP_CTRL_PORT : sPort));

    /* 做域名查询 */
    iResult = ftp_do_event(EV_HOST, hWnd, ftp_ctrl.cHost);
    if (iResult == FTPE_SUCESS)
        FtpEnableConnWindow(FALSE);
}

/**************************************************************************
 *
 * 函数功能: 本地文件列表 LIST 控件双击处理.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 双击消息的 WPARAM 参数.
 *           [IN] lParam, 双击消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpOnCmdList(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HANDLE hFile;
    int i, wmEvent = HIWORD(wParam);
    HWND hWndDir, hWndList = (HWND)lParam;
    TCHAR szPath[MAX_PATH + 1], szFile[MAX_PATH + 1];

    if (wmEvent != LBN_DBLCLK)
        return;

    hWndDir = hWndWgt[ID_DIR_LOCAL + 1];

    /* 处理 LBN_DBLCLK, 得到用户输入的信息 */
    i = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
    if (LB_ERR == i)
        return;

    SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szPath);
    
    hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        szPath[lstrlen (szPath) - 1] = '\0';
        
        /* 设置目录失败, 应该是驱动器 */
        if (!SetCurrentDirectory(szPath + 1))
        {
            szPath[3] = ':';
            szPath[4] = '\0';
            SetCurrentDirectory(szPath + 2);
        }

        /* 得到新的目录, 并填充列表框 */
        GetCurrentDirectory(MAX_PATH + 1, szPath);
        SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
        SendMessage(hWndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
    }
    else /* 选择的是文件 */
    {
        CloseHandle(hFile) ;
        lstrcpy(szFile, szPath) ;
        GetCurrentDirectory(MAX_PATH + 1, szPath);

        if (szPath[lstrlen(szPath) - 1] != '\\')
            lstrcat(szPath, TEXT("\\"));

        lstrcat(szPath, szFile);
    }

    SetWindowText(hWndDir, szPath);
}

/**************************************************************************
 *
 * 函数功能: 远端文件列表 LIST 控件双击处理.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 双击消息的 WPARAM 参数.
 *           [IN] lParam, 双击消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpOnRemoteList(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i, wmEvent = HIWORD(wParam);
    TCHAR szPath[FTP_NAME_LEN];
    HWND hList = (HWND)lParam;

    if (wmEvent != LBN_DBLCLK)
        return;

    /* 处理 LBN_DBLCLK, 得到用户选择的信息 */
    i = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (i == LB_ERR)
        return;

    i = SendMessage(hList, LB_GETTEXT, i, (LPARAM)szPath);
    
    strcpy(ftp_ctrl.cPath, szPath);
    ftp_do_event(EV_LDIR, NULL, NULL);
}

/**************************************************************************
 *
 * 函数功能: 处理 FTP 程序的各种命令.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static LRESULT FtpOnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    wmId    = LOWORD(wParam); 
    wmEvent = HIWORD(wParam); 

    switch (wmId)
    {
    case ID_BTN_CONN: /* 连接 */
        FtpOnCmdConn(hWnd, wParam, lParam);
        break;

    case ID_BTN_DISC: /* 断连 */
        ftp_cleanup(FALSE);
        FtpEnableConnWindow(TRUE);
        /* 清空远端列表框 */
        SendMessage(hWndWgt[ID_FILE_REMOTE], LB_RESETCONTENT, 0,0); 
        break;

    case ID_FILE_LOCAL: /* 处理本地文件列表项的双击 */
        FtpOnCmdList(hWnd, wParam, lParam);
        break;

    case ID_FILE_REMOTE: /* 远端文件列表项的双击 */
        FtpOnRemoteList(hWnd, wParam, lParam);
        break;

    case IDM_ABOUT:
        DialogBox(ftp_ctrl.hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
        break;
    case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
    }

    return 0;
}

/**************************************************************************
 *
 * 函数功能: 收到域名查询消息的处理.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
static void FtpOnHostNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    unsigned long addr;
    struct hostent *host_ent;
    int iError = WSAGETASYNCERROR(lParam);
    
    if ((wParam != (WPARAM)ftp_ctrl.hAsyncHost) || iError)
    {
        ftp_ctrl.iStatus = FTPS_CLOSED;
        FtpEnableConnWindow(TRUE);
        FtpPrintf(TEXT("Get host error #%i."), iError);
        return; /* 域名查询错误 */
    }

    host_ent = (struct hostent *)ftp_ctrl.HostEnt;
    addr = *(unsigned long *)host_ent->h_addr; /* 网络字节序 */
    iError = ftp_do_event(EV_CONN, hWnd, &addr); /* 与服务器建立连接 */
}

/**************************************************************************
 *
 * 函数功能: socket 事件处理.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: 事件处理结果, 成功是 TRUE, 失败是 FALSE.
 *
 **************************************************************************/
static void FtpOnSocketNotify(WPARAM wParam, LPARAM lParam)
{
    int iResult = 0;
    WORD wEvent, wError;

    wEvent = WSAGETSELECTEVENT(lParam); /* LOWORD */
    wError = WSAGETSELECTERROR(lParam); /* HIWORD */
    
    switch (wEvent)
    {       
    case FD_CONNECT: /* connect() 的结果 */
        if (wError)
        {
            ftp_cleanup(FALSE);
            FtpEnableConnWindow(TRUE);
            FtpPrintf(TEXT("FD_CONNECT error #%i."), wError);
            return;
        }
        FtpPrintf(TEXT("=====================================\r\n"));
        FtpPrintf(TEXT("> Connect Successfully.\r\n"));
        break;
        
    case FD_READ:
        if (wError)
        {
            FtpPrintf(TEXT("FD_READ error #%i."), wError);
            return;
        }
        iResult = ftp_recv_process(wParam);
        break;

    case FD_ACCEPT:
        if (wError)
        {
            FtpPrintf(TEXT("FD_ACCEPT error #%i."), wError);
            return;
        }
        iResult = ftp_accept_conn(wParam);
        break;

    case FD_WRITE:
        if (wError)
        {
            FtpPrintf(TEXT("FD_WRITE error #%i."), wError);
            return;
        }
        iResult = ftp_store_file(wParam); /* 上传文件 */
        break;

    case FD_CLOSE:
        ftp_close_process(wParam);
        break;
    }
}

/**************************************************************************
 *
 * 函数功能: 窗口过程函数, 处理应用程序的窗口消息.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] message, 要处理的消息.
 *           [IN] wParam, 消息的 WPARAM 参数.
 *           [IN] lParam, 消息的 LPARAM 参数.
 *
 * 返 回 值: 0.
 *
 **************************************************************************/
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                                WPARAM wParam, LPARAM lParam)
{
    int cxClient, cyClient;

    switch (message) 
    {
    case WM_CREATE:
        FtpOnCreate(hWnd, wParam, lParam);
        break;

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        FtpOnSize(hWnd, cxClient, cyClient);
        break;

    case WM_COMMAND:
        FtpOnCommand(hWnd, wParam, lParam);
        break;

    case WM_SETFOCUS :
        SetFocus(hWndWgt[ID_FILE_LOCAL]);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_GETHOST_NOTIFY:
        FtpOnHostNotify(hWnd, wParam, lParam);
        break;

    case WM_SOCKET_NOTIFY:
        FtpOnSocketNotify(wParam, lParam);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

/**************************************************************************
 *
 * 函数功能: 处理鼠标的移动操作.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] lParam, 消息的 LPARAM 参数.
 *
 * 返 回 值: 合理的操作返回操作码, 否则返回 0.
 *
 **************************************************************************/
int MouseListOperation(HWND hWnd, LPARAM lParam)
{
    HWND hWndLst;
    POINT ptPos;
    RECT rect;
    int op;

    if (ftp_ctrl.iSourID == ID_FILE_LOCAL)
    {
        hWndLst = hWndWgt[ID_FILE_REMOTE];
        op = FTP_OP_STOR;
    }
    else if (ftp_ctrl.iSourID == ID_FILE_REMOTE)
    {
        hWndLst = hWndWgt[ID_FILE_LOCAL];
        op = FTP_OP_RETR;
    }
    else
        return 0;

    GetWindowRect(hWndLst, &rect);
    GetCursorPos(&ptPos);    

    if (ftp_ctrl.iMouseMove && PtInRect(&rect, ptPos))
        return op;

    return 0;
}

/**************************************************************************
 *
 * 函数功能: LIST 窗口过程函数, 处理 LIST 的消息.
 *
 * 参数说明: [IN] hWnd, 窗口句柄.
 *           [IN] message, 要处理的消息.
 *           [IN] wParam, 单击消息的 WPARAM 参数.
 *           [IN] lParam, 单击消息的 LPARAM 参数.
 *
 * 返 回 值: 处理结果.
 *
 **************************************************************************/
static LRESULT CALLBACK ListProc(HWND hWnd, UINT message, 
                                 WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    HCURSOR hCurs;
    int i, op;
    int id = GetWindowLong(hWnd, GWL_ID);

    lResult = CallWindowProc(ftp_ctrl.ListProc, hWnd, message, 
                             wParam, lParam);

    switch (message)
    {
    case WM_KEYDOWN:
        if ((wParam == VK_RETURN) && (id == ID_FILE_LOCAL))
            SendMessage(GetParent(hWnd), WM_COMMAND, 
                        MAKELONG(id, LBN_DBLCLK), (LPARAM)hWnd);
        break;

    case WM_LBUTTONDOWN:
        i = SendMessage(hWnd, LB_GETCURSEL, 0, 0);
        if (i == LB_ERR)
            break;

        ftp_ctrl.iLBtnDown = TRUE;
        SendMessage(hWnd, LB_GETTEXT, i, (LPARAM)ftp_ctrl.cSelFile);

        /* 记录鼠标按下的 listbox */
        if (hWnd == hWndWgt[ID_FILE_LOCAL])
            ftp_ctrl.iSourID = ID_FILE_LOCAL;
        else if (hWnd == hWndWgt[ID_FILE_REMOTE])
            ftp_ctrl.iSourID = ID_FILE_REMOTE;
        
        break;

    case WM_MOUSEMOVE:
        if (!ftp_ctrl.iLBtnDown)
            break;
        
        ftp_ctrl.iMouseMove = 1;
        hCurs = LoadCursor(ftp_ctrl.hInst, MAKEINTRESOURCE(IDC_COPY));
        SetCursor(hCurs);
        break;

    case WM_LBUTTONUP:
        ftp_ctrl.iLBtnDown = 0;

        if (!ftp_ctrl.iMouseMove)
            break;
        
        op = MouseListOperation(hWnd, lParam);
        ftp_file_process(op);
        ftp_ctrl.iMouseMove = 0;
        break;
    }

    return lResult;
}

static LRESULT CALLBACK About(HWND hDlg, UINT message, 
                              WPARAM wParam, LPARAM lParam)
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

/**************************************************************************
 *
 * 函数功能: 在远端文件列表框中增加新的文件.
 *
 * 参数说明: [IN] szFile, 文件数据.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void FtpRemoteListAdd(char *szFile)
{
    char *pEnd;
    TCHAR szName[MAX_PATH + 1];

    SetWindowText(hWndWgt[ID_SRV_PATH], ftp_ctrl.cPath);
    SendMessage(hWndWgt[ID_FILE_REMOTE], LB_RESETCONTENT, 0, 0);

    do 
    {
        pEnd = strchr(szFile, '\r');
        if (!pEnd)
            break;

        *pEnd = 0;
        if (*(pEnd + 1) == '\n')
            *(++pEnd) = 0;

#ifdef UNICODE
        MultiByteToWideChar(CP_ACP, 0, szFile, -1, szName, MAX_PATH);
#else
        strcpy(szName, szFile);
#endif
        SendMessage(hWndWgt[ID_FILE_REMOTE], LB_ADDSTRING, 0, 
                    (LPARAM)szName);
        szFile = ++pEnd;
    } while(*szFile);
}

/**************************************************************************
 *
 * 函数功能: 在本地或远端文件列表框中增加新的文件.
 *
 * 参数说明: [IN] iLocal, 是否是本地文件.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void FtpListAddFile(int iLocal)
{
    int id = iLocal ? ID_FILE_LOCAL : ID_FILE_REMOTE;
    int result;
    TCHAR *szName = ftp_ctrl.cSelFile;

    result = SendMessage(hWndWgt[id], LB_FINDSTRINGEXACT, -1, 
                         (LPARAM)szName);
    if (result == LB_ERR)
        SendMessage(hWndWgt[id], LB_ADDSTRING, -1, (LPARAM)szName);
}

/**************************************************************************
 *
 * 函数功能: 设置连接和断连按钮的状态.
 *
 * 参数说明: [IN] bEnable, 是否允许连接.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void FtpEnableConnWindow(int bEnable)
{
    EnableWindow(hWndWgt[ID_BTN_CONN], bEnable);
    EnableWindow(hWndWgt[ID_BTN_DISC], !bEnable);
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
void FtpPrintf(TCHAR *szFormat, ...)
{
    int iBufLen = 0;
    TCHAR szBuffer[FTP_BUF_LEN];
    va_list pVaList;
    HWND hWndEdit = hWndWgt[ID_EDIT_TRACE];
    
    va_start(pVaList, szFormat);

#ifdef UNICODE
    iBufLen = _vsnwprintf(szBuffer, FTP_BUF_LEN, szFormat, pVaList);
#else
    iBufLen = _vsnprintf(szBuffer, FTP_BUF_LEN, szFormat, pVaList);
#endif

    va_end(pVaList);

    SendMessage(hWndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM)szBuffer);
    SendMessage(hWndEdit, EM_SCROLLCARET, 0, 0);

    ftp_log_write(szBuffer, iBufLen);
}