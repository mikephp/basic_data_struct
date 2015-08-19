/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  AsyncHost.c
 *
 * PURPOSE :  �첽������ѯ����.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */

/* ����������ѯ��Ϣ */
#define WM_GETHOST_NOTIFY  (WM_USER + 1)

/* ����ؼ��ķ�� */
#define STATIC_STYLE  (WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT)
#define EDIT_STYLE    (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT)
#define BUTTON_STYLE  (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)

/* �ؼ��ı�ʶ, �ǿؼ��������е��±� */
#define ID_EDIT_HOST     1 /* ���� */
#define ID_BTN_QUERY     2 /* ��ѯ��ť�� ID */

#define HOST_BUF_LEN  1024 /* ���������� */
#define HOST_POS_X      60 /* �����Ϣ���Ͻ� x λ�� */
#define HOST_POS_Y      80 /* �����Ϣ���Ͻ� y λ�� */

#define TABLE_SIZE(a)   (sizeof a / sizeof a[0])

/* �ؼ������Խṹ */
struct Widget
{
    int iLeft;      /* ���Ͻǵ� x ���� */
    int iTop;       /* ���Ͻǵ� y ���� */
    int iWidth;     /* ��� */
    int iHeigh;     /* �߶� */
    int iStyle;     /* �ؼ��ķ�� */
    TCHAR *szType;  /* �ؼ�����: static, button, edit. */
    TCHAR *szTitle; /* �ؼ�����ʾ������ */
};

struct QueryHost
{
    HANDLE hAsyncHost; /* ������ѯ��� */
    char cHostEnt[MAXGETHOSTSTRUCT]; /* ������ѯ������ */
    TCHAR szInfoBuf[HOST_BUF_LEN];   /* �����Ϣ�Ļ����� */
};

/* ����������ѯ����ʹ�õĿؼ� */
static struct Widget HostWgt[] = 
{
    /* ������ǩ */
    { 1, 2, 6,  2, STATIC_STYLE, TEXT("static"), TEXT("����:") },
    /* �������� */
    { 7, 2, 40, 2, EDIT_STYLE,   TEXT("edit"), TEXT("") },
    /* ��ѯ��ť */
    { 50, 2, 12, 2, BUTTON_STYLE, TEXT("button"), TEXT("��ѯ") }
};

/* ȫ�ֱ��� */
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

    /* ע�ᴰ���� */
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;  /* ���Ǵ��ڹ��� */
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL; /* û�в˵� */
    wndclass.lpszClassName = szClassName; /* �������� */
    
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Requires Windows NT!"), szClassName, 0);
        return 0;
    }

    hWnd = CreateWindow(szClassName,          /* ��ע���������ͬ */
                        TEXT("Query Host"),   /* ���ڱ��� */
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

    /* �����ʽ������ */
    result = wsprintf(pOut, TEXT("official name: %s\r\n"), ent_p->h_name);
    pOut += result;

    /* ������������б��� */
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
    
    /* ���ݵ�ַ����, ����ַ��ӡ���� */	
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
        /* ��ʼ�� */
        SetWindowText(hWndWgt[ID_EDIT_HOST], TEXT("www.google.com"));
        return 0;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        if (gQureyHost.hAsyncHost)
        {
            GetClientRect(hWnd, &rect);

            rect.left = HOST_POS_X;
            rect.top = HOST_POS_Y;
            /* ��ʾ��Ϣ */
            DrawText(hDC, gQureyHost.szInfoBuf, -1, &rect, DT_LEFT);
        }

        EndPaint(hWnd, &ps);
        return 0;

    case WM_COMMAND:
        wmId    = LOWORD(wParam); 
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case ID_BTN_QUERY: /* ��ѯ */
            if (wmEvent != BN_CLICKED)
                break;

            /* ���� BN_CLICKED, �õ��û��������Ϣ */
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
            return 0; /* �������� */
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