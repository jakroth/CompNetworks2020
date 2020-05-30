/*
 * File : tcpclient.c
 * 
 * Author: Joel Pillar-Rogers
 * 
 * Acknowledgment: adapted from a project by Amine Amanzou 
 * https://github.com/amineamanzou/UDP-TCP-File-Transfer
 * 
 * Created on 20 May 2020
 *
 * Under GNU Licence
 */

#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

/* Buffer size used to send the file in several blocks
 */
#define BUFFER 512

/* Command to generate a test file (uses Linux' built in randomiser and the dd command)
 * dd if=/dev/urandom of=testfile count=8
 */

/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_client_socket (int port, char* ipaddr);

// Struct to hold details about the server socket
struct sockaddr_in sock_serv;

int main (int argc, char**argv){
	struct timeval start, stop, delta;
    int sfd,fd;
    char buffer[BUFFER];
    off_t count=0, m,sz; //long
	long int n;
    int l = sizeof(struct sockaddr_in);
	struct stat filestat;
    
	// checks there are exactly 4 arguments provided on the command line
	if (argc != 4){
		printf("Usage error: %s <ip_serv> <port_serv> <filename>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
	// calls the create client socket function, using the port and ip address of the destination server 
	// and stores the returned file descriptor in sfd
    sfd=create_client_socket(atoi(argv[2]), argv[1]);
    
	// opens the file containing the text to be sent, prints an error if unsuccessful
	if((fd = open(argv[3],O_RDONLY))==-1){
		perror("open fail");
		return EXIT_FAILURE;
	}
    
	// obtains information about the input file and saves it in a stat struct
	// sets the file size variable from the struct, prints an error if unsuccessful
	if(stat(argv[3],&filestat)==-1){
		perror("stat fail");
		return EXIT_FAILURE;
	}
	else
		sz=filestat.st_size;
    
	// bzero sets all the bytes in the buffer to zero
	bzero(buffer,BUFFER);
    
	// initiates a tcp socket connection between the local socket and the socket on the server
	// sfd (the local socket) and sock_serv (the server socket address) were set up earlier
	// sock_serv is an ip socket address, so needs to be cast to a general socket address to use as a parameter
	// prints an error if unsuccessful
    if(connect(sfd,(struct sockaddr*)&sock_serv,l)==-1){
        perror("connection error\n");
        exit(3);
    }

	// start the timer before sending data
	gettimeofday(&start,NULL);
    
	// the read and send function for sending data the the server
	// read data from the local file
	n=read(fd,buffer,BUFFER);
	while(n){
		// check for read errors
		if(n==-1){
			perror("read fail");
			return EXIT_FAILURE;
		}
		// send the buffer of data to the destination server
		// sfd is the local sending socket, buffer is where the data is, n is the size of the data in buffer to send
		// sock_serv is the destination address, and l is the type of address of sock_serv (by it's size)/
		// m will hold the number of bytes sent each time
		m=sendto(sfd,buffer,n,0,(struct sockaddr*)&sock_serv,l);
		// check for send errors
		if(m==-1){
			perror("send error");
			return EXIT_FAILURE;
		}
		// add to the total bytes sent
		count+=m;
		// print what's in the buffer for this read
		// fprintf(stdout,"----\n%s\n----\n",buffer);
		// zero everything in the buffer
		bzero(buffer,BUFFER);
		// read the next section of the file into the buffer
        n=read(fd,buffer,BUFFER);
	}
	// read has returned 0 = end of file
	
	// original comment said this is "to unlock the server" - not sure why it does that. may be something on the server side
	// sends 0 bytes to the server
	m=sendto(sfd,buffer,0,0,(struct sockaddr*)&sock_serv,l);

	// gets the end time after sending all packets
	gettimeofday(&stop,NULL);
	//calculates the duration of packet sending
	duration(&start,&stop,&delta);
    
	// print all the values from the whole process
	printf("Number of bytes transferred: %lld\n",count);
	printf("Out of a total size of: %lld \n",sz);
	printf("For a total duration of: %ld.%d \n",delta.tv_sec,delta.tv_usec);
    
	//close the local socket
    close(sfd);
	return EXIT_SUCCESS;
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

/* Function allowing the creation of a socket. Returns a file descriptor.
 */
int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd;
    
	// *****setting up the local socket
	// uses the ip4 address family and the tcp connection-oriented sock stream (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for use in main
	sfd = socket(AF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
	// *****preparing the destination socket address
	// finds the size of a sockadd_in struct
	l=sizeof(struct sockaddr_in);
	// zeroes the struct so no info is hanging around
	bzero(&sock_serv,l);

	// sets the sock_serv address family to use the ip4 protocol	
	// ref: https://www.tutorialspoint.com/unix_sockets/socket_structures.htm
	sock_serv.sin_family=AF_INET;
	
	// sets the port number of the sock_serv struct to the command line port_serv argument
	// needs to have a conversion from host byte order to network byte order, which htons does
	sock_serv.sin_port=htons(port);

	// sets the ip address of the sock_serv struct to the command line ip_serv argument
	// the inet_pton function converts a character string ip address to an in_addr network address struct
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}
    
	// return the file descriptor number of the local socket
    return sfd;
}