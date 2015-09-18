#for ((i=0; i<100; i++))
#do
#echo ${array[$i]}
#echo "/usr/local/bin/"$i
#/u2sr/local/bin/$i
#done
filename="DPI_L2.txt"
echo $filename
while read myline
do
#echo "LINE:"$myline
$cli_path -c "$myline"
done < $filename
