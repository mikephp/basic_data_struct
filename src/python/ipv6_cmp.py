#!/usr/local/bin/python
print "hello"
import sys,os,time
from operator import itemgetter
timenow = time.localtime()
start = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
print "start",start
#print time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
def file_cmp(filename,str):
	os_path = sys.path[0]
	print os_path
	n=7
	i=0
	file = open(os_path+filename,"r")
	for line in file:
		i=i+1
		if not line:
			break
		else:
			s1=line.split()[0]
			s2=line.split()[1]
		if(0!=cmp(s1[0:n],str[0:n])):
			print i,s1
	print i
	file.close()

filename = "/sipdip_hash.txt"
str="00000000"
arr=[]
d={}
def fun(filename,savefile):
	path = sys.path[0]
	f2=open(path+savefile,'a+')
	if not f2:
		print "open file failed\n"
		return -1
	print "open file %s ok!" %(savefile)
	os_path=sys.path[0]
	print os_path
	file = open(os_path+filename,"r")
	i=0
	for line in file:
		i=i+1
		if not file:
			break
		else:
			s1=line.split()[0]
			s2=line.split()[1]
		arr.append(s2)
	file.close()
	word_dict={}
	for item in arr:
		if item not in word_dict:
			word_dict[item]=1
		else:
			word_dict[item]+=1
	
	d=sorted(word_dict.items(),key=lambda x:x[1],reverse=True)
	
	for key,v in d:
		astr="%s %d\n" %(key,v)
		f2.write(astr)
	f2.close()
	print len(d)
	print "d",type(d)
	print "arr",type(arr)
	print "word_dict",type(word_dict)


def fun_ip_no(master,client):
	curent_path = sys.path[0]
	print curent_path
	fmaster = open(curent_path+master,"r")	
	fclient = open(curent_path+client,"r")
	counter=0
	for line_m in fmaster:
		counter=counter+1
		line_c = fclient.readline()
		sid_index=line_m.find("service")
		if -1!=sid_index:
			sid_m = line_m[sid_index+8:(sid_index+10)]
		else:
			sid_m = ""

		sip_index=line_m.find("sip")
		if -1!=sip_index:
			sip_m = line_m[sip_index+4:(sip_index+43)]
		else:
			sip_m = '0000:0000:0000:0000:0000:0000:0000:0000'

		dip_index=line_m.find("dip")
		if -1!=dip_index:
			dip_m = line_m[dip_index+4:(dip_index+43)]
		else:
			dip_m = '0000:0000:0000:0000:0000:0000:0000:0000'

		sport_index=line_m.find("sport")
		if -1!=sport_index:
			sport_m = line_m[sport_index+8:(sport_index+12)]
		else:
			sport_m = '0000'

		dport_index=line_m.find("dport")
		if -1!=dport_index:
			dport_m = line_m[dport_index+8:(dport_index+12)]
		else:
			dport_m = '0000'
		
		sid_c = line_c[0:2]
		sip_c = line_c[9:44]
		dip_c = line_c[45:80]
		sport_c = line_c[81:85]
		dport_c = line_c[85:89]
		#print type(sid_c),type(sid_m),sid_c,sid_m,len(sid_c),len(sid_m)
		#print i,type(sip_c),type(sip_m),sip_c,sip_m,len(sip_c),len(sip_m)
		#print i,type(dip_c),type(dip_m),dip_c,dip_m,len(dip_c),len(dip_m)
		#print i,type(sport_c),type(sport_m),sport_c,sport_m,len(sport_c),len(sport_m)
		#print i,type(dport_c),type(dport_m),dport_c,dport_m,len(dport_c),len(dport_m)
	
		#print line_c
		#print i,"service:",sid_m,",sip:",sip_m,",dip:",dip_m,",sport:",sport_m,",dport:",dport_m
		#print i,sip_index,sip_m
		#ip_m  line_m.split()[2]		
		#sport_m = line_m.split()[2]		
		#dport_m = line_m.split()[2]	
		if (int(sid_c,16)!=int(sid_m,10) | 0!=cmp(sport_c,dport_m) | 0!=cmp(dport_c,dport_m)):
			print i,sid_c,sid_m
		#sip_m=0240:0000:0000:0000:0000:0000:0000:0011 
		#sip_c=02400000 00000000 00000000 00000021
		sip_m_buf=[]
		dip_m_buf=[]
		for i in range(0,8):
			sip_m_buf.append(sip_m.split(':')[i])
			dip_m_buf.append(dip_m.split(':')[i])
			#print sip_m_buf[i]	
		sip_c_buf=[]
		sip_m_buf_2=[]
		
		dip_c_buf=[]
		dip_m_buf_2=[]

		for i in range(0,4):
			sip_c_buf.append(sip_c.split(' ')[i])
			dip_c_buf.append(dip_c.split(' ')[i])
			sip_m_buf_2.append(sip_m_buf[2*i]+sip_m_buf[2*i+1])
			dip_m_buf_2.append(dip_m_buf[2*i]+dip_m_buf[2*i+1])
			#print sip_c_buf[i],sip_m_buf_2[i]
			if(0!=cmp(sip_m_buf_2[i].upper(),sip_c_buf[i])):
				print i,sip_m_buf_2[i],sip_c_buf[i]," #sip client"
			if(0!=cmp(dip_m_buf_2[i].upper(),dip_c_buf[i])):
				print i,dip_m_buf_2[i],dip_c_buf[i]," #dip client"

	print counter,sid_c,sip_c,dip_c,sport_c,dport_c
	fmaster.close()
	fclient.close()

