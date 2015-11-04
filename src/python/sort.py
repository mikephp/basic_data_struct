from operator import itemgetter
d={'aa':1,'c':5,'e':3,'g':2}
d=sorted(d.iteritems(),key=itemgetter(1),reverse=True)
print d
for key,value in d:
    #astr="%s %d\n" %(key,dict[key])
    #print astr
    print key,value
