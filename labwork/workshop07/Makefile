# 
# File : Makefile
# Author: Joel Pillar-Rogers
# Acknowledgment: adapted from a makefile by Amine Amanzou 
# https://github.com/amineamanzou/UDP-TCP-File-Transfer
#
#
# Created on 20 May 2020
# Updated on 13 June 2020
#
#

CC = gcc

CFLAGS = -Wall

# variables for the .c files. 
C_TCPCLT = client/tcpclient.c
C_TCPSRV = server/tcpserver.c
C_UDPCLT = client/udpclient.c
C_UDPSRV = server/udpserver.c
C_TSTCLT = client/testclient.c

# variables for the a.out files. 
A_TCPCLT = client/tcpclient
A_TCPSRV = server/tcpserver
A_UDPCLT = client/udpclient
A_UDPSRV = server/udpserver
A_TSTCLT = client/testclient

all: ${A_TCPCLT} ${A_TCPSRV} ${A_UDPCLT} ${A_UDPSRV} ${A_TSTCLT}

tcp: ${A_TCPCLT} ${A_TCPSRV} 

udp: ${A_UDPCLT} ${A_UDPSRV}

test: ${A_TSTCLT} ${A_UDPSRV}

# make commands for individual files
${A_TCPCLT}: ${C_TCPCLT}
	$(CC) $(CFLAGS) -o ${A_TCPCLT} ${C_TCPCLT}

${A_TCPSRV}: ${C_TCPSRV}
	$(CC) $(CFLAGS) -o ${A_TCPSRV} ${C_TCPSRV}

${A_UDPCLT}: ${C_UDPCLT}
	$(CC) $(CFLAGS) -o ${A_UDPCLT} ${C_UDPCLT}

${A_UDPSRV}: ${C_UDPSRV}
	$(CC) $(CFLAGS) -o ${A_UDPSRV} ${C_UDPSRV}

${A_TSTCLT}: ${C_TSTCLT}
	$(CC) $(CFLAGS) -o ${A_TSTCLT} ${C_TSTCLT}

# remove all binaries, ~files and clt transmitted data files
clean:
	-rm -fv ${OFILES} ${A_TCPCLT} ${A_TCPSRV} ${A_UDPCLT} ${A_UDPSRV} ${A_TSTCLT} *~ clt.* client/*~ server/*~ server/clt.* client/test*.txt test*.txt
                 
proper: clean all

# these commands are not the names of files, they are just commands 
# so .PHONY means the makefile will always run them, won't check to see if there's a file that needs to be updated
.PHONY: all clean proper tcp udp test

# fin du Makefile (left in some French :))



