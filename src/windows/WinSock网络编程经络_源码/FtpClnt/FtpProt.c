/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  FtpProt.c
 *
 * PURPOSE :  FTP 协议客户端实现程序.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#include <winsock2.h>
#include <stdio.h>
#include "FtpClnt.h"
#include "FtpProt.h"

/* 函数的前向声明 */
static int ftp_status_gethost(enum ftp_event ev, void *arg1, void *arg2);
static int ftp_cmd_port(SOCKET ctrl_soc, SOCKET lstn_soc);
static int ftp_recv_data(SOCKET soc);


/**************************************************************************
 *
 * 函数功能: 创建侦听 socket.
 *
 * 参数说明: [IN] port, 侦听的端口.
 *
 * 返 回 值: 成功返回创建的 socket, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
static SOCKET ftp_listen_socket(unsigned short port)
{
    struct sockaddr_in local; /* socket 地址结构 */
    SOCKET lstn_soc; /* 侦听 socket 句柄 */
    int result;

    lstn_soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = WSAAsyncSelect(lstn_soc, ftp_ctrl.hWnd, WM_SOCKET_NOTIFY, 
                FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE);

    /* 由系统来分配地址 */
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;

    /* 绑定 socket */
    result = bind(lstn_soc, (struct sockaddr *)&local, sizeof(local));
    if (result == SOCKET_ERROR)
    {
        closesocket(lstn_soc);
        return INVALID_SOCKET;
    }

    result = listen(lstn_soc, SOMAXCONN); /* 侦听来自服务器的连接 */
    return lstn_soc;
}

/**************************************************************************
 *
 * 函数功能: 接受从对方过来的连接请求.
 *
 * 参数说明: [IN] soc, 侦听 socket 句柄.
 *
 * 返 回 值: 成功返回接受的 socket, 失败返回 INVALID_SOCKET.
 *
 **************************************************************************/
SOCKET ftp_accept_conn(SOCKET soc)
{
    struct sockaddr_in addr; /* socket 地址结构 */
    int len = sizeof(addr); /* 地址长度 */

    if (soc != ftp_ctrl.hLstnSoc)
        return INVALID_SOCKET;

    /* 接受新的连接 */
    ftp_ctrl.hDataSoc = accept(soc, (struct sockaddr *)&addr, &len);
    if (ftp_ctrl.hDataSoc == INVALID_SOCKET)
    {
        FtpPrintf(TEXT("accept error: %i.\r\n"), WSAGetLastError());
        return INVALID_SOCKET;
    }

    return ftp_ctrl.hDataSoc;
}

/**************************************************************************
 *
 * 函数功能: 生成 FTP 命令, 并发送给服务器.
 *
 * 参数说明: [IN] soc, 客户 socket 句柄.
 *           [IN] cmd, 要发送的命令.
 *           [IN] param, 命令所需要的参数.
 *
 * 返 回 值: 成功返回发送的数据长度, 失败返回 WinSock 错误码.
 *
 **************************************************************************/
