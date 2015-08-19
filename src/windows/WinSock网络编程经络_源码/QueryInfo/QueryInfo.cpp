/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  QueryInfo.cpp
 *
 * PURPOSE :  ��ѯ����, ����, Э����Ϣ.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "wsock32") /* WinSock ʹ�õĿ⺯�� */

/* ��ѯ��Ϣ */
#define WM_QUERYINFO_MSG    (WM_USER + 5)

/* ������ʹ�õĳ��� */
#define STR_MAX_LEN         64
#define QUERY_MAX_LINE      4
#define QUERY_SHOW_X        20
#define QUERY_SHOW_Y        20

/* ����Ҫ��ѯ����Ϣ */
#define QUERY_HOST_NAME     "www.google.com"
#define QUERY_SERV_NAME     "telnet"
#define QUERY_PROTO_NAME    "tcp"

/* ��ѯ������ */
enum QueryType
{
    QUERY_HOST,
    QUERY_SERV,
    QUERY_PROTO
};

typedef struct QueryInfo
{
    HINSTANCE hInstance;                      /* ����ʵ����� */
    HANDLE    hQuery;                         /* ��ѯ�������ص��첽��� */
    char      chQueryInfo[MAXGETHOSTSTRUCT];  /* �����ѯ��������� */
    char      chShowBuffer[MAXGETHOSTSTRUCT]; /* ��ʾ��ѯ��������� */
    char      *pShowLine[QUERY_MAX_LINE];     /* ��ʾ���л����� */
    int       nLine;                          /* ��ʾ������ */
    int       nType;      /* ��ǰ�Ĳ�ѯ����, ֵΪ enum QueryType */
} QUERYINFO;


/*************************************************************************
 *  ȫ�ֱ���
 *************************************************************************/
QUERYINFO g_QueryInfo;

/* ������ǰ������ */
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

    WSAStartup(MAKEWORD(2,0), &wsaData); /* ��ʼ�� */

    g_QueryInfo.hQuery = 0;

    /* װ���������ַ��� */
    LoadString(hInstance, IDC_QUERYINFO, szWindowClass, STR_MAX_LEN);
    QueryRegisterClass(hInstance, szWindowClass);

    /* Ӧ�ó����ʼ�� */
    if (!QueryInitInstance(hInstance, szWindowClass, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_QUERYINFO);

    /* ����Ϣѭ�� */
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    WSACleanup();   /* �ͷ� WinSock ��Դ */
    return msg.wParam;
}

/*************************************************************************
 * FUNCTION : ע�ᴰ����.
 *
 * PARAMETER: 
 *  hInstance, Ӧ�ó���ʵ�����.
 *	szWindowClass, ������.
 *
 * RETURN   : ע��ɹ�����������������͵ı�ʶ�ţ������ñ�ʶ�Ž��д������ڣ�
 *            ���Ҵ��ں�ע�����ڵ�; ʧ�ܷ��ص�ֵ�� 0.
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
 * FUNCTION : ����ʵ�����������������.
 *
 * PARAMETER: 
 *  hInstance, Ӧ�ó���ʵ�����.
 *	szWindowClass, ������.
 *  nCmdShow, �Ƿ���ʾ.
 *
 * RETURN   : �ɹ����� TRUE; ʧ�ܷ��� FALSE.
 *************************************************************************/
BOOL QueryInitInstance(HINSTANCE hInstance, TCHAR *szWindowClass, int nCmdShow)
{
   HWND hWnd;
   TCHAR szTitle[STR_MAX_LEN]; /* �������ı� */

   LoadString(hInstance, IDS_APP_TITLE, szTitle, STR_MAX_LEN);
   
   g_QueryInfo.hInstance = hInstance; /* ����ʵ����� */

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
 * FUNCTION : �����û���ѡ�������Ϣ��ѯ.
 *
 * PARAMETER: 
 *  hWnd, ���ھ��.
 *	nType, ��ѯ����.
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
 * FUNCTION : ��ӡ��ѯ��������Ϣ.
 *
 * PARAMETER: 
 *  pQuery, ��ѯ���.
 *	pResult, ��һ����ʽ�����ѯ����Ļ�����, Ϊ���ڴ�������ʾ.
 *
 * RETURN   : ������ʾ������.
 *************************************************************************/
int QueryHostPrint(char *pQuery, char *pResult)
{
    struct hostent	*hostent_p;
    struct in_addr addr;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    hostent_p = (struct hostent *)pQuery;
    length = sprintf(dest, "official name: %s", hostent_p->h_name); /* �����ʽ������ */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);

    /* ������������б��� */
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

    dest += 1; /* ��β�� 0 */

    /* ���ݵ�ַ���ͣ�����ַ����� */	
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

    g_QueryInfo.nLine = i;	/* ��¼���� */

    return i;
}

