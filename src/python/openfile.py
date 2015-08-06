from urllib import urlopen
file_name = "model_10w.txt"

file_path = 'D:/work/'

file_object = open(file_path+file_name,'r')

str = "00  00  00  00  00  00  00  00"
n = 30
i = 0
try:
    while True:
        line = file_object.readline()
        if not line:
            break
        else:
            #print len(line)
            if(0==cmp(line[0:n],str[0:n])):
                print i,line
            i = i+1
finally:
    print i,"over"
    file_object.close( )