static int ftp_send_cmd(SOCKET soc, char *cmd, char *param)
{
    char cmd_buf[FTP_BUF_LEN];
    int result;

    if (param)
        result = sprintf(cmd_buf, "%s %s\r\n", cmd, param);
    else
        result = sprintf(cmd_buf, "%s\r\n", cmd);

    result = send(soc, cmd_buf, result, 0);
    ftp_ctrl.iRecvLen = 0;
    FtpPrintf("> %s", cmd_buf);

    return result;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端的 FTPS_CLOSED 状态处理函数.
 *
 * 参数说明: [IN] ev, 事件.
 *           [IN] arg1, 参数 1.
 *           [IN] arg2, 参数 2.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回值小于 0.
 *
 **************************************************************************/
static int ftp_status_closed(enum ftp_event ev, void *arg1, void *arg2)
{
    HANDLE async_host;
    unsigned long addr;
    HWND hWnd = (HWND)arg1;
    char *host = (char *)arg2;

    if (ftp_ctrl.iStatus != FTPS_CLOSED && ev != EV_HOST)
        return FTPE_INVALID;

    /* 确定主机是否是 IP 地址 */
    addr = inet_addr(host);
    if (addr != INADDR_NONE)
    {
        ftp_ctrl.iStatus = FTPS_GETHOST;
        return ftp_status_gethost(EV_CONN, hWnd, &addr);
    }

    /* 主机为域名 */
    async_host = WSAAsyncGetHostByName(hWnd, WM_GETHOST_NOTIFY, 
                    host, ftp_ctrl.HostEnt, MAXGETHOSTSTRUCT);
    if (async_host == 0)
    {
        FtpPrintf(TEXT("gethost error: %i.\r\n"), WSAGetLastError());
        return FTPE_ERROR;
    }

    ftp_ctrl.hAsyncHost = async_host;
    ftp_ctrl.iStatus = FTPS_GETHOST;

    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端的 FTPS_GETHOST 状态处理函数.
 *
 * 参数说明: [IN] ev, 事件.
 *           [IN] arg1, 参数 1.
 *           [IN] arg2, 参数 2.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回值小于 0.
 *
 **************************************************************************/
static int ftp_status_gethost(enum ftp_event ev, void *arg1, void *arg2)
{
    int result;
    SOCKET sock;
    struct sockaddr_in dest;

    HWND hWnd = (HWND)arg1;
    unsigned long addr = *(unsigned long *)arg2;

    if (ftp_ctrl.iStatus != FTPS_GETHOST && ev != EV_CONN)
        return FTPE_INVALID;

    /* 创建 TCP 套接口 */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        FtpPrintf(TEXT("socket error: %i.\r\n"), WSAGetLastError());
        return SOCKET_ERROR;
    }

    ftp_ctrl.hCtrlSoc = sock;
    result = WSAAsyncSelect(sock, hWnd, WM_SOCKET_NOTIFY, 
                FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
    
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = addr;
    dest.sin_port = ftp_ctrl.sPort;

    result = connect(sock, (struct sockaddr *)&dest, sizeof(dest));
    if (result == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            FtpPrintf(TEXT ("connect error: %i.\r\n"), result);
            closesocket(sock);
            return SOCKET_ERROR;
        }
    }

    FTP_EVENT_ENTER(EV_CONN, TRUE);
    ftp_ctrl.iStatus = FTPS_CONNECT;

    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端的 FTPS_CONNECT 状态处理函数.
 *
 * 参数说明: [IN] ev, 事件.
 *           [IN] arg1, 参数 1.
 *           [IN] arg2, 参数 2.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回值小于 0.
 *
 **************************************************************************/
static int ftp_status_connect(enum ftp_event ev, void *arg1, void *arg2)
{
    int result;

    if (ftp_ctrl.iStatus != FTPS_CONNECT && ev != EV_USER)
        return FTPE_INVALID;

    result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "USER", ftp_ctrl.cUser);
    ftp_ctrl.iStatus = FTPS_USER;

    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端的 FTPS_USER 状态处理函数.
 *
 * 参数说明: [IN] ev, 事件.
 *           [IN] arg1, 参数 1.
 *           [IN] arg2, 参数 2.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回值小于 0.
 *
 **************************************************************************/
static int ftp_status_user(enum ftp_event ev, void *arg1, void *arg2)
{
    int result;

    if ((ftp_ctrl.iStatus != FTPS_USER) && (ev != EV_PASS))
        return FTPE_INVALID;

    result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "PASS", ftp_ctrl.cPass);

    if (strlen(ftp_ctrl.cPath) > 1)
        FTP_EVENT_ENTER(EV_CWD, FALSE);

    FTP_EVENT_ENTER(EV_PORT, FALSE);
    FTP_EVENT_ENTER(EV_LIST, FALSE);
    ftp_ctrl.iStatus = FTPS_PASSWD;

    return FTPE_SUCESS;
}

static int ftp_status_password(enum ftp_event ev, void *arg1, void *arg2)
{
    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端的 FTPS_LOGIN 状态处理函数.
 *
 * 参数说明: [IN] ev, 事件.
 *           [IN] arg1, 参数 1.
 *           [IN] arg2, 参数 2.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回值小于 0.
 *
 **************************************************************************/
static int ftp_status_login(enum ftp_event ev, void *arg1, void *arg2)
{
    int result, open_file = 0;
    char *file_name = (char *)arg1;
    DWORD dwAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD dwCreate = CREATE_ALWAYS;
    HANDLE hFile;

    switch (ev)
    {
    case EV_LDIR: /* 改变目录并列出文件 */
        FTP_EVENT_ENTER(EV_CWD, FALSE);
        FTP_EVENT_ENTER(EV_PORT, FALSE);
        FTP_EVENT_ENTER(EV_LIST, FALSE);
    case EV_CWD:
        result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "CWD", ftp_ctrl.cPath);
        break;

    case EV_PORT:
        if (ftp_ctrl.hLstnSoc == INVALID_SOCKET)
            ftp_ctrl.hLstnSoc = ftp_listen_socket(0);

        result = ftp_cmd_port(ftp_ctrl.hCtrlSoc, ftp_ctrl.hLstnSoc);
        break;

    case EV_LIST:
        result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "NLST", NULL);
        open_file = 1;
        file_name = TEXT("list.cmd");
        ftp_ctrl.DataOp = EV_LIST;
        break;

    case EV_RETR:
        file_name = ftp_ctrl.cSelFile;
        result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "RETR", file_name);
        open_file = 1;
        ftp_ctrl.DataOp = EV_RETR;
        break;

    case EV_STOR:
        file_name = ftp_ctrl.cSelFile;
        result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "STOR", file_name);
        open_file = 1;
        dwCreate = OPEN_EXISTING;
        ftp_ctrl.DataOp = EV_STOR;
        break;

    case EV_QUIT:
        result = ftp_send_cmd(ftp_ctrl.hCtrlSoc, "QUIT", NULL);
        break;
    }

    if ((result != SOCKET_ERROR) && open_file)
    {   /* 创建文件, 保存数据 */
        hFile = CreateFile(file_name, dwAccess, FILE_SHARE_READ, 
                           NULL, dwCreate, 0, NULL);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            FtpPrintf(TEXT("open file error.\r\n"));
            return FTPE_ERROR;
        }

        ftp_ctrl.hFile = hFile;
        ftp_ctrl.iStatus = FTPS_SENDCMD;
    }

    return result;
}

