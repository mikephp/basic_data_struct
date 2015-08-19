/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpClnt.c
 *
 * PURPOSE :  FTP �������, �û�����.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "resource.h"
#include "FtpClnt.h"
#include "FtpProt.h"

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

/* ���� FTP ����ʹ�õĿؼ� */
static struct Widget FtpWgt[] = 
{
    /* ���� */
    { 1, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("����:") },
    { 7, 0, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("127.0.0.1") },
    /* �˿� */
    { 32, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("�˿�:") },
    { 38, 0, 8, 2, EDIT_STYLE,   TEXT("edit"), TEXT("21") },
    /* �û� */
    { 47, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("�û�:") },
    { 52, 0, 16, 2, EDIT_STYLE,   TEXT("edit"), TEXT("helen") },
    /* ���� */
    { 69, 0, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("����:") },
    { 75, 0, 20, 2, EDIT_STYLE,   TEXT("edit"), TEXT("1234") },
    /* ���� */
    { 96, 0, 10, 2, BUTTON_STYLE, TEXT("button"), TEXT("����") },
    /* ���� */
    { 107, 0, 10, 2, BUTTON_STYLE, TEXT("button"), TEXT("����") },

    /* ����Ŀ¼ */
    { 1, 3, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("����:") },
    { 7, 3, 12, 2, EDIT_STYLE,   TEXT("edit"), TEXT("C:\\") },
    /* Զ��Ŀ¼ */
    { 20, 3, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("Զ��:") },
    { 26, 3, 12, 2, EDIT_STYLE,   TEXT("edit"), TEXT("/") },

    /* �ļ��б� */
    { 1, 5, 24,  20, LIST_STYLE,   TEXT("listbox"), NULL },
    { 25, 5, 24, 20, LIST_STYLE,   TEXT("listbox"), NULL },

    /* ��Ϣ */
    { 1, 25, 64, 20, EDIT_STYLE_EX, TEXT("edit"), TEXT("") }
};


/* ȫ�ֱ��� */
static HWND hWndWgt[TABLE_SIZE(FtpWgt)];
struct ftp_ctrl_s ftp_ctrl;

/* �������� */
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

    ftp_startup(); /* ��ʼ�� FTP Э�� */

    /* ��ʼ��ȫ���ַ��� */
    LoadString(hInstance, IDS_APP_TITLE, ftp_ctrl.szTitle, MAX_LOAD_STR);
    LoadString(hInstance, IDC_FTPCLNT, ftp_ctrl.szClass, MAX_LOAD_STR);

    FtpRegisterClass(hInstance);
    InitInstance(hInstance, nCmdShow); /* Ӧ�ó����ʼ�� */
    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FTPCLNT);

    /* ��Ϣѭ�� */
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ftp_cleanup(TRUE); /* �ͷ� FTP Э����Դ */
    return msg.wParam;
}

/**************************************************************************
 *
 * ��������: ע�� FTP ����Ĵ�����.
 *
 * ����˵��: [IN] hInstance, Ӧ�ó���ʵ�����.
 *
 * �� �� ֵ: �����ɹ�ʱ����ֵ������0, ��Ψһ��ʶ�˱�ע�����; ʧ��ʱ����0.
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
 * ��������: ��������ʾӦ�ó��򴰿�.
 *
 * ����˵��: [IN] hInstance, Ӧ�ó���ʵ�����.
 *           [IN] nCmdShow, ������ʾ��ʽ, ����������󻯡���С�������ص�.
 *
 * �� �� ֵ: �����ɹ�ʱ���� TRUE; ʧ��ʱ���� FALSE.
 *
 **************************************************************************/
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    ftp_ctrl.hWnd = CreateWindow(ftp_ctrl.szClass, ftp_ctrl.szTitle, 
                     WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 
                     CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ftp_ctrl.hInst = hInstance; /* ����Ӧ�ó���ʵ����� */
    ShowWindow(ftp_ctrl.hWnd, nCmdShow);
    UpdateWindow(ftp_ctrl.hWnd);

    return TRUE;
}

/**************************************************************************
 *
 * ��������: ���� WM_CREATE ��Ϣ.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ��Ϣ�� WPARAM ����.
 *           [IN] lParam, ��Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
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
 * ��������: �õ����ڿͻ�����С.
 *
 * ����˵��: [IN] hWnd, �����ھ��.
 *           [IN] hSubWnd, �Ӵ��ھ��.
 *           [OUT] rt, �Ӵ��ڵĴ�С.
 *
 * �� �� ֵ: void.
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
 * ��������: �����ڸı��С��Ϣ, �����Ӵ��ڵ�λ�ü���С.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] cx, ���ڿ��.
 *           [IN] cy, ���ڸ߶�.
 *
 * �� �� ֵ: void.
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

        /* Ŀ¼˵�� */
        FtpGetClientRect(hWnd, hWndWgt[iIndex], &rtWidget);
        
        iWidth = rtWidget.right - rtWidget.left;
        iHeight= rtWidget.bottom - rtWidget.top;
        rtWidget.left = cxOffset;
        MoveWindow(hWndWgt[iIndex], rtWidget.left, rtWidget.top, 
                   iWidth, iHeight, FALSE);

        iRight = rtWidget.left + iWidth;
        iIndex += 1;

        /* Ŀ¼ */
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
        
        /* �ļ��б� */
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
 * ��������: �û��������Ӱ�ť����.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
