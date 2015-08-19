/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  GetServ.c
 *
 * PURPOSE :  �����ѯ����.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32") /* WinSock ʹ�õĿ��ļ�  */

void serv_print(struct servent *serv_info, const char *serv_name, short port)
{
    char **list_p;

    if (serv_info == NULL)
    { 
        printf("__________________________________\n");

        if (serv_name)
            printf("can't get service: %s\n", serv_name);
        else
            printf("can't get service on port: %d\n", port);
    }
    else
    {
        printf("__________________________________\n");
        printf("official name: %s\n", serv_info->s_name); /* �����ʽ������ */
        
        if (*serv_info->s_aliases)
            printf("alias:\n"); /* �����������б��� */
        
        for(list_p = serv_info->s_aliases; *list_p != NULL; list_p++)
            printf("\t%s\n",*list_p);
        
        printf("port: %d\n", ntohs(serv_info->s_port)); /* ����˿�   */
        printf("protocol: %s\n", serv_info->s_proto);   /* ���Э���� */
    }
}

int main(int argc, char* argv[]) 
{ 
    const char *proto = "tcp";
    const char *serv_name[] = { "domain", "http", "invalid" };
    int i, serv_cnt = sizeof(serv_name) / sizeof(serv_name[0]);
    short port_num[] = { 7, 88 };
    int port_cnt = sizeof(port_num) / sizeof(port_num[0]);
    struct servent *serv_info;
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2,0), &wsaData); /* ��ʼ�� */

    printf("[getservbyname] get service by name\n");

    for (i = 0; i < serv_cnt; i++)
    {
        serv_info = getservbyname(serv_name[i], proto);
        serv_print(serv_info, serv_name[i], 0);
    }       
    
    printf("\n[getservbyport] get service by port\n");

    for (i = 0; i < port_cnt; i++)
    {
        serv_info = getservbyport(htons(port_num[i]), proto);
        serv_print(serv_info, NULL, port_num[i]);
    }

    WSACleanup();
    return 0;
}

