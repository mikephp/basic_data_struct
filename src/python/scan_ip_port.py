#!/usr/bin/env python
# -*- coding: gbk -*-
import socket,time,sys,os
current_file_path = sys.path[0]
print current_file_path
def scan_ip_port():
	file_obj = open(current_file_path+'/ip.txt')
	file = open(current_file_path+"/log.txt", "a")
	for line in file_obj:
		try:
			sc=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			ip = line.split()[0]
			port = int(line.split()[1])
			print ip,port
			#���ó�ʱʱ�䣨0.0��
			sc.settimeout(2)
			sc.connect((ip,port))
			timenow=time.localtime()
			datenow = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
			logstr="%s:%s ���ӳɹ�->%s \n" %(ip,port,datenow)
			print logstr
			sc.close()
		except:
			timenow=time.localtime()
			datenow = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
			logstr="%s:%s ����ʧ��->%s \n" %(ip,port,datenow)
			print logstr
			file.write(logstr)
	file_obj.close()
	file.close()
  
scan_ip_port()
print "sleep 10....."
time.sleep(10)
print "task over"
