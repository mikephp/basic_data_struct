#/bin/bash
# author: <a title="" href="http://blog.csdn.net/ablo_zhou" target="_blank">周海汉</a>
# date :2010.3.25
# blog.csdn.net/ablo_zhou
arr=("a" "b" "c")
echo "arr is (${arr[@]})"
echo "item in array:"
for i in ${arr[@]}
do
 echo "$i"
done
echo "参数,/$*表示脚本输入的所有参数："
for i in $* ; do
echo $i
done
echo
echo '处理文件 /proc/sys/net/ipv4/conf/*/accept_redirects：'
for File in /proc/sys/net/ipv4/conf/*/accept_redirects; do
echo $File
done
echo "直接指定循环内容"
for i in f1 f2 f3 ;do
echo $i
done
echo 
echo "C 语法for 循环:"
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