def fun_ip_no_method_2(master,client):
	curent_path = sys.path[0]
	print curent_path
	fmaster = open(curent_path+master,"r")	
	fclient = open(curent_path+client,"r")
	counter=0
	for line_m in fmaster:
		counter=counter+1
		line_c = fclient.readline()
		sid_index=line_m.find("service")
		if -1!=sid_index:
			sid_m = line_m[sid_index+8:(sid_index+10)]
		else:
			sid_m = ""

		sip_index=line_m.find("sip")
		if -1!=sip_index:
			sip_m = line_m[sip_index+4:(sip_index+43)]
		else:
			sip_m = '0000:0000:0000:0000:0000:0000:0000:0000'

		dip_index=line_m.find("dip")
		if -1!=dip_index:
			dip_m = line_m[dip_index+4:(dip_index+43)]
		else:
			dip_m = '0000:0000:0000:0000:0000:0000:0000:0000'

		sport_index=line_m.find("sport")
		if -1!=sport_index:
			sport_m = line_m[sport_index+8:(sport_index+12)]
		else:
			sport_m = '0000'

		dport_index=line_m.find("dport")
		if -1!=dport_index:
			dport_m = line_m[dport_index+8:(dport_index+12)]
		else:
			dport_m = '0000'
		
		sid_c = line_c[0:2]
		sip_c = line_c[9:44]
		dip_c = line_c[45:80]
		sport_c = line_c[81:85]
		dport_c = line_c[85:89]
		
		if (int(sid_c,16)!=int(sid_m,10) | 0!=cmp(sport_c,dport_m) | 0!=cmp(dport_c,dport_m)):
			print i,sid_c,sid_m
		#sip_m=0240:0000:0000:0000:0000:0000:0000:0011 
		#sip_c=02400000 00000000 00000000 00000021
		sip_m_buf=[]
		dip_m_buf=[]
		for i in range(0,8):
			sip_m_buf.append(sip_m.split(':')[i])
			dip_m_buf.append(dip_m.split(':')[i])
		
		sip_c_buf=[]
		sip_m_buf_2=[]
		dip_c_buf=[]
		dip_m_buf_2=[]

		for i in range(0,4):
			sip_c_buf.append(sip_c.split(' ')[i])
			dip_c_buf.append(dip_c.split(' ')[i])
			sip_m_buf_2.append(sip_m_buf[2*i]+sip_m_buf[2*i+1])
			dip_m_buf_2.append(dip_m_buf[2*i]+dip_m_buf[2*i+1])
			if(0!=cmp(sip_m_buf_2[i].upper(),sip_c_buf[i])):
				print i,sip_m_buf_2[i],sip_c_buf[i]," #sip client"
			if(0!=cmp(dip_m_buf_2[i].upper(),dip_c_buf[i])):
				print i,dip_m_buf_2[i],dip_c_buf[i]," #dip client"

	print counter,sid_c,sip_c,dip_c,sport_c,dport_c
	fmaster.close()
	fclient.close()

fun_ipv6_no_mask_method_2('/a_cfg','/b_rul')

end=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()) 
print "end  ",end,type(end)
