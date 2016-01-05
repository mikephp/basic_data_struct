#!/usr/bin/env python
# -*- coding: gbk -*-
__author__ = 'mikephp'
import socket,time,sys,os,struct
current_file_path = sys.path[0]
print current_file_path
def ip2int(ip):
    return socket.ntohl(struct.unpack("I",socket.inet_aton(ip))[0])
def int2ip(ip):
    if ip < 0:
        ip = struct.unpack("I", struct.pack('i', ip))[0]
    return socket.inet_ntoa(struct.pack('I',socket.htonl(ip)))

def scan_ip_port(ip_start,ip_step,ip_end,port,port_step):
    #file_obj = open(current_file_path+'/ip.txt')
    print 'start ip %s'% ip_start
    file = open(current_file_path+"/log.txt", "a")
    file_failed = open(current_file_path+"/log_failed.txt", "a")
    #a=socket.inet_aton()
    #for line in file_obj:
    print type(ip_start)
    current_ip = ip2int(ip_start)
    #current_ip = int(socket.inet_aton(ip_start),16)
    print current_ip
    current_port = port
    print type(current_ip)
    print type(ip_step)
    while current_ip < socket.inet_aton(ip_end):
        print 'current ip %s,port %d'%(int2ip(current_ip),current_port)
        try:
            sc=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            #ip = line.split()[0]
            #port = int(line.split()[1])
            #print curren_ip,port
            #设置超时时间（0.0）
            sc.settimeout(1)
            sc.connect((current_ip,current_port))
            timenow=time.localtime()
            datenow = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
            logstr="%s:%d 连接成功->%s \n" %(socket.inet_ntoa(current_ip),current_port,datenow)
            print logstr
            sc.close()
        except:
            timenow=time.localtime()
            datenow = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
            logstr="%s:%d 连接失败->%s \n" %(int2ip(current_ip),current_port,datenow)
            print logstr
            file_failed.write(logstr)

        current_ip = (current_ip) + (ip_step)
        current_port = current_port + port_step
    #file_obj.close()
    file.close()
    file_failed.close()

ip_start = '172.16.0.1'
ip_end = '172.31.255.254'
ip_step = 1
port=170
port_step=0

scan_ip_port(ip_start,ip_step,ip_end,port,port_step)
print "sleep 10....."
time.sleep(3)
print "task over"
