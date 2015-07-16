# coding: GBK
from urllib import urlopen
import time
i=1
while True:
    url = "http://weibo.com/zhengzhouditie?is_search=0&visible=0&is_tag="\
    "0&profile_ftype=1&page="+str(i)
    print "时间 " +time.strftime("%c")
    print url
    output = open('zzdt.html','w+')
    webdata = urlopen(url).read()
    time.sleep(2)
    output.write(webdata)
    i = i+1
output.close()
