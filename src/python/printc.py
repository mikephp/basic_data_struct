buf=[0x1122,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
len = len(buf)
print "len = %d"%(len)
def printc(comment,buf,len):
	print "%s"%(comment)
	for i in range(len):
		print "%02x"%(buf[i]&0xff),
		if(0==(i+1)%8):
			print "  #%d\r"%(i/8+1)
printc("test",buf,len)