static void FtpOnCmdConn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    char cBuff[8], *szPath;
    unsigned short sPort;
    int iResult, wmEvent = HIWORD(wParam); 

    if (wmEvent != BN_CLICKED)
        return;

    /* ���� BN_CLICKED, �õ��û��������Ϣ */
    GetWindowText(hWndWgt[ID_EDIT_HOST], ftp_ctrl.cHost,FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_USER], ftp_ctrl.cUser, FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_PASS], ftp_ctrl.cPass, FTP_NAME_LEN);
    GetWindowText(hWndWgt[ID_EDIT_PORT], cBuff, 8);

    /* �õ��������ϵ�Ŀ¼ */
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

    /* ��������ѯ */
    iResult = ftp_do_event(EV_HOST, hWnd, ftp_ctrl.cHost);
    if (iResult == FTPE_SUCESS)
        FtpEnableConnWindow(FALSE);
}

/**************************************************************************
 *
 * ��������: �����ļ��б� LIST �ؼ�˫������.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ˫����Ϣ�� WPARAM ����.
 *           [IN] lParam, ˫����Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
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

    /* ���� LBN_DBLCLK, �õ��û��������Ϣ */
    i = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
    if (LB_ERR == i)
        return;

    SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szPath);
    
    hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        szPath[lstrlen (szPath) - 1] = '\0';
        
        /* ����Ŀ¼ʧ��, Ӧ���������� */
        if (!SetCurrentDirectory(szPath + 1))
        {
            szPath[3] = ':';
            szPath[4] = '\0';
            SetCurrentDirectory(szPath + 2);
        }

        /* �õ��µ�Ŀ¼, ������б�� */
        GetCurrentDirectory(MAX_PATH + 1, szPath);
        SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
        SendMessage(hWndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
    }
    else /* ѡ������ļ� */
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
 * ��������: Զ���ļ��б� LIST �ؼ�˫������.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ˫����Ϣ�� WPARAM ����.
 *           [IN] lParam, ˫����Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
static void FtpOnRemoteList(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i, wmEvent = HIWORD(wParam);
    TCHAR szPath[FTP_NAME_LEN];
    HWND hList = (HWND)lParam;

    if (wmEvent != LBN_DBLCLK)
        return;

    /* ���� LBN_DBLCLK, �õ��û�ѡ�����Ϣ */
    i = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (i == LB_ERR)
        return;

    i = SendMessage(hList, LB_GETTEXT, i, (LPARAM)szPath);
    
    strcpy(ftp_ctrl.cPath, szPath);
    ftp_do_event(EV_LDIR, NULL, NULL);
}

/**************************************************************************
 *
 * ��������: ���� FTP ����ĸ�������.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
static LRESULT FtpOnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    wmId    = LOWORD(wParam); 
    wmEvent = HIWORD(wParam); 

    switch (wmId)
    {
    case ID_BTN_CONN: /* ���� */
        FtpOnCmdConn(hWnd, wParam, lParam);
        break;

    case ID_BTN_DISC: /* ���� */
        ftp_cleanup(FALSE);
        FtpEnableConnWindow(TRUE);
        /* ���Զ���б�� */
        SendMessage(hWndWgt[ID_FILE_REMOTE], LB_RESETCONTENT, 0,0); 
        break;

    case ID_FILE_LOCAL: /* �������ļ��б����˫�� */
        FtpOnCmdList(hWnd, wParam, lParam);
        break;

    case ID_FILE_REMOTE: /* Զ���ļ��б����˫�� */
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
 * ��������: �յ�������ѯ��Ϣ�Ĵ���.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
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
        return; /* ������ѯ���� */
    }

    host_ent = (struct hostent *)ftp_ctrl.HostEnt;
    addr = *(unsigned long *)host_ent->h_addr; /* �����ֽ��� */
    iError = ftp_do_event(EV_CONN, hWnd, &addr); /* ��������������� */
}

/**************************************************************************
 *
 * ��������: socket �¼�����.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: �¼�������, �ɹ��� TRUE, ʧ���� FALSE.
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
    case FD_CONNECT: /* connect() �Ľ�� */
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
        iResult = ftp_store_file(wParam); /* �ϴ��ļ� */
        break;

    case FD_CLOSE:
        ftp_close_process(wParam);
        break;
    }
}

/**************************************************************************
 *
 * ��������: ���ڹ��̺���, ����Ӧ�ó���Ĵ�����Ϣ.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] message, Ҫ�������Ϣ.
 *           [IN] wParam, ��Ϣ�� WPARAM ����.
 *           [IN] lParam, ��Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: 0.
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
 * ��������: ���������ƶ�����.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] lParam, ��Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: ����Ĳ������ز�����, ���򷵻� 0.
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
 * ��������: LIST ���ڹ��̺���, ���� LIST ����Ϣ.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] message, Ҫ�������Ϣ.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: ������.
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

        /* ��¼��갴�µ� listbox */
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
 * ��������: ��Զ���ļ��б���������µ��ļ�.
 *
 * ����˵��: [IN] szFile, �ļ�����.
 *
 * �� �� ֵ: void.
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
 * ��������: �ڱ��ػ�Զ���ļ��б���������µ��ļ�.
 *
 * ����˵��: [IN] iLocal, �Ƿ��Ǳ����ļ�.
 *
 * �� �� ֵ: void.
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
 * ��������: �������ӺͶ�����ť��״̬.
 *
 * ����˵��: [IN] bEnable, �Ƿ���������.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void FtpEnableConnWindow(int bEnable)
{
    EnableWindow(hWndWgt[ID_BTN_CONN], bEnable);
    EnableWindow(hWndWgt[ID_BTN_DISC], !bEnable);
}

/**************************************************************************
 *
 * ��������: ����־��������ʾ��ϸ�Ľ�����Ϣ.
 *
 * ����˵��: [IN] szFormat, ��ӡ���ݵĸ�ʽ��Ϣ.
 *
 * �� �� ֵ: void.
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