static int ftp_status_sendcmd(enum ftp_event ev, void *arg1, void *arg2)
{
    FtpPrintf(TEXT("The previous command is not done.\r\n"));
    return FTPE_ERROR;
}

/* FTP 客户端状态机每个状态的处理函数数组 */
static int (*ftp_fsm[])(enum ftp_event ev, void *arg1, void *arg2) = 
{
    ftp_status_closed,
    ftp_status_gethost,
    ftp_status_connect,
    ftp_status_user,
    ftp_status_password,
    ftp_status_login,
    ftp_status_sendcmd
};

int ftp_do_event(enum ftp_event ev, void *arg1, void *arg2)
{
    return ftp_fsm[ftp_ctrl.iStatus](ev, arg1, arg2);
}

/**************************************************************************
 *
 * 函数功能: 解析服务器的应答.
 *
 * 参数说明: [IN] data, 接收到的数据.
 *
 * 返 回 值: 成功返回应答的长度, 失败返回 0.
 *
 **************************************************************************/
int ftp_parse_reply(char *data)
{
    int i, reply_len = 0, sep_len = 2;
    char *line_end = NULL, *last_line = data;
    char reply_code[] = "123 ";

    if (*(data + 3) == FTP_MULTILINE_FLAG)
    {
        /* 多行应答 */
        for (i = 0; i < 3; i++)
            reply_code[i] = data[i];
        
        /* 先找最后一行的应答码 */
        last_line = strstr(data, reply_code);
    }
    /* else 单行应答 */

    if (last_line)
    {
        line_end = strstr(last_line, FTP_LINE_END);
        if (line_end == NULL) /* 找不到 "\r\n", 再找 '\r' */
        {
            line_end = strchr(last_line, '\r');
            sep_len = 1; /* 结束符长度为 1 */
        }
    }

    /* 应答的长度 */
    reply_len = line_end ? (line_end - data + sep_len) : 0;

    return reply_len;
}

