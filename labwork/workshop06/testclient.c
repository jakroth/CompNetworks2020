/*
 * File : udpclient.c
 * 
 * Author: Joel Pillar-Rogers
 * 
 * Acknowledgment: adapted from a project by Amine Amanzou 
 * https://github.com/amineamanzou/UDP-TCP-File-Transfer
 * 
 * Created on 30 May 2020
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


/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int setup_server_socket (int port, char* ipaddr, struct sockaddr_in *sock_serv);

TESTS{  
    /* Declaration of structures and variables */
    // Struct to hold time details for timing the ping
    struct timeval start, stop, delta;
    // Struct to hold details about the destination server socket 
    struct sockaddr_in sock_serv;
    //size of the server address struct, used in the sendto function
    int l=sizeof(struct sockaddr_in);
    // variable for the socket file descriptor
    int sfd;
    // int and char * for storing the destination server details
    int port = 23456;
	char * ipaddr = "127.0.0.1";
    // variables to hold the message and return message (will be a random 8 numbers)
    long message = 0;
    char message_str[10];
    char rtn_message[10];
    // for holding the number of bytes transmitted and received
    off_t m; 

	// get the start time, before commencing socket setup and message transmission
	ok(gettimeofday(&start,NULL) == 0, "get the start time");

    // setting up the local socket
	// uses the ip4 address family and the UDP connectionless sock datagram (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for sending out later on
    sfd = socket(AF_INET,SOCK_DGRAM,0);
	ok(sfd >= 0, "setup the local socket");    

    // calls the setup server socket function, using the port and ip address of the destination server 
	ok(setup_server_socket(port, ipaddr, &sock_serv) >= 0, "populate the server destination struct");

    // checks that the destination port was setup correctly in the previous function
    cmp_ok(port, "==", ntohs(sock_serv.sin_port), "destination port was setup correctly");
    
    // checks that the destination IP address was setup correctly in the previous function
    // need to convert from an in_addr network address struct to a character string 
    char ip_str[12];
	inet_ntop(AF_INET, &sock_serv.sin_addr, ip_str, INET_ADDRSTRLEN);
    is(ipaddr, ip_str,"destination IP address was setup correctly");

    // print this message, confirms we are targeting the right location
    diag("******Pinging %s:%d*******",ipaddr,port);

	// initialise random number generator, using the start time
    // this will create a different random seed each time
	srand((unsigned)time(&(start.tv_sec)));
    // obtain a random number (range 10,000,000 to 99,999,999)
    message = rand() % 90000000 + 10000000; 
    sprintf(message_str,"%ld",message);

    // print the message
    diag("Message to send: %s", message_str);

    // send the message to the destination server
    // sfd is the local sending socket, message is where the data is
    // sock_serv is the destination address, and l is the type of address of sock_serv (by it's size)
    // m will hold the number of bytes sent each time
    // sendto will also bind this socket to an ip/port, automatically assigned, which will be used to receive messages later on
    ok(m=sendto(sfd,message_str,sizeof(message_str),0,(struct sockaddr*)&sock_serv,l) > 0, "sent the message to the server");

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

