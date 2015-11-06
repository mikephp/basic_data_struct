#!/usr/bin/expect
#PASSWD="embed220\n"
set timeout 5
spawn scp -r logrecv root@172.30.30.205:/root 
expect "password"
#send $PASSWD
send "embed220\r"
interact
#echo $?
#sleep 1
