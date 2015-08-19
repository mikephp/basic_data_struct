/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FingerSrv.c
 *
 * PURPOSE :  Finger ����������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "resource.h"

#pragma comment(lib, "ws2_32.lib") /* WinSockʹ�õĿ⺯�� */

#define WM_SOCKET_NOTIFY   (WM_USER + 11) /* ���� socket ��Ϣ */
#define FINGER_DEF_PORT    79 /* �����Ķ˿� */
#define ID_EDIT_LOG        1  /* ��־�ؼ��ı�ʶ */

/* ����ؼ��ķ�� */
#define EDIT_STYLE  (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |\
                     ES_MULTILINE | WS_HSCROLL | WS_VSCROLL)

#define FINGER_LINE_END  "\r\n" /* �н����� */
#define FINGER_MAX_BUF    1024  /* ���Ľ��ջ����� */
#define FINGER_MAX_SOC       8  /* �����Խ��ܵĿͻ��� */
#define TABLE_SIZE(a) (sizeof a / sizeof a[0])

/* �����û���Ϣ */
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

/* ����ȫ�ֱ��� */
static HWND hWndLog;       /* �����־��Ϣ�Ĵ��ھ�� */
static SOCKET hLstnSoc;    /* ���� socket ��� */
static SOCKET hClntSoc[FINGER_MAX_SOC]; /* �ͻ��˵� socket ��� */

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

    WSAStartup(WINSOCK_VERSION, &wsaData); /* ��ʼ�� */
    memset(hClntSoc,INVALID_SOCKET,FINGER_MAX_SOC*sizeof(SOCKET));

    /* ע�ᴰ���� */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* ���Ǵ��ڹ��� */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = (LPCSTR)IDR_FINGER_SRV; /* �˵� */
    wndclass.lpszClassName = szClassName; /* �������� */

    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("TRequires Windows NT!"), szClassName, 0);
        return 0;
    }

    hWnd = CreateWindow(szClassName,          /* ��ע���������ͬ */
                        TEXT("Finger Server"),/* ���ڱ��� */
                        WS_OVERLAPPEDWINDOW,  /* ���ڷ�� */
                        CW_USEDEFAULT,        /* ��ʼ x ���� */
                        CW_USEDEFAULT,        /* ��ʼ y ���� */
                        CW_USEDEFAULT,        /* ��ʼ��� */
                        CW_USEDEFAULT,        /* ��ʼ�߶� */
                        NULL,                 /* �����ھ�� */
                        NULL,                 /* �˵���� */
                        hInstance,            /* ����ʵ����� */
                        NULL);                /* ������� */

    ShowWindow(hWnd, iCmdShow); /* ��ʾ���� */
    UpdateWindow(hWnd);         /* ���´��� */

    /* ��Ϣѭ�� */
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* �ر� socket */
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
 * ��������: �������� socket.
 *
 * ����˵��: [IN] hWnd,  ���ھ��.
 *           [IN] port, �����Ķ˿�.
 *
 * �� �� ֵ: �ɹ����ش����� socket, ʧ�ܷ��� INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerListenSoc(HWND hWnd, unsigned short port)
{
    struct sockaddr_in soc_addr; /* socket ��ַ�ṹ */
    SOCKET lstn_soc; /* ���� socket ��� */
    int result;

    /* �������� socket */
    lstn_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = WSAAsyncSelect(lstn_soc, hWnd, WM_SOCKET_NOTIFY, 
                            FD_ACCEPT | FD_READ | FD_CLOSE);

    /* ��ϵͳ�������ַ */
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_port = htons(port);
    soc_addr.sin_addr.s_addr = INADDR_ANY;

    /* �� socket */
    result = bind(lstn_soc, (struct sockaddr *)&soc_addr, sizeof(soc_addr));
    if (result == SOCKET_ERROR)
    {
        closesocket(lstn_soc);
        return INVALID_SOCKET;
    }

    result = listen(lstn_soc, SOMAXCONN); /* �������Կͻ��˵����� */
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
 * ��������: ���ܿͻ�������.
 *
 * ����˵��: [IN] lstn_soc,  ���� socket ���.
 *
 * �� �� ֵ: �ɹ����ؽ��ܵ� socket ���, ʧ�ܷ��� INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerOnAccept(SOCKET lstn_soc)
{
    struct sockaddr_in soc_addr; /* socket ��ַ�ṹ */
    int i, addr_len = sizeof(soc_addr);	/* ��ַ���� */
    SOCKET data_soc;

    /* �����µ����� */
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
 * ��������: ����ͻ�������.
 *
 * ����˵��: [IN] clnt_soc,  ��ͻ������ӵ� socket ���.
 *
 * �� �� ֵ: �ɹ����� TRUE, ʧ�ܷ��� FALSE.
 *
 **************************************************************************/
static int FingerOnRead(SOCKET clnt_soc)
{
    int i, j, result, buflen = FINGER_MAX_BUF - 1;
    int iFind = 0, iCount = TABLE_SIZE(FingerUser);
    char cBuf[FINGER_MAX_BUF], cSendBuf[FINGER_MAX_BUF], *pEnd;
    struct UserInfo *pUser;

    /* ���ҿͻ��˶�Ӧ�� socket ��� */
    for (i = 0; i < FINGER_MAX_SOC; i++)
    {
        if (hClntSoc[i] == clnt_soc)
            break;
    }

    if (i == FINGER_MAX_SOC)
        return FALSE;
    
    result = recv(clnt_soc, cBuf, buflen, 0);  /* �������� */
    if (result <= 0)
    {
        closesocket(clnt_soc);
        hClntSoc[i] = INVALID_SOCKET;
        return FALSE;
    }

    cBuf[result] = 0;
    LogPrintf(TEXT("recv >: %s\r\n"), cBuf);

    /* �����û�����β�� "\r\n" */
    pEnd = strstr(cBuf, FINGER_LINE_END);
    if ((pEnd != NULL) && (pEnd != cBuf))
        *pEnd = 0; /*��β�� "\r\n" ���� 0, ���ڲ��� */

    for (j = 0; j < iCount; j++) /* �����û���Ϣ */
    {
        pUser = &FingerUser[j];
        
        if (strcmp(cBuf, FINGER_LINE_END) == 0) /* �����û� */
            buflen = sprintf(cSendBuf, "%s\r\n", pUser->szUser);
        else if (strcmp(cBuf, pUser->szUser) == 0) /* �ض��û� */
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
 * ��������: ����ر��¼�.
 *
 * ����˵��: [IN] clnt_soc,  ��ͻ������ӵ� socket ���.
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
static void FingerOnClose(SOCKET clnt_soc)
{
    int i;

    /* ���ҿͻ��˶�Ӧ�� socket ��� */
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
 * ��������: socket �¼�����.
 *
 * ����˵��: [IN] wParam, socket ���.
 *           [IN] lParam, �� 16 λ�������¼�, �� 16 λ�Ǵ�����.
 *
 * �� �� ֵ: void.
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
 * ��������: ����־��������ʾ��ϸ�Ľ�����Ϣ.
 *
 * ����˵��: [IN] szFormat, ��ӡ���ݵĸ�ʽ��Ϣ.
 *
 * �� �� ֵ: void.
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