/*************************************************************************
 * FUNCTION : ��ӡ��ѯ�ķ�����Ϣ.
 *
 * PARAMETER: 
 *  pQuery, ��ѯ���.
 *	pResult, ��һ����ʽ�����ѯ����Ļ�����, Ϊ���ڴ�������ʾ.
 *
 * RETURN   : ������ʾ������.
 *************************************************************************/
int QueryServPrint(char *pQuery, char *pResult)
{
    struct servent	*servent_p;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    servent_p = (struct servent *)pQuery;
    length = sprintf(dest, "official name: %s", servent_p->s_name); /* �����ʽ������ */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1); /* 1 �ǽ�β�� 0 */

    /* ������������б��� */
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

    dest += 1; /* ��β�� 0 */

    length = sprintf(dest, "port: %d", ntohs(servent_p->s_port)); /* ����˿�   */
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);	/* 1 �ǽ�β�� 0 */

    length = sprintf(dest, "protocol: %s", servent_p->s_proto);   /* ���Э���� */
    g_QueryInfo.pShowLine[i++] = dest;

    g_QueryInfo.nLine = i; /* ��¼���� */

    return i;
}

/*************************************************************************
 * FUNCTION : ��ӡ��ѯ��Э����Ϣ.
 *
 * PARAMETER: 
 *  pQuery, ��ѯ���.
 *	pResult, ��һ����ʽ�����ѯ����Ļ�����, Ϊ���ڴ�������ʾ.
 *
 * RETURN   : ������ʾ������.
 *************************************************************************/
int QueryProtoPrint(char *pQuery, char *pResult)
{
    struct protoent	*protoent_p;
    char **list_p, *dest = pResult;
    int offset = 0, length = 0, i = 0;

    protoent_p = (struct protoent *)pQuery;
    /* �����ʽ������ */
    length = sprintf(dest, "official name: %s", protoent_p->p_name); 
    g_QueryInfo.pShowLine[i++] = dest;
    dest += (length + 1);	/* 1 �ǽ�β�� 0 */

    /* ������������б��� */
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

    dest += 1; /* ��β�� 0 */

    /* ���Э���� */
    length = sprintf(dest, "protocol number: %d", protoent_p->p_proto);   
    g_QueryInfo.pShowLine[i++] = dest;

    g_QueryInfo.nLine = i;  /* ��¼���� */

    return i;
}

/*************************************************************************
 * FUNCTION : �ڴ�������ʾ��ѯ��Ϣ.
 *
 * PARAMETER: 
 *  hWnd, ���ھ��.
 *	hDC, ��ʾ�豸���.
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
 * FUNCTION : �����û�ѡ��Ĳ˵�����.
 *
 * PARAMETER: 
 *  hWnd, Ӧ�ó��򴰿ھ��.
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

    /* ����˵����� */
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
 * FUNCTION : ��������, ����, Э���ѯ��Ϣ.
 *
 * PARAMETER: 
 *  hWnd, Ӧ�ó��򴰿ھ��.
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
 * FUNCTION : ���ڴ������, ����ô��ڵ���Ϣ.
 *
 * PARAMETER: 
 *  hWnd, Ӧ�ó��򴰿ھ��.
 *	message, ��Ϣ��.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : �ɹ����� TRUE; ʧ�ܷ��� FALSE.
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
 * FUNCTION : About �Ի��������.
 *
 * PARAMETER: 
 *  hWnd, �Ի��򴰿ھ��.
 *	message, ��Ϣ��.
 *  wParam, .
 *  lParam, 
 *
 * RETURN   : �ɹ����� TRUE; ʧ�ܷ��� FALSE.
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
