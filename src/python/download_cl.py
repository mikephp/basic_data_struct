# coding: GBK
import urllib 
import urllib2
import time
i=1
user_agent = 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)'
values = {'name' : 'Mozilla',  
          'location' : 'HK',  
          'language' : 'html' }  
headers = { 'User-Agent' : user_agent }  
data = urllib.urlencode(values)  
while True:
    url = "http://123.com/"+str(i)
    print "时间" +time.strftime("%c")
    print url
    url_save = "cl/"+str(i)+".html"
    print url_save
    output = open(url_save,'w+')
    req2 = urllib2.Request(url,data,headers)
    result = urllib2.urlopen(req2)
    webdata = result.read()
    time.sleep(1)
    output.write(webdata)
    i = i+1
output.close()
