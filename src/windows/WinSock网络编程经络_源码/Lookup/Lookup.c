/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  Lookup.c
 *
 * PURPOSE :  ��������IP��ַ��������.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32") /* WinSock ʹ�õĿ��ļ�	*/

int main(int argc, char* argv[]) 
{ 
    char *name = NULL, **list_p;
    struct hostent *hostent_p;
    struct in_addr addr;
    WSADATA wsaData; 

    WSAStartup(MAKEWORD(2,0), &wsaData); /* ��ʼ�� */

    if (argc == 2)
        name = argv[1]; /* �����еڶ���������Ҫ���������������ַ */

    if (name && isdigit(name[0])) /* name[0] ������, ��Ϊ�� IP ��ַ */
    {
        addr.s_addr = inet_addr(name);
        if (addr.s_addr == INADDR_NONE) /* ��ַ��Ч */
        {
            printf("[%s] is invalid address\n", name);
            return -1;
        } 
        /* ���ݵ�ַ��ѯ������Ϣ */
        hostent_p = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
    }
    else
        hostent_p = gethostbyname(name); /* ������������ѯ��ַ */

    if (hostent_p == NULL)
    { 
        printf("fail to lookup, error : %d\n", WSAGetLastError());
        return -1;
    } 

    printf("official name: %s\n", hostent_p->h_name); /* �����ʽ������ */

    /* ������������б��� */
    if (*hostent_p->h_aliases)
        printf("alias:\n");

    for(list_p = hostent_p->h_aliases; *list_p != NULL; list_p++)
        printf("\t%s\n",*list_p);

    /* ���ݵ�ַ���ͣ�����ַ����� */
    switch(hostent_p->h_addrtype) 
    {
    case AF_INET:
        printf("address type: AF_INET\n");
        break; 

    case AF_INET6:
        printf("address type: AF_INET6\n");
        break;

    default: 
        printf("address type is unknown\n");
        return -1; 
    }

    printf("address:\n");
    for(list_p = hostent_p->h_addr_list; *list_p != NULL; list_p++)
    {
        memcpy(&addr.S_un.S_addr, *list_p, hostent_p->h_length);
        printf("\t%s\n",inet_ntoa(addr)); 
    }

    WSACleanup();
    return 0;
}

