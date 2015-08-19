/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  CheckSum.c
 *
 * PURPOSE :  ����У��ͳ���.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <string.h>

#define DATA_MAX_LEN    14   /* ������ݳ��� */

struct data_sum
{
    char data[DATA_MAX_LEN]; /* ���� */
    unsigned short checksum; /* У��� */
};

/**************************************************************************
 *
 * ��������: ����У���.
 *
 * ����˵��: [IN] buf, ���ݻ�����;
 *           [IN] buf_len, buf ���ֽڳ���.
 *
 * �� �� ֵ: У���.
 *
 **************************************************************************/
unsigned short ip_checksum(unsigned short *buf, int buf_len)
{
    unsigned long checksum = 0;

    while (buf_len > 1)
    {
        checksum += *buf++;
        buf_len -= sizeof(unsigned short);
    }

    if (buf_len)
    {
        checksum += *(unsigned char *)buf;
    }

    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);

    return (unsigned short)(~checksum);
}

int main(int argc, char **argv)
{
    struct data_sum msg = { "Hello World!", 0 };
    int length = sizeof(msg);

    /* checksum = 0, ����У��� */
    msg.checksum = ip_checksum((unsigned short *)&msg, length);
    printf("Calculate check sum : 0x%x\n", msg.checksum);

    /* ���� checksum ֵ, ��֤У��� */
    msg.checksum = ip_checksum((unsigned short *)&msg, length);
    printf("Verify check sum : 0x%x\n", msg.checksum);

    return 0;
}