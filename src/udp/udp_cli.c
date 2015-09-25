/*
 * File:   main.c
 * Author: tianshuai
 *
 * Created on 2011��11��29��, ����10:34
 *
 * ��Ҫʵ�֣�����20���ı���Ϣ��Ȼ���ٷ���һ����ֹ��Ϣ
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int port=6789;
int main(int argc, char** argv) {
    int socket_descriptor; //�׽ӿ�������
    int iter=0;
    char buf[80];
    struct sockaddr_in address;//��������ͨ�ŵĵ�ַ

    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");//���ﲻһ��
    address.sin_port=htons(port);

    //����һ�� UDP socket

    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM ���ݱ��׽��֣�UDPЭ�飩

    for(iter=0;iter<=20;iter++)
    {
         /*
         * sprintf(s, "%8d%8d", 123, 4567); //������" 123 4567"
         * ����ʽ���� �ַ�����ŵ�s����
         */
        sprintf(buf,"data packet with ID %d\n",iter);

        /*int PASCAL FAR sendto( SOCKET s, const char FAR* buf, int len, int flags,const struct sockaddr FAR* to, int tolen);����
         * s��һ����ʶ�׽ӿڵ������֡���
         * buf���������������ݵĻ�����������
         * len��buf�����������ݵĳ��ȡ���
         * flags�����÷�ʽ��־λ������
         * to������ѡ��ָ�룬ָ��Ŀ���׽ӿڵĵ�ַ����
         * tolen��to��ָ��ַ�ĳ��ȡ�
����      */
        sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));
    }

    sprintf(buf,"stop\n");
    sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));//����stop ����
    close(socket_descriptor);
    printf("Messages Sent,terminating\n");

    exit(0);

    return (EXIT_SUCCESS);
}

