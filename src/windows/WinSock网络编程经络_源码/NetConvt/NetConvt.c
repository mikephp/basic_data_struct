/*************************************************************************
 * 
 * Copyright (c) 2002-2005 by Zhang Huiyong All Rights Reserved
 *
 * FILENAME:  NetConvt.c
 *
 * PURPOSE :  ����ת������ʵ�ֳ���.
 *  
 * AUTHOR  :  �Ż���
 * 
 * BOOK    :  <<WinSock�����̾���>>
 * 
 **************************************************************************/

#include <stdio.h>
#include <winsock2.h>

#define INADDR_INVALD   0xffffffff  /* ��Ч��ַ */

static unsigned long range[] = { 0xffffffff, 0xffffff, 0xffff, 0xff };

unsigned short net_htons(unsigned short shost)
{
    unsigned char *p = (unsigned char *)&shost;

    return (unsigned short)(p[0] << 8 | p[1]);
}

unsigned short net_ntohs(unsigned short snet)
{
    unsigned char *p = (unsigned char *)&snet;
    
    return (unsigned short)(p[0] << 8 | p[1]);
}

unsigned long net_htonl(unsigned long lhost)
{
    unsigned char *p = (unsigned char *)&lhost;
    
    return (unsigned long)(p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24));
}

unsigned long net_ntohl(unsigned long lnet)
{
    unsigned char *p = (unsigned char *)&lnet;
    
    return (unsigned long)(p[3] | p[2] << 8 | p[1] << 16 | p[0] << 24);
}

/**************************************************************************
 *
 * ��������: �������ֽ����ַת��Ϊ���ʮ���Ƶ�IP��ַ.
 *
 * ����˵��: [IN] addr,  �����ֽ����ַ.
 *
 * �� �� ֵ: ���ص��ʮ���Ƶ�IP��ַ.
 *
 **************************************************************************/
char *net_ntoa(struct in_addr addr)
{
    static char buf[16]; /* ת������󳤶�Ϊ15 */
    unsigned char *p = (unsigned char *)&addr;

    sprintf(buf, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

    return buf;
}

/**************************************************************************
 *
 * ��������: �ѵ��ʮ���Ƶ�IP��ַת��Ϊ�����ֽ����ַ.
 *
 * ����˵��: [IN] cp,  ���ʮ���Ƶ�IP��ַ.
 *
 * �� �� ֵ: �ɹ����������ֽ����ַ, ʧ�ܷ��� INADDR_INVALD.
 *
 **************************************************************************/
unsigned long net_addr(const char *cp)
{
    unsigned char addr[4], *p = addr, c = *cp;
    unsigned long value;
    int i, n, base, digit;

    while (1)
    {
        base = 10, value = 0, digit = 0;
        if (!isdigit(c))
            return INADDR_INVALD;
        
        /* 1. ����ǰ׺: 0, 8����; 0x, 16����; ����, 10���� */
        if (c == '0')
        {
            base = 8, c = *++cp;
            if (c == 'x' || c == 'X')
                base = 16, c = *++cp;
        }
        /* 2. ������ַ��һ����, ��������, ֱ�� '.'��������ַ� */
        while (c = *cp++)
        {
            if (isdigit(c))
            {
                if (base == 8 && c >= '8')
                    return INADDR_INVALD;
                value = value * base + (c - '0');
            }
            else if (base == 16 && isxdigit(c))
                value = (value << 4) | (c + 10 - (islower(c) ? 'a' : 'A'));
            else
                break;

            digit = 1; /* ����˵��������һ������ */
        }
        /* 3. ���� '.' �Ĵ��� */
        if (c == '.')
        {
            if (!digit || p >= addr + 3 || value > 0xff)
                return INADDR_INVALD;
            *p++ = (unsigned char)value;
            c = *cp;
        }
        else
            break;
    }

    n = p - addr; /* �Ѿ����浽 addr �е����ָ��� */
    /* �������, ���Ƿ�������, ���β���ֽڼ�ֵ�ķ�Χ */
    if (!digit || (value > range[n]) ||
        (c != '\0' && !isspace(c)))
        return INADDR_INVALD;

    /* �ѵ�ַ�����һ���ֱ��浽 addr �� */
    for (i = 3; i >= n; i--, value >>= 8)
        addr[i] = (unsigned char)(value & 0xff);

    value = *(unsigned long *)addr;

    return value;
}

int main(int argc, char *argv[])
{
    unsigned short host_s = 0x1234, net_s;
    unsigned long  host_l = 0x12345678, net_l;
    char *addr_dec = "192.168.10.26", *p;
    struct in_addr addr;

    net_s = net_htons(host_s);
    net_l = net_htonl(host_l);

    printf("net byte order is : net_s = 0x%x, net_l = 0x%x\n", net_s, net_l);

    addr.s_addr = net_addr(addr_dec);
    p = net_ntoa(addr);

    printf("net addr is 0x%x, string addr is %s\n", addr.s_addr, p);
    
    return 0;
}