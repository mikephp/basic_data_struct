from urllib import urlopen
from HTMLParser  import HTMLParser
import time
url="http://web.mit.edu/freebsd/head/sys/contrib/octeon-sdk/"
baseurl=url
class hp(HTMLParser):
    a_text = False  
    def __init__(self):   
        HTMLParser.__init__(self)   
        self.links = []
        self.data_value = []
    def handle_starttag(self,tag,attr):  
        if tag == 'a' or tag == 'A':  
            self.a_text = True  
            #print (dict(attr))  
            #print attr[0]
            if len(attr) == 0:   
                pass   
            else:   
                for (variable, value) in attr:   
                    if variable == "href" or variable == "HREF":   
                        self.links.append(value)
    def handle_endtag(self,tag):  
        if tag == 'a' or tag == 'A':  
            self.a_text = False  
              
    def handle_data(self,data):  
        if self.a_text:
            self.data_value.append(data)

outf = open('octeon-sdk.html','w+')
webdata = urlopen(url).read()
outf.write(webdata)
#time.sleep(2)
outf.close()

yk = hp()
yk.feed(webdata)
yk.close()
length = len(yk.data_value)
print length
for i in range(132,length):
    print "i=%d,%s\n" %(i,yk.data_value[i])
    filename = yk.data_value[i]
    of = open(filename,'w+')
    ourl=baseurl+filename
    ofdata = urlopen(ourl).read()
    of.write(ofdata)
    of.close()
#print (yk.data_value)
#print len(yk.data_value)