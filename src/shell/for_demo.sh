#/bin/bash
# author: <a title="" href="http://blog.csdn.net/ablo_zhou" target="_blank">�ܺ���</a>
# date :2010.3.25
# blog.csdn.net/ablo_zhou
arr=("a" "b" "c")
echo "arr is (${arr[@]})"
echo "item in array:"
for i in ${arr[@]}
do
 echo "$i"
done
echo "����,/$*��ʾ�ű���������в�����"
for i in $* ; do
echo $i
done
echo
echo '�����ļ� /proc/sys/net/ipv4/conf/*/accept_redirects��'
for File in /proc/sys/net/ipv4/conf/*/accept_redirects; do
echo $File
done
echo "ֱ��ָ��ѭ������"
for i in f1 f2 f3 ;do
echo $i
done
echo 
echo "C �﷨for ѭ��:"
for (( i=0; i<10; i++)); do
echo $i
done 

for i in {1..10};
do
	echo $i;
done;

for i in $(seq 10);
do
	echo $i;
done;