/**************************************************************************
 *
 * 函数功能: 得到服务器的应答码.
 *
 * 参数说明: [IN] data, 接收到的数据.
 *
 * 返 回 值: 返回应答码.
 *
 **************************************************************************/
int ftp_get_reply_code(char *data)
{
    int code;
    char byte;

    byte = *(data + 3); /* 保存字符 */
    *(data + 3) = '\0'; /* 终止应答码 */
    code = atoi(data); /* 把应答码转换成数值 */
    *(data + 3) = byte; /* 恢复字符 */

    return code;
}

/**************************************************************************
 *
 * 函数功能: 根据服务器的应答码做不同的处理.
 *
 * 参数说明: [IN] code, 应答码.
 *
 * 返 回 值: 返回应答码.
 *
 **************************************************************************/
int ftp_code_process(int code)
{
    enum ftp_event ev = ftp_ctrl.Event[0];

    code /= 100;
    switch (code)
    {
    case FTP_CODE_100:
        break;

    case FTP_CODE_200:
        if (ev == EV_CONN) /* 连接成功, 发送 USER */
            FTP_EVENT_ENTER(EV_USER, FALSE);
        else if (ev == EV_USER)
        {
            if (strlen(ftp_ctrl.cPath) > 1)
                FTP_EVENT_ENTER(EV_CWD, FALSE);

            FTP_EVENT_ENTER(EV_PORT, FALSE);
            FTP_EVENT_ENTER(EV_LIST, FALSE);
            ftp_ctrl.iStatus = FTPS_LOGIN;
        }
        else if (ev == EV_PASS)
            ftp_ctrl.iStatus = FTPS_LOGIN;
        break;

    case FTP_CODE_300:
        if (ev == EV_USER)
            FTP_EVENT_ENTER(EV_PASS, FALSE);
        break;

    case FTP_CODE_400: /* throughout */
    case FTP_CODE_500:
        /* 出错时关闭文件 */
        if (ftp_ctrl.iStatus == FTPS_SENDCMD)
        {			
            ftp_ctrl.iStatus = FTPS_LOGIN;
            if (ftp_ctrl.hFile != INVALID_HANDLE_VALUE)
            {
                CloseHandle(ftp_ctrl.hFile);
                ftp_ctrl.hFile = INVALID_HANDLE_VALUE;
            }
        }
        ftp_ctrl.sEvtCnt = 0; /* 清空相关事件 */
        break;

    default:
        break;
    }

    if ((code != FTP_CODE_100) && (ftp_ctrl.sEvtCnt > 1))
    {
        memmove(ftp_ctrl.Event, &ftp_ctrl.Event[1], 
            (--ftp_ctrl.sEvtCnt) * sizeof(enum ftp_event));
    }
    else
        ftp_ctrl.sEvtCnt = 0;

    return code;
}

/**************************************************************************
 *
 * 函数功能: 接收服务器的应答, 并根据接收的数据做不同的处理.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回接收数据的长度, 失败返回错误码.
 *
 **************************************************************************/
int ftp_recv_reply(SOCKET soc)
{
    int recv_len, code;
    int offset = ftp_ctrl.iRecvLen;
    int left = FTP_BUF_LEN;

    do 
    {
        left = FTP_BUF_LEN - offset;
        recv_len = recv(soc, &(ftp_ctrl.cRecvBuf[offset]), left, 0);
        if (recv_len == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
                return SOCKET_ERROR;
        }
        else
        {
            ftp_ctrl.cRecvBuf[offset + recv_len] = 0;
            FtpPrintf("< %s", &(ftp_ctrl.cRecvBuf[offset]));
            offset += recv_len;
        }
    } while (recv_len > 0  && offset < FTP_BUF_LEN);

    /* 接收了所有数据 */
    ftp_ctrl.iRecvLen = offset;
    ftp_ctrl.cRecvBuf[offset] = '\0';

    do 
    {
        recv_len = ftp_parse_reply(ftp_ctrl.cRecvBuf);
        if (recv_len <= 0) /* 没有接收完全 */
            return 0;

        code = ftp_get_reply_code(ftp_ctrl.cRecvBuf);
        if (code == 0)
            return 0;

        code = ftp_code_process(code);

        /* 把后面的数据移到前面 */
        left = ftp_ctrl.iRecvLen - recv_len;
        ftp_ctrl.iRecvLen = left;
        if (left > 0)
        {            
            memmove(ftp_ctrl.cRecvBuf, &ftp_ctrl.cRecvBuf[recv_len], left);
            ftp_ctrl.cRecvBuf[left] = 0;
        }
        
        if (((code == FTP_CODE_200) || (code == FTP_CODE_300)) &&
            (ftp_ctrl.sEvtCnt > 0))
        {        
            ftp_do_event(ftp_ctrl.Event[0], NULL, NULL);
        }
        
    } while (left > 0);

    return offset;
}

