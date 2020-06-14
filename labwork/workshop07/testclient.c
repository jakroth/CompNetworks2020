/*
 * File : testclient.c
 * 
 * Author: Joel Pillar-Rogers
 * 
 * Acknowledgment: adapted from a project by Amine Amanzou 
 * https://github.com/amineamanzou/UDP-TCP-File-Transfer
 * 
 * Created on 20 May 2020
 * Updated on 13 June 2020
 *
 * Under GNU Licence
 */

#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// File functions and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h> // for bzero

// Testing
#include "ctap.h"

//BUFFER size for transferring data
#define BUFFER 512

/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int setup_server_socket (int port, char* ipaddr, struct sockaddr_in *sock_serv);

TESTS{  
    /* Declaration of structures and variables */
    // Struct to hold time details for timing the http request
    struct timeval start, stop, delta;
    // Struct to hold details about the destination server socket 
    struct sockaddr_in sock_serv;
    //size of the server address struct, used in the sendto function
    int l=sizeof(struct sockaddr_in);
    // variable for the socket file descriptor
    int sfd;
    // variable for the local file descriptor
    int fd;
    // int and char * for storing the destination server details
    int port = 45678;
	char * ipaddr = "127.0.0.1";
    char * file = "test1.txt";
    // variable to hold the returned filename and size
    char rtn_message[64] = "";
    char rtn_filename[64] = "";
    long long size = 0;
    // for holding the number of bytes transmitted and received
    off_t m; 
    // variable for the loop counter
    int counter = 1;

    while(counter < 6){

        // get the start time, before commencing socket setup and message transmission
        ok(gettimeofday(&start,NULL) == 0, "get the start time");

        // setting up the local socket
        // uses the ip4 address family and the TCP connection-oriented sock stream (and the default protocol, 0)
        // assigns a file descriptor integer to sfd, for sending out later on
        sfd = socket(AF_INET,SOCK_STREAM,0);
        ok(sfd >= 0, "setup the local socket");    

        // calls the setup server socket function, using the port and ip address of the destination server 
        // the server socket is saved in the sock_serv sockaddr_in struct
        ok(setup_server_socket(port, ipaddr, &sock_serv) >= 0, "populate the server destination struct");

        // initiates a tcp socket connection between the local socket and the socket on the server
        // sfd (the local socket) and sock_serv (the server socket address) were set up previously
        // sock_serv is an ip socket address struct, so needs to be cast to a general socket address to use as a parameter
        // connect will also bind the local socket to an ip/port, automatically assigned, which will be used to receive messages later on
        m=connect(sfd,(struct sockaddr*)&sock_serv,l);
        ok(m == 0, "connection established successfully");

        // checks that the destination port was setup correctly in the previous function
        cmp_ok(port, "==", ntohs(sock_serv.sin_port), "destination port was setup correctly");
        
        // checks that the destination IP address was setup correctly in the previous function
        // need to convert from an in_addr network address struct to a character string 
        char ip_str[12];
        inet_ntop(AF_INET, &sock_serv.sin_addr, ip_str, INET_ADDRSTRLEN);
        is(ipaddr, ip_str,"destination IP address was setup correctly");

        // print this message, confirms we are targeting the right location
        diag("******Target server: %s:%d*******",ipaddr,port);

        // print this message, confirms the file we are requesting in this loop
        diag("******File requested: %s******",file);

        // send the the name of the file requested to the server
        // sfd is the local sending socket, file is the name of the file requested
        // because this socket is in a connected state, don't need to specify sendto address
        // m will hold the number of bytes sent each time
        ok(m=send(sfd,file,sizeof(file),0) > 0, "sent the filename requested to the server");

        // make sure rtn_string is empty
        bzero(rtn_message,64);

        // wait for a response from the server
        // the response will be in the form "filename:size", and needs to be parsed
        // the response will be saved in rtn_message
        recv(sfd,rtn_message,64,0);

        // break the string into filename and size
        rtn_filename = strtok(rtn_message,":");
        size = atoi(strtok(NULL,":"));
        diag("%s", filename);
        diag("%lld", size);

        // test that the correct filename is returned and that the size of the file is > 0
        ok(rtn_filename == file, "correct filename returned from server");
        ok(size > 0, "file size was greater than 0");

        // if file size is zero, print this message and go to the next loop
        if(size == 0){
            diag("file does not exist on the server");
            continue;
        } 



        // open filename (create it first) in write only mode, but make the file read/write for the user
		// the file is opened on file descriptor fd
		(fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600));
        ok(fd == 0, "local file created and opened successfully");
        		
		// ***preparing to receive datagrams
		// bzero sets all the bytes in the buffer to zero
		bzero(buffer,BUFFER);

		// reads BUFFER bytes from sfd into buffer and saves the source ip address and port in clt
		// waits on recvfrom until datagram is received
		n=recvfrom(sfd,buffer,BUFFER,0,(struct sockaddr *)&clt,&l);

		// while there are bytes to read
		while(n){
			if(n==-1){
				perror("read fails");
				return EXIT_FAILURE;
			}
			// cumulative count of bytes read and written
			count+=n;

			//write the bytes from buffer into the unique file for this connection
			if((m=write(fd,buffer,n))==-1){
				perror("write fail");
				exit(6);
			}
			// clear the buffer
			bzero(buffer,BUFFER);

			// read the next packet from the connection socket
			// continues until there is nothing else on the listening socket
			n=recvfrom(sfd,&buffer,BUFFER,0,(struct sockaddr *)&clt,&l);
		}
		// close the open file
		close(fd);

		// clear the buffer
		bzero(buffer,BUFFER);







        // unlock the server by sending 0 bytes, signifying end of file
        ok(m=sendto(sfd,NULL,0,0,(struct sockaddr*)&sock_serv,l) == 0, "sent message to unlock the server, indicating last datagram");

        // wait for a response from the server
        // reads 8 bytes from sfd into rtn_message, don't bother to save server details (don't need them)
        // ref: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
        ok(m=recvfrom(sfd,rtn_message,8,0,NULL,NULL) == 8, "received return message from server");

        // check that the returned string matches the transmitted string
        is(rtn_message,message_str,"return data matches sent data");

        // print the return message
        diag("Message from server: %s", rtn_message);

        // get the stop time, after message transmission
        ok(gettimeofday(&stop,NULL) == 0, "got the stop time");

        //calculates the duration of packet sending
        ok(duration(&start,&stop,&delta) == 0, "calculated the duration of sending");

        // display the times for start, stop and duration
        diag("Start time: %ld.%ld, Stop time: %ld.%ld, Duration: %ld.%ld", start.tv_sec,start.tv_usec,stop.tv_sec,stop.tv_usec,delta.tv_sec,delta.tv_usec);

        ok(delta.tv_usec < 1000000, "time taken was less than 1 second");    

        // increase the counter
        counter++;
        // change the file being requested
        file = "test" + counter + ".txt";
    }
}


