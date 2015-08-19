/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME: GetProto.c
 *
 * PURPOSE : Э����Ϣ��ѯ����.
 *  
 * AUTHOR  : �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32") /* WinSock ʹ�õĿ��ļ�	*/

void proto_print(struct protoent *proto_info, const char *proto_name, int proto)
{
	char **list_p;
	
	if (proto_info == NULL)
	{ 
		printf("__________________________________\n");
		
		if (proto_name)
			printf("can't get protocol infomation: %s\n", proto_name);
		else
			printf("can't get protocol infomation by number: %d\n", proto);
	}
	else
	{
		printf("__________________________________\n");
		printf("official name: %s\n", proto_info->p_name); /* ���Э���� */ 
		
		/* �����������б��� */
		for(list_p = proto_info->p_aliases; *list_p != NULL; list_p++) 
			printf("alias: %s\n",*list_p);
				
		printf("protocol: %d\n", proto_info->p_proto);   /* ���Э���� */
	}
}

int main(int argc, char* argv[]) 
{ 
	const char *proto_name[] = { "tcp", "ipv6", "esp" };
	int i, name_cnt = sizeof(proto_name) / sizeof(proto_name[0]);
	int proto_number[] = { 1, 8, 51 };
	int proto_cnt = sizeof(proto_number) / sizeof(proto_number[0]);
	struct protoent *proto_info;
	WSADATA wsaData;
	
	WSAStartup(MAKEWORD(2,0), &wsaData); /* ��ʼ�� */
	
	printf("[getprotobyname] get protocol by name\n");
	
	for (i = 0; i < name_cnt; i++)
	{
		proto_info = getprotobyname(proto_name[i]);		
		proto_print(proto_info, proto_name[i], 0);
	}		
	
	printf("\n[getprotobynumber] get protocol by number\n");
	
	for (i = 0; i < proto_cnt; i++)
	{
		proto_info = getprotobynumber(proto_number[i]);	
		proto_print(proto_info, NULL, proto_number[i]);
	}
	
	WSACleanup();
	return 0;
}