/**************************************************************************
 *
 * 函数功能: 在数据连接上接收从服务器过来的数据.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回接收数据的长度, 失败返回 0.
 *
 **************************************************************************/
static int ftp_recv_data(SOCKET soc)
{
    int recv_len, total = 0;
    char cRecvBuf[FTP_BUF_LEN];
    DWORD dwWrite;
    
    do /* 接收数据并保存到文件中 */
    {
        recv_len = recv(soc, cRecvBuf, FTP_BUF_LEN, 0);
        if (recv_len > 0)
        {
            WriteFile(ftp_ctrl.hFile, cRecvBuf, recv_len, &dwWrite, NULL);

            total += recv_len;
            if (total < 512) /* 小于 512 才显示 */
            {
                cRecvBuf[recv_len] = 0;
                FtpPrintf("< %s", cRecvBuf);
            }
        }
    } while (recv_len > 0);

    return total;
}

/**************************************************************************
 *
 * 函数功能: 在数据通道上向服务器发送数据.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回FTPE_SUCESS, 失败返回 FTPE_ERROR.
 *
 **************************************************************************/
int ftp_store_file(SOCKET soc)
{
    HANDLE hFile = ftp_ctrl.hFile;
    DWORD dwSizeLow = FTP_BUF_LEN, dwRead = 0;
    int result;
    char cSendBuf[FTP_BUF_LEN];

    /* 只处理上传文件 */
    if ((soc != ftp_ctrl.hDataSoc) || (ftp_ctrl.DataOp != EV_STOR) || 
        (ftp_ctrl.hFile == INVALID_HANDLE_VALUE))
        return FTPE_ERROR;

    /* 读文件中的数据, 发送给服务器 */
    do 
    {
        result = ReadFile(hFile, cSendBuf, dwSizeLow, &dwRead, NULL);
        if (result > 0 && dwRead > 0)
            result = send(soc, cSendBuf, dwRead, 0);
    } while (dwRead > 0);

    /* 数据发完, 关闭 socket 及文件 */
    closesocket(soc);
    ftp_ctrl.hDataSoc = INVALID_SOCKET;
    CloseHandle(ftp_ctrl.hFile);
    ftp_ctrl.hFile = INVALID_HANDLE_VALUE;

    FtpListAddFile(FALSE); /* 增加到远端文件列表中 */
    ftp_ctrl.iStatus = FTPS_LOGIN; /* 发送完文件, 恢复状态 */

    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: 接收从服务器过来的数据.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回接收数据的长度, 失败返回错误码.
 *
 **************************************************************************/
int ftp_recv_process(SOCKET soc)
{
    int result = 0;

    if (soc == ftp_ctrl.hCtrlSoc)
        result = ftp_recv_reply(soc);
    else if (soc == ftp_ctrl.hDataSoc)
        result = ftp_recv_data(soc);

    return result;
}

/**************************************************************************
 *
 * 函数功能: 前面已经发送了 LIST 命令, 读取服务器过来的数据, 增加到远端文件
 *           列表中.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回 FTPE_ERROR.
 *
 **************************************************************************/
static int ftp_data_list(SOCKET soc)
{
    HANDLE hFile = ftp_ctrl.hFile;
    DWORD dwSizeLow = 0, dwPtr, dwRead;
    int result = FTPE_ERROR;
    char *buf;

    /* 得到文件大小 */
    dwPtr = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    dwSizeLow = GetFileSize(hFile, NULL);
    
    if ((dwSizeLow != INVALID_FILE_SIZE) &&
        (ftp_ctrl.DataOp == EV_LIST))
    {
        /* 读取文件数据, 并增加到 ListBox 中 */
        buf = malloc(dwSizeLow + 1);
        ReadFile(hFile, buf, dwSizeLow, &dwRead, NULL);
        buf[dwRead] = 0;
        FtpRemoteListAdd(buf);
        free(buf);

        ftp_ctrl.sEvtCnt = 0;
        result = FTPE_SUCESS;
    }

    /* 关闭文件 */
    CloseHandle(hFile);
    ftp_ctrl.hFile = INVALID_HANDLE_VALUE;
    ftp_ctrl.hDataSoc = INVALID_SOCKET;

    return result;
}

/**************************************************************************
 *
 * 函数功能: 读取服务器过来的数据, 并根据前面发送的命令做对应的处理.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回 FTPE_ERROR.
 *
 **************************************************************************/
static int ftp_data_process(SOCKET soc)
{
    int result = FTPE_SUCESS;

    switch (ftp_ctrl.DataOp)
    {
    case EV_LIST: /* 列出目录 */
        result = ftp_data_list(soc);
        break;

    case EV_RETR: /* 获取文件 */
        FtpListAddFile(TRUE);
        break;

    case EV_STOR: /* 上传文件 */
        FtpListAddFile(FALSE);
        break;

    default:
        break;
    }

    return result;
}

/**************************************************************************
 *
 * 函数功能: 接收到对方关闭通知时的处理函数.
 *
 * 参数说明: [IN] soc, socket 句柄.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void ftp_close_process(SOCKET soc)
{
    if (soc == ftp_ctrl.hCtrlSoc)
    {
        ftp_cleanup(FALSE);
        FtpEnableConnWindow(TRUE);
        FtpPrintf("\r\n[Control close]\r\n");
    }
    else if (soc == ftp_ctrl.hDataSoc)
    {
        ftp_data_process(soc);
        closesocket(ftp_ctrl.hLstnSoc);
        ftp_ctrl.hLstnSoc = INVALID_SOCKET;
        FtpPrintf("\r\n[Data close]\r\n");

        if (ftp_ctrl.hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(ftp_ctrl.hFile);
            ftp_ctrl.hFile = INVALID_HANDLE_VALUE;
        }
        if (ftp_ctrl.iStatus == FTPS_SENDCMD)
            ftp_ctrl.iStatus = FTPS_LOGIN;

        closesocket(soc);
    }
}

/**************************************************************************
 *
 * 函数功能: 得到本地地址和端口, 生成 PORT 命令, 并发送给服务器.
 *
 * 参数说明: [IN] ctrl_soc, 控制连接的 socket 句柄.
 *           [IN] lstn_soc, 侦听 socket 句柄.
 *
 * 返 回 值: 成功返回发送的长度, 失败返回错误码.
 *
 **************************************************************************/
static int ftp_cmd_port(SOCKET ctrl_soc, SOCKET lstn_soc)
{
    struct sockaddr_in soc_addr;  /* 套接字地址结构 */
    int result, addr_len = sizeof(soc_addr); 
    unsigned short local_port;	/* 本地侦听端口 */

    /* 得到侦听套接口地址 */
    result = getsockname(lstn_soc, (struct sockaddr *)&soc_addr, &addr_len);
    if (result == SOCKET_ERROR)
    {
        FtpPrintf(TEXT("sockname error: %i.\r\n"), WSAGetLastError());
        return SOCKET_ERROR;
    }

    /* 保存本地侦听端口 */
    local_port = soc_addr.sin_port;

    /* 得到控制 socket 的本地地址 */
    result = getsockname(ctrl_soc, (struct sockaddr *)&soc_addr, &addr_len);
    if (result == SOCKET_ERROR)
    {
        FtpPrintf(TEXT("ctrl sockname error: %i.\r\n"), WSAGetLastError());
        return SOCKET_ERROR;
    }

    /* 根据地址和端口生成 PORT 命令, 都是网络字节序 */
    result = sprintf(ftp_ctrl.cCmdBuf, "%d,%d,%d,%d,%d,%d", 
                    soc_addr.sin_addr.S_un.S_un_b.s_b1, 
                    soc_addr.sin_addr.S_un.S_un_b.s_b2,
                    soc_addr.sin_addr.S_un.S_un_b.s_b3,
                    soc_addr.sin_addr.S_un.S_un_b.s_b4, 
                    local_port & 0xFF, local_port >> 8);

    result = ftp_send_cmd(ctrl_soc, "PORT", ftp_ctrl.cCmdBuf);
    return result;
}

/**************************************************************************
 *
 * 函数功能: 根据文件操作命令, 向服务器发送命令.
 *
 * 参数说明: [IN] operation, 文件操作: FTP_OP_RETR 或 FTP_OP_STOR.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回错误码.
 *
 **************************************************************************/
int ftp_file_process(int operation)
{
    int result;
    enum ftp_event ev;

    switch (operation)
    {
    case FTP_OP_RETR:
        ev = EV_RETR;
        break;

    case FTP_OP_STOR:
        ev = EV_STOR;
        break;

    default:
        return FTPE_ERROR;
    }

    FTP_EVENT_ENTER(EV_PORT, FALSE);
    FTP_EVENT_ENTER(ev, FALSE);
    result = ftp_do_event(EV_PORT, ftp_ctrl.cSelFile, NULL);
    return result;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端初始化函数.
 *
 * 参数说明: void.
 *
 * 返 回 值: 成功返回 FTPE_SUCESS, 失败返回 FTPE_ERROR.
 *
 **************************************************************************/
int ftp_startup(void)
{
    WSADATA wsa_data;
    
    if (ftp_ctrl.iInit)
        return FTPE_SUCESS;
    
    WSAStartup(WINSOCK_VERSION, &wsa_data);

    memset(&ftp_ctrl, 0, sizeof(ftp_ctrl));
    ftp_ctrl.iInit = TRUE;
    ftp_ctrl.hCtrlSoc = INVALID_SOCKET;
    ftp_ctrl.hDataSoc = INVALID_SOCKET;
    ftp_ctrl.hLstnSoc = INVALID_SOCKET;
    ftp_ctrl.hFile = INVALID_HANDLE_VALUE;

    ftp_log_open("ftp_trace.txt");
    return FTPE_SUCESS;
}

/**************************************************************************
 *
 * 函数功能: FTP 客户端终止函数, 关闭对应的句柄, 释放资源.
 *
 * 参数说明: [IN] end, 是否完全结束.
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void ftp_cleanup(int end)
{
    if (!ftp_ctrl.iInit)
        return;

    /* 关闭 socket 及文件句柄 */
    if (ftp_ctrl.hCtrlSoc != INVALID_SOCKET)
    {
        if (ftp_ctrl.iStatus >= FTPS_USER)
            ftp_send_cmd(ftp_ctrl.hCtrlSoc, "QUIT", NULL);

        closesocket(ftp_ctrl.hCtrlSoc);
        ftp_ctrl.hCtrlSoc = INVALID_SOCKET;
    }

    if (ftp_ctrl.hDataSoc != INVALID_SOCKET)
    {
        closesocket(ftp_ctrl.hDataSoc);
        ftp_ctrl.hDataSoc = INVALID_SOCKET;
    }

    if (ftp_ctrl.hLstnSoc != INVALID_SOCKET)
    {
        closesocket(ftp_ctrl.hLstnSoc);
        ftp_ctrl.hLstnSoc = INVALID_SOCKET;
    }

    if (ftp_ctrl.hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(ftp_ctrl.hFile);
        ftp_ctrl.hFile = INVALID_HANDLE_VALUE;
    }

    ftp_ctrl.iStatus = FTPS_CLOSED;
    ftp_ctrl.sEvtCnt = 0;

    if (end)
    {
        ftp_ctrl.iInit = FALSE;
        ftp_log_close();
        WSACleanup();
    }
}