/* Function to setup the destination server sockaddr_in struct */
int setup_server_socket (int port, char* ipaddr, struct sockaddr_in * sock_serv){
    
    // finds the size of a sockaddr_in struct
	int l=sizeof(struct sockaddr_in);

	// zeroes the struct so no info is hanging around
	bzero(sock_serv,l);
	
	// sets the sock_serv address family to use the ip4 protocol	
	// ref: https://www.tutorialspoint.com/unix_sockets/socket_structures.htm
	sock_serv->sin_family=AF_INET;

	// sets the port number of the sock_serv struct to the command line port_serv argument
	// needs to have a conversion from host byte order to network byte order, which htons does
	sock_serv->sin_port=htons(port);

    // sets the ip address of the sock_serv struct to the command line ip_serv argument
	// the inet_pton function converts a character string ip address to an in_addr network address struct
    struct in_addr temp;
    if (inet_pton(AF_INET,ipaddr,&temp)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}
    sock_serv->sin_addr = temp;

    return 1;
}


/* Function to calculate the duration of the sending. */
int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
	// these are just longs
    suseconds_t microstart, microstop, microdelta;
    
	// calculate the difference between the start and stop timeval structs
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
	// set the values of the delta timeval struct. uses pointers, so accessible outside this function
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
	// check for errors
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}

