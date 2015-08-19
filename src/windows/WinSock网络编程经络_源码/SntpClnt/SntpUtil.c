/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  SntpUtil.c
 *
 * PURPOSE :  得到系统当前时间.
 *  
 * AUTHOR  :  张会勇
 * 
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <Windows.h>
#include <time.h>
#include "Sntp.h"


/**************************************************************************
 *
 * 函数功能: 得到系统当前时间.
 *
 * 参数说明: [OUT] current, SNTP 时间.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
 *
 **************************************************************************/
int sntp_get_sys_time(time_fp *current)
{
    double temp_d;
    time_t second;
    long nano_sec;

    union {
        FILETIME ft;
        ULONGLONG ull;
    } ft_now;

    if (current == NULL)
        return -1;

    GetSystemTimeAsFileTime(&ft_now.ft);

    /* 把 hecto-nano second 转换成 second 和 nanosecond */
    ft_now.ull -= SNTP_TIME_1900;
    second = (time_t)(ft_now.ull / SNTP_HECTO_NANOSECONDS);
    nano_sec = (long)((ft_now.ull % SNTP_HECTO_NANOSECONDS) * 100);

    /* 转换成 NTP Timestamp 格式 */
    current->fp_si = second;
    temp_d = nano_sec * 1e-9; /* 转换成 second */

    if (temp_d >= 1.0) 
    {
        temp_d -= 1.0;
        current->fp_si++;
    } 
    else if (temp_d < 0) 
    {
        temp_d += 1.0;
        current->fp_si--;
    }

    current->fp_uf = (unsigned int)(temp_d * SNTP_TS_FRAC);

    return 0;
}
