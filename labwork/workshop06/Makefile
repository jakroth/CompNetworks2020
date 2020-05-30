# 
# File : Makefile
# Author: Joel Pillar-Rogers
# Acknowledgment: adapted from a makefile by Amine Amanzou 
# https://github.com/amineamanzou/UDP-TCP-File-Transfer
#
## Created on 20 May 2020
#
#
#

CC = gcc

CFLAGS = -Wall

# not using .o files for now.
OFILES = tcpclient.o tcpserver.o udpclient.o udpserver.o 

# variables for the .c files. not sure what value it adds.
C_TCPCLT = tcpclient.c
C_TCPSRV = tcpserver.c
C_UDPCLT = udpclient.c
C_UDPSRV = udpserver.c

# variables for the a.out files. not sure what value this adds. 
A_TCPCLT = tcpclient
A_TCPSRV = tcpserver
A_UDPCLT = udpclient
A_UDPSRV = udpserver

all: ${A_TCPCLT} ${A_TCPSRV} ${A_UDPCLT} ${A_UDPSRV}

tcp: ${A_TCPCLT} ${A_TCPSRV} 

udp: ${A_UDPCLT} ${A_UDPSRV}

# make commands for individual files
${A_TCPCLT}: ${C_TCPCLT}
	$(CC) $(CFLAGS) -o ${A_TCPCLT} ${C_TCPCLT}

${A_TCPSRV}: ${C_TCPSRV}
	$(CC) $(CFLAGS) -o ${A_TCPSRV} ${C_TCPSRV}

${A_UDPCLT}: ${C_UDPCLT}
	$(CC) $(CFLAGS) -o ${A_UDPCLT} ${C_UDPCLT}

${A_UDPSRV}: ${C_UDPSRV}
	$(CC) $(CFLAGS) -o ${A_UDPSRV} ${C_UDPSRV}

clean:
	-rm -fv ${OFILES} ${A_TCPCLT} ${A_TCPSRV} ${A_UDPCLT} ${A_UDPSRV} *~
                 
proper: clean all

#these commands are not the names of files, they are just commands 
#so .PHONY means the makefile will always run them, won't check to see if there's a file that needs to be updated
.PHONY: all clean proper  

# fin du Makefile



