#!/bin/sh

#SERVERLIST='registryserver loginserver gatewayserver centerserver hallserver matchserver battleserver dbserver'
SERVERLIST='gatewayserver centerserver hallserver matchserver battleserver dbserver'

bin=`pwd`

ulimit -c unlimited

for serv in $SERVERLIST
do 
	cd $bin/$serv 
	nohup  ./$serv > $serv.log 2>&1 &
	sleep 1
	echo "run $serv ok ..."

	echo ""
done 

ps x | grep "server" | grep -v "vim" | grep -v "redis" | grep -v "grep" | grep -v "rungame"
