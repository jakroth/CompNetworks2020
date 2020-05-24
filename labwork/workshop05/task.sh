#!/bin/sh
a=1
while [ $a -gt 0 ] # if a is greater than zero
	do
	sleep 1 # sleep for ten seconds

	## BODY OF TEST GOES HERE
	#ping without printing to console
	#from https://www.unix.com/shell-programming-and-scripting/140143-suppress-output-ping.html
	#moves stdout to stderr and redirects to /dev/null
	ping -c 1 10.0.2.2 2>&1 >/dev/null

	## if ping fails set loop counter to zero
	if [ $? -ne 0 ] ; then
		export status="Gateway: Fail."
		a=0;
	else
		export status="Gateway: OK."
	fi

	#calculate received and transmitted bytes
	export rbytes=`/sbin/ifconfig eth0 | grep "RX packets" | sed -e "s/.*bytes//" | sed -e "s/(.*//"`
	export tbytes=`/sbin/ifconfig eth0 | grep "TX packets" | sed -e "s/.*bytes//" | sed -e "s/(.*//"`

	#calculate date and time
	export date=`date +%Y/%m/%d`
	export time=`date +%T`

	#print everything to console
	echo "Time:" $date"-"$time", Rx:" $rbytes "bytes, Tx:" $tbytes "bytes," $status
done
