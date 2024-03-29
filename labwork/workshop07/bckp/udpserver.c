/*
 * File : udpserver.c
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
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

/* Buffer size used to send the file in several blocks */
#define BUFFER 512

/* Command to generate a test file (uses Linux' built in randomiser and the dd command)
 * dd if=/dev/urandom of=fichier count=8
 */

/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_server_socket (int port);

// Structs to hold details about the server socket details and client details
struct sockaddr_in sock_serv,clt;

int main (int argc, char**argv){
    // file descriptors
	int fd, sfd, port, conn_count = 1;
	char buffer[BUFFER];
	long long count = 0, n;
	off_t m; // long type
	char filename[256];
    unsigned int l=sizeof(struct sockaddr_in);
	char * ip_address = "127.0.0.1";
	
    // variables to save the date
	time_t intps;
	struct tm* tmi;
    
	// checks there are exactly 2 arguments provided on the command line
	if(argc != 2){
		printf("Usage error: %s [-p port_serv]\n",argv[0]);
		return EXIT_FAILURE;
	}
	// checks that the port number is in the appropriate range
	port = atoi(argv[1]);
	if(port < 10000 || port > 59999){
		printf("Usage error: port_serv must be between 10000-59999");
	}
	

    // calls the create server socket function, using the port address of the server 
	// and stores the returned socket file descriptor in sfd
    sfd = create_server_socket(port);
	printf("Listening on IP: %s and port: %d\n",ip_address,port);

	// loop to accept multiple connections
	while(1){
		intps = time(NULL);
		// convert those seconds to a tm struct called tmi
		tmi = localtime(&intps);
		// assign the time info into the string filename
		sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
		// print filename (not used in Workshop06)
		// printf("Creating an output file: %s\n",filename);
		
		// open filename (create it first) in write only mode, but make the file read/write for the user
		// the file is opened on file descriptor fd
		if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
			perror("open fail");
			return EXIT_FAILURE;
		}
		
		// preparing to receive datagrams
		// bzero sets all the bytes in the buffer to zero
		bzero(buffer,BUFFER);
		// reads BUFFER bytes from sfd into buffer and saves the source ip address and port in clt
		n=recvfrom(sfd,buffer,BUFFER,0,(struct sockaddr *)&clt,&l);
		// while there are bytes to read
		while(n){
			if(n==-1){
				perror("read fails");
				return EXIT_FAILURE;
			}
			// print the number of bytes received (not used in Workshop06)
			// printf("%lld bytes of data received \n",n);
			// cumulative count of bytes read and written
			count+=n;
			//write the bytes from buffer into the file fd
			if((m=write(fd,buffer,n))==-1){
				perror("write fail");
				exit(6);
			}
			// clear the buffer
			bzero(buffer,BUFFER);
			// read the next packet from the connection socket
			n=recvfrom(sfd,&buffer,BUFFER,0,(struct sockaddr *)&clt,&l);
		}
		// print the connection number and the total number of bytes received
		printf("Connection: \"%d\", data received: \"%lld\" \n",conn_count,count);
		
		// close the open file
		close(fd);
		// sleep for 1 second
		sleep(1);
		conn_count++;
	}
	// close the server socket and 
    close(sfd);

	return EXIT_SUCCESS;
}

/* Function to calculate the duration of the transmission. */
int duration(struct timeval *start,struct timeval *stop,struct timeval *delta)
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

/* Function allowing the creation of a socket and its attachment to the system
 * Returns a file descriptor in the process descriptor table
 * bind() allows its definition with the system
 */
int create_server_socket (int port){
    int l;
	int sfd;

    // *****setting up the local socket
	// uses the ip4 address family and the UDP socket datagram (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for use in main
	sfd = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
    // *****preparing the server socket address and server socket
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

	// sets the ip address of the sock_serv struct to all local interfaces (rather than a specific local IP address)
    // needs to have a conversion from host byte order to network byte order, which htonl does
	//sock_serv.sin_addr.s_addr=htonl("127.0.0.1");
    
	// sets the ip address of the sock_serv struct to the loopback address
	// the inet_pton function converts a character string ip address to an in_addr network address struct
    if (inet_pton(AF_INET,"127.0.0.1",&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}

	// assign an identity (sockaddr struct) to the local socket created above
    // gives the socket an IP address interface (or in this case all interfaces) to bind to
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
	// return the file descriptor number of the local socket   
    return sfd;
}