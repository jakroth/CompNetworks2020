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

OFILES = client.o \
	server.o 

CFILECLT = client.c

CFILESRV = server.c

EXESRV = server

EXECLT = client

all: ${EXESRV} ${EXECLT}

${EXESRV}:
	$(CC) $(CFLAGS) -o ${EXESRV} ${CFILESRV}

${EXECLT}:
	$(CC) $(CFLAGS) -o ${EXECLT} ${CFILECLT}

clean:
	-rm -fv ${OFILES} ${EXESRV} ${EXECLT} *~
                 
proper: clean all

#these commands are not the names of files, they are just commands, so .PHONY means the makefile will always run them, won't check to see if updated
.PHONY: all clean proper  

# fin du Makefile



