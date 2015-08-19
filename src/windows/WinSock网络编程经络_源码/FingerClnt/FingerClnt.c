/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FingerClnt.c
 *
 * PURPOSE :  Finger �ͻ��˳���.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */


#define WM_GETHOST_NOTIFY  (WM_USER + 1)  /* ����������ѯ��Ϣ */
#define WM_SOCKET_NOTIFY   (WM_USER + 11) /* ���� socket ��Ϣ */

#define FINGER_DEF_PORT     79 /* �����Ķ˿� */
#define FINGER_NAME_LEN    256 /* һ�����ֻ��������� */
#define FINGER_MAX_BUF    1024 /* ���Ľ��ջ����� */

/* ����ؼ��ķ�� */
#define STATIC_STYLE   (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define BUTTON_STYLE   (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)
#define EDIT_STYLE     (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define EDIT_STYLE_EXT (EDIT_STYLE | ES_MULTILINE | ES_READONLY | \
                        WS_HSCROLL | WS_VSCROLL)

/* �ؼ��ı�ʶ, �ǿؼ��������е��±� */
#define ID_EDIT_USER    1  /* �û� */
#define ID_EDIT_HOST    3  /* ���� */
#define ID_BTN_FINGER   4  /* ��ѯ��ť�� ID */
#define ID_EDIT_LOG     5  /* ��־�ؼ��ı�ʶ */

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

struct Finger
{
    HWND   hWnd;        /* ���ھ�� */
    HANDLE hAsyncHost;  /* ������ѯ��� */
    SOCKET hSoc;        /* socket ���  */
    char cHostEnt[MAXGETHOSTSTRUCT]; /* ������ѯ������ */
    char szUser[FINGER_NAME_LEN];    /* �û��� */
    char szHost[FINGER_NAME_LEN];    /* ���� */
};

/* ���� Finger ����ʹ�õĿؼ� */
static struct Widget FgrWgt[] = 
{
    /* �û��� */
    { 1, 1, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("�û�:") },
    { 7, 1, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("Alice") },
    /* ���� */
    { 33, 1, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("����:") },
    { 38, 1, 24, 2, EDIT_STYLE,   TEXT("edit"), TEXT("127.0.0.1") },

    /* Finger ��ť */
    { 64, 1, 12, 2, BUTTON_STYLE, TEXT("button"), TEXT("Finger") },
    
    /* ��Ϣ */
    { 1, 4, 64, 20, EDIT_STYLE_EXT, TEXT("edit"), TEXT("") }
};

/* ����ȫ�ֱ��� */
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
        
    WSAStartup(WINSOCK_VERSION, &wsaData); /* ��ʼ�� */
    
    /* ע�ᴰ���� */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* ���Ǵ��ڹ��� */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL; /* �˵� */
    wndclass.lpszClassName = szClassName; /* �������� */
    
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Requires Windows NT!"), szClassName, 0);
        return 0;
    }
    
    gFingerCtrl.hWnd = CreateWindow(szClassName,/* ��ע��������ͬ */
                        TEXT("Finger Client"),/* ���ڱ��� */
                        WS_OVERLAPPEDWINDOW,  /* ���ڷ�� */
                        CW_USEDEFAULT,        /* ��ʼ x ���� */
                        CW_USEDEFAULT,        /* ��ʼ y ���� */
                        CW_USEDEFAULT,        /* ��ʼ��� */
                        CW_USEDEFAULT,        /* ��ʼ�߶� */
                        NULL,                 /* �����ھ�� */
                        NULL,                 /* �˵���� */
                        hInstance,            /* ����ʵ����� */
                        NULL);                /* ������� */
    
    ShowWindow(gFingerCtrl.hWnd, iCmdShow); /* ��ʾ���� */
    UpdateWindow(gFingerCtrl.hWnd);         /* ���´��� */    
    
    /* ��Ϣѭ�� */
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
 * ��������: ���� WM_CREATE ��Ϣ.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ��Ϣ�� WPARAM ����.
 *           [IN] lParam, ��Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: void.
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

    /* �����ؼ� */
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
 * ��������: ���� Finger ����Ĳ�ѯ����.
 *
 * ����˵��: [IN] hWnd, ���ھ��.
 *           [IN] wParam, ������Ϣ�� WPARAM ����.
 *           [IN] lParam, ������Ϣ�� LPARAM ����.
 *
 * �� �� ֵ: ����������� TRUE, ���򷵻� FALSE.
 *
 **************************************************************************/
static BOOL FingerOnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);

    /* ���� BN_CLICKED, �õ��û��������Ϣ */
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
 * ��������: �����ͻ��� socket, ���������������.
 *
 * ����˵��: [IN] hWnd,  ���ھ��.
 *
 * �� �� ֵ: �ɹ����ش����� socket, ʧ�ܷ��� INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET FingerQuery(HWND hWnd)
{
    struct sockaddr_in soc_addr; /* socket ��ַ�ṹ */
    SOCKET soc; /* Finger �� socket ��� */
    int result;
    unsigned long addr;
    struct hostent *host_ent;

    host_ent = (struct hostent *)gFingerCtrl.cHostEnt;
    addr = *(unsigned long *)host_ent->h_addr; /* �����ֽ��� */
    
    soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    
    result = WSAAsyncSelect(soc, hWnd, WM_SOCKET_NOTIFY, 
                FD_CONNECT | FD_READ | FD_CLOSE);

    /* ���ӵĵ�ַ�Ͷ˿� */
    soc_addr.sin_family = AF_INET;
    soc_addr.sin_port = htons(FINGER_DEF_PORT);
    soc_addr.sin_addr.s_addr = addr;

    /* ��������������� */
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
 * ��������: ���ӳɹ�, ����������Ͳ�ѯ.
 *
 * ����˵��: [IN] clnt_soc,  ��ͻ������ӵ� socket ���.
 *
 * �� �� ֵ: �ɹ����ط��͵ĳ���, ʧ�ܷ��ش�����.
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
 * ��������: ���� socket �ϵĶ��¼�֪ͨ.
 *
 * ����˵��: [IN] clnt_soc,  socket ���.
 *
 * �� �� ֵ: �ɹ����ؽ��յ����ݳ���, ʧ�ܷ��ش�����.
 *
 **************************************************************************/
static int FingerOnRead(SOCKET clnt_soc)
{
    int result, buflen = FINGER_MAX_BUF - 1;
    char cBuf[FINGER_MAX_BUF];

    /* �������� */
    result = recv(clnt_soc, cBuf, buflen, 0);
    if (result <= 0)
    {
        closesocket(clnt_soc); /* ��ü� gFingerCtrl.hSoc = INVALID_SOCKET; */
        return result;
    }

    cBuf[result] = 0;
    LogPrintf(TEXT("%s\r\n"), cBuf);

    return result;
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
            return 0; /* �������� */
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