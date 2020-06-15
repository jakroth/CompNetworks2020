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
#include <string.h> // for strcmp
#include <strings.h> // for bzero
#include <stdbool.h> // for bool type

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
    struct timeval start, stop, delta, rcv_start, rcv_stop, rcv_delta;
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
    char file[10] = "test1.txt";
    // variable to hold the returned filename and size
    char rtn_message[BUFFER] = "";
    char * rtn_filename = "";
    char * ascii_size = "";
    long size = 0;
    // buffer for storing the file as it is streamed over the connection
    char buffer[BUFFER];
    // for holding the return value for functions and the number of bytes transmitted and received
    off_t m,n; 
    // variable for the loop counter
    int counter = 1;
    // variable to hold the filesize
	struct stat filestat;


    // main loop
    while(counter < 6){
        diag("");
        diag("********STARTING LOOP********");
        // get the start time, before commencing socket setup and message transmission
        ok(gettimeofday(&start,NULL) == 0, "get the start time");

        // setting up the local socket
        // uses the ip4 address family and the TCP connection-oriented sock stream (and the default protocol, 0)
        // assigns a file descriptor integer to sfd, for sending out later on
        sfd = socket(AF_INET,SOCK_STREAM,0);
        ok(sfd >= 0, "setup the local socket");    

        // calls the setup server socket function, using the port and ip address of the destination server 
        // the server socket is saved in the sock_serv sockaddr_in struct
        m = setup_server_socket(port, ipaddr, &sock_serv);
        ok(m >= 0, "populate the server destination struct");

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

        // send the name of the file requested to the server
        // sfd is the local sending socket, file is the name of the file requested
        // because this socket is in a connected state, don't need to specify sendto address
        // m will hold the number of bytes sent each time
        m=send(sfd,file,sizeof(file),0); 
        ok(m > 0, "sent the filename requested to the server");

        // make sure rtn_message is empty
        bzero(rtn_message,BUFFER);

        // wait for a response from the server
        // the response will be saved in rtn_message
        // the response will be in the form "filename:size", and needs to be parsed
        recv(sfd,rtn_message,BUFFER,0);

        // break the string into filename and size
        rtn_filename = strtok(rtn_message,":");
        ascii_size = strtok(NULL,":");
        size = atoll(ascii_size);
        //diag("%s", rtn_filename);
        //diag("%lld", size);

        // test that the correct filename is returned and that the size of the file is > 0
        ok(strcmp(rtn_filename,file) == 0, "correct filename returned from server");
        ok(size > 0, "file size was greater than 0");

        // if file size is zero, print this message and go to the next loop
        if(size == 0){
            diag("file does not exist on the server");
        }
        else{

            // send the file size as a response to the server, 
            // letting the server know it was received correctly and to start sending the file. 
            // sfd is the local sending socket, size is the size of the file requested, converted into ascii
            // because this socket is in a connected state, don't need to specify sendto address
            // m will hold the number of bytes sent each time
            m=send(sfd,ascii_size,sizeof(ascii_size),0);
            ok(m > 0, "sent the file size confirmation to the server");


            // *** preparing to receive the file, all checks passed till this point
            // open file (create it first) in write only mode, but make the file read/write for the user
            // the file is opened on file descriptor fd
            fd=open(file,O_CREAT|O_WRONLY,0600);
            ok(fd >= 0, "local file created and opened successfully");
                    
            // bzero makes sure all the bytes in the buffer are set to zero
            bzero(buffer,BUFFER);


            // get the start time of when data started being sent
            ok(gettimeofday(&rcv_start,NULL) == 0, "got the start time for file transfer");

            // reads BUFFER bytes from sfd into buffer
            // waits on recv until datagram is received
            n=recv(sfd,buffer,BUFFER,0);
            buffer[BUFFER] = '\0';
            //diag("%ld",n);
            //diag("1%s",buffer);

            //int cntr = 1;
            // while there are bytes to read
            while(n>0){
                if(n==-1){
                    perror("read fail");
                    exit(6);
                }

                // write the bytes from buffer into the unique file for this connection
                if((m=write(fd,buffer,n))==-1){
                    perror("write fail");
                    exit(6);
                }
                // clear the buffer
                bzero(buffer,BUFFER);

                // tell the server it received the packet (like an ACK?)
                send(sfd,"OK",sizeof("OK"),0);

                // read the next packet from the connection socket
                // continues until it receives a 0 bytes packet from the server
                n=recv(sfd,buffer,BUFFER,0);
                buffer[n] = '\0';
                //diag("loop: %d",cntr);
                //cntr++;
            }
            // get the stop time of when data started being sent
            ok(gettimeofday(&rcv_stop,NULL) == 0, "got the stop time for file transfer");

            // close the open file
            close(fd);

            // clear the buffer
            bzero(buffer,BUFFER);
        
            // obtains information about the payload file and saves it in a stat struct
            m=stat(file,&filestat);
            ok(m == 0, "local file size obtained sucessfully");

            // check that the filesize of the payload matches the nominal size received from the server
            bool size_match = (size == (long long)filestat.st_size);
            ok(size_match, "received payload matches file size");

            // this crashes the program on larger files
            // send "OK" or "Failure" to the server, depending on whether file size matches
            //if(size_match){        
            //    ok(m=send(sfd,"OK",sizeof("OK"),0) > 0, "sent \"OK\" back to the server");
            //}
            //else{        
            //    ok(m=send(sfd,"Failure",sizeof("Failure"),0) > 0, "sent \"Failure\" back to the server");
            //}

            
            // *** reopen the file in read only mode to print the payload
            fd = open(file,O_RDONLY);
            ok(fd >= 0, "local file reopened successfully");

            // read data from the local file, print to console
            // fd is the reopened local file holding the payload
            n=read(fd,buffer,BUFFER);

            diag("***********READING THE COPIED FILE**************");
            // continue printing till no data left
            while(n>0){
                // check for read errors
                if(n==-1){
                    perror("file read fail");
                    exit(6);
                }
                // print the buffer of data to console using diag
                fprintf(stderr,buffer);

                // zero the buffer
                bzero(buffer,BUFFER);

                // read the next section of the file into the buffer
                n=read(fd,buffer,BUFFER);
            }
            // read has returned 0 = end of file

            diag("***********FINISHED READING THE COPIED FILE**************");

            // *** timings
            // get the stop time, after message transmission
            ok(gettimeofday(&stop,NULL) == 0, "got the stop time");

            //calculates the duration of the file request and receive operation
            ok(duration(&start,&stop,&delta) == 0, "calculated the duration of sending");

            // display the times for start, stop and duration
            diag("Start time: %ld.%ld, Stop time: %ld.%ld, Duration: %ld.%ld", start.tv_sec,start.tv_usec,stop.tv_sec,stop.tv_usec,delta.tv_sec,delta.tv_usec);

            //calculates the duration of the data transfer only
            ok(duration(&rcv_start,&rcv_stop,&rcv_delta) == 0, "calculated the duration of data transfer");
            float calc = ((float)filestat.st_size/rcv_delta.tv_usec);
            diag("Data transfer on the network is roughly %f MB/s", calc);

            ok((delta.tv_sec < 1 && delta.tv_usec < 1000000), "time taken was less than 1 second");    
        }

        // close the connection
        close(sfd);  

        // increase the counter
        counter++;

        // zero the file name
        bzero(file,10);

        // change the file being requested
        sprintf(file,"%s%d%s","test",counter,".txt");

        diag("***********ENDING LOOP**************");
        // sleep for 2 seconds to make sure the server is ready to receive the next connection
		sleep(2);
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

