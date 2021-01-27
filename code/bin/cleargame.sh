#!/bin/sh

SERVERLIST='registryserver loginserver gatewayserver centerserver hallserver matchserver battleserver dbserver'

bin=`pwd`

ulimit -c unlimited

for serv in $SERVERLIST
do 
	rm -rf $bin/$serv/core.*
	rm -rf $bin/$serv/log
	rm -rf $bin/$serv/*.log
	rm -rf $bin/$serv/$serv
	rm -rf $bin/$serv/Debug
	rm -rf $bin/$serv/Release
	echo "$serv clear core log exe ok ..."
done 

