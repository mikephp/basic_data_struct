import sys,os,time
timenow = time.localtime()
datenow = time.strftime('%Y-%m-%d %H:%M:%S', timenow)
print datenow
print time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
#logstr="%d:%s .%s \n" %(i,line,datenow)
#print logstr
current_file_path = sys.path[0]
print current_file_path
file = open(current_file_path+'\ipv4.txt','r')
counter = [0 for i in range(0,16)]
for i in range(0,16):
    print counter[i]
for i in range(0,16):
    print "counter[%d]=%d" %(i,counter[i])

for line in file:
    serviceid = int(line.split()[0],16)
    ruletype = int(line.split()[2],16)
    if(ruletype==8):
        counter[serviceid]= counter[serviceid]+1
print "----------------------------------------------"


print time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
for i in range(0,16):
    print "counter[%d]=%d" %(i,counter[i])
    
file.close()
