/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  GetServ.c
 *
 * PURPOSE :  服务查询程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32") /* WinSock 使用的库文件  */

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
        printf("official name: %s\n", serv_info->s_name); /* 输出正式服务名 */
        
        if (*serv_info->s_aliases)
            printf("alias:\n"); /* 输出服务的所有别名 */
        
        for(list_p = serv_info->s_aliases; *list_p != NULL; list_p++)
            printf("\t%s\n",*list_p);
        
        printf("port: %d\n", ntohs(serv_info->s_port)); /* 输出端口   */
        printf("protocol: %s\n", serv_info->s_proto);   /* 输出协议名 */
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

    WSAStartup(MAKEWORD(2,0), &wsaData); /* 初始化 */

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

