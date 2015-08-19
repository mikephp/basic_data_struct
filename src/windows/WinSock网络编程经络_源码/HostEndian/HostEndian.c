/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  HostEndian.c
 *
 * PURPOSE :  判断主机字节序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>

union endian_u
{
    unsigned short sval;
    unsigned char cval[2];    
};

int main(int argc, char *argv[])
{
    char *info = "unknow endian";
    union endian_u t = { 0x1234 };

    if (t.cval[0] == 0x12 && t.cval[1] == 0x34)
        info = "big-endian";
    else if (t.cval[0] == 0x34 && t.cval[1] == 0x12)
        info = "little-endian";

    printf("host is %s.\n", info);

    return 0;
}