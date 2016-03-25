for i in  $(rpm -q gcc gcc-c++ openssl openssl-devel lzo lzo-devel pam pam-devel automake pkgconfig|grep 'not installed' | awk '{print $2}')
do
echo $i
yum -y  install $i;
done
