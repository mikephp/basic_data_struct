/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  MsgSndSrv.h
 *
 * PURPOSE :  ���� Message Send Protocol 2 �ĳ��������ݽṹ.
 *  
 * AUTHOR  :  �Ż���
 *
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#ifndef MSG_SND_SRV_
#define MSG_SND_SRV_

#define MSG_SEND_VERS       'B' /* �汾�� */
#define MSG_SEND_PORT       18  /* ��Ϣ����Э��ȱʡ�˿�*/
#define MSG_BUF_SIZE       512  /* �������Ĵ�С */

/* ��������ֵ */
#define MSG_SUCCESS         0
#define MSG_VERSION_ERR    -1
#define MSG_FIELD_ERR      -2

/* ������Ϣ�ֶε�ö��ֵ */
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