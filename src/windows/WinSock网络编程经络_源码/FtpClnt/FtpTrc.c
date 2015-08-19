/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpTrc.c
 *
 * PURPOSE :  把日志输出到文件.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <stdarg.h>

#define FLOG_BUF_SIZE    1024  /* FTP log 缓冲区的大小 */

static FILE *ftp_log = NULL;

FILE *ftp_log_open(char *log_file)
{
    ftp_log = fopen(log_file, "w+");

    return ftp_log;
}

void ftp_log_close(void)
{
    if (ftp_log)
    {
        fclose(ftp_log);
        ftp_log = NULL;
    }
}


char *ftp_log_print(const char *fmt, ...)
{
    static char buf[FLOG_BUF_SIZE + 1] = { 0 };
    int result;
    va_list arg;

    va_start(arg, fmt);

    result = _vsnprintf(buf, FLOG_BUF_SIZE, fmt, arg);

    if (result > 0)
        result = fwrite(buf, sizeof(char), result, ftp_log);
    
    va_end(arg);

    return buf;
}

int ftp_log_write(const char *buf, int buf_len)
{
    int result = 0;

    if (buf_len > 0)
        result = fwrite(buf, sizeof(char), buf_len, ftp_log);

    return result;
}