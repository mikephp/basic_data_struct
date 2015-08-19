/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  Lookup.c
 *
 * PURPOSE :  主机名或IP地址解析程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32") /* WinSock 使用的库文件	*/

int main(int argc, char* argv[]) 
{ 
    char *name = NULL, **list_p;
    struct hostent *hostent_p;
    struct in_addr addr;
    WSADATA wsaData; 

    WSAStartup(MAKEWORD(2,0), &wsaData); /* 初始化 */

    if (argc == 2)
        name = argv[1]; /* 命令行第二个参数是要解析的主机名或地址 */

    if (name && isdigit(name[0])) /* name[0] 是数字, 认为是 IP 地址 */
    {
        addr.s_addr = inet_addr(name);
        if (addr.s_addr == INADDR_NONE) /* 地址无效 */
        {
            printf("[%s] is invalid address\n", name);
            return -1;
        } 
        /* 根据地址查询主机信息 */
        hostent_p = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
    }
    else
        hostent_p = gethostbyname(name); /* 根据主机名查询地址 */

    if (hostent_p == NULL)
    { 
        printf("fail to lookup, error : %d\n", WSAGetLastError());
        return -1;
    } 

    printf("official name: %s\n", hostent_p->h_name); /* 输出正式主机名 */

    /* 输出主机的所有别名 */
    if (*hostent_p->h_aliases)
        printf("alias:\n");

    for(list_p = hostent_p->h_aliases; *list_p != NULL; list_p++)
        printf("\t%s\n",*list_p);

    /* 根据地址类型，将地址打出来 */
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

