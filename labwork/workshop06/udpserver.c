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
	int fd, sfd;
	char buffer[BUFFER];
	off_t count=0, n; // long type
	char filename[256];
    unsigned int l=sizeof(struct sockaddr_in);
	
    // variables to save the date
	time_t intps;
	struct tm* tmi;
    
	// checks there are exactly 2 arguments provided on the command line
	if (argc != 2){
		printf("Usage error: %s <port_serv>\n",argv[0]);
		return EXIT_FAILURE;
	}

    // calls the create server socket function, using the port address of the server 
	// and stores the returned socket file descriptor in sfd
    sfd = create_server_socket(atoi(argv[1]));
    
	intps = time(NULL);
	tmi = localtime(&intps);
	bzero(filename,256);
	sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
	printf("Creating the output file : %s\n",filename);
    
	//ouverture du fichier
	if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
		perror("open fail");
		return EXIT_FAILURE;
	}
    
	//preparation de l'envoie
	bzero(&buf,BUFFER);
    n=recvfrom(sfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	while(n){
		printf("%lld of data received \n",n);
		if(n==-1){
			perror("read fails");
			return EXIT_FAILURE;
		}
		count+=n;
		write(fd,buf,n);
		bzero(buf,BUFFER);
        n=recvfrom(sfd,&buf,BUFFER,0,(struct sockaddr *)&clt,&l);
	}
    
	printf("Nombre d'octets transférés : %lld \n",count);
    
    close(sfd);
    close(fd);
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
	sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
	// assign an identity (sockaddr struct) to the local socket created above
    // gives the socket an IP address interface (or in this case all interfaces) to bind to
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
	// return the file descriptor number of the local socket   
    return sfd;
}