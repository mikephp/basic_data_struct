/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  SntpUtil.c
 *
 * PURPOSE :  �õ�ϵͳ��ǰʱ��.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <Windows.h>
#include <time.h>
#include "Sntp.h"


/**************************************************************************
 *
 * ��������: �õ�ϵͳ��ǰʱ��.
 *
 * ����˵��: [OUT] current, SNTP ʱ��.
 *
 * �� �� ֵ: �ɹ����� 0, ʧ�ܷ��� -1.
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

    /* �� hecto-nano second ת���� second �� nanosecond */
    ft_now.ull -= SNTP_TIME_1900;
    second = (time_t)(ft_now.ull / SNTP_HECTO_NANOSECONDS);
    nano_sec = (long)((ft_now.ull % SNTP_HECTO_NANOSECONDS) * 100);

    /* ת���� NTP Timestamp ��ʽ */
    current->fp_si = second;
    temp_d = nano_sec * 1e-9; /* ת���� second */

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
