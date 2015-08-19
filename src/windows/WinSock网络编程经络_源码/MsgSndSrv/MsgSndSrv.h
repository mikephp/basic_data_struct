/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndSrv.h
 *
 * PURPOSE :  定义 Message Send Protocol 2 的常量及数据结构.
 *  
 * AUTHOR  :  张会勇
 *
 * BOOK    :  <<WinSock网络编程经络>>
 * 
 **************************************************************************/

#ifndef MSG_SND_SRV_
#define MSG_SND_SRV_

#define MSG_SEND_VERS       'B' /* 版本号 */
#define MSG_SEND_PORT       18  /* 消息发送协议缺省端口*/
#define MSG_BUF_SIZE       512  /* 缓冲区的大小 */

/* 函数返回值 */
#define MSG_SUCCESS         0
#define MSG_VERSION_ERR    -1
#define MSG_FIELD_ERR      -2

/* 定义消息字段的枚举值 */
enum msp_field
{
    MSP_VERSION,
    MSP_RECIP,
    MSP_RECIP_TERM,
    MSP_MESSAGE,
    MSP_SENDER,
    MSP_SEND_TERM,
    MSP_COOKIE,
    MSP_SIGNAT,
    MSP_TOTAL
};

struct field_name
{
    enum msp_field field;
    char *name;
};

#endif /* MSG_SND_SRV_ */