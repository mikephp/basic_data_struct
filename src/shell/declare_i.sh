declare -i i
i=10
while [ $i -gt 0 ]
do
echo $i
i=i-1
done > "a.txt"
