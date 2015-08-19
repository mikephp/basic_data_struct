/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  CheckSum.c
 *
 * PURPOSE :  计算校验和程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <string.h>

#define DATA_MAX_LEN    14   /* 最大数据长度 */

struct data_sum
{
    char data[DATA_MAX_LEN]; /* 数据 */
    unsigned short checksum; /* 校验和 */
};

/**************************************************************************
 *
 * 函数功能: 计算校验和.
 *
 * 参数说明: [IN] buf, 数据缓冲区;
 *           [IN] buf_len, buf 的字节长度.
 *
 * 返 回 值: 校验和.
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

    /* checksum = 0, 计算校验和 */
    msg.checksum = ip_checksum((unsigned short *)&msg, length);
    printf("Calculate check sum : 0x%x\n", msg.checksum);

    /* 包含 checksum 值, 验证校验和 */
    msg.checksum = ip_checksum((unsigned short *)&msg, length);
    printf("Verify check sum : 0x%x\n", msg.checksum);

    return 0;
}