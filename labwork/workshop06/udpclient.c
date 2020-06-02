/*
 * File : udpclient.c
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

// Time function, sockets, htons, file stat
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

/* Declaration of functions*/
int create_client_socket (int port, char* ipaddr);

// Struct to hold details about the server socket 
struct sockaddr_in sock_serv;

int main (int argc, char**argv){
    int sfd,fd,flag = 0,opt,port;
    char buffer[BUFFER];
	char *filename, *ipaddr;
	long long count = 0,sz,rem_sz;
    off_t m;//long
	long int n;
    int l=sizeof(struct sockaddr_in);
	struct stat filestat;
	time_t t;

	// checks there are exactly 3 or 5 arguments provided on the command line
	if(!(argc == 3 || argc == 5)){
		printf("Usage error: %s <ip_serv> <port_serv> [-f file]\n",argv[0]);
		return EXIT_FAILURE;
	}
	// assigns the ip address and port number before getopt messes with them
	ipaddr = argv[1];
	port = atoi(argv[2]);

	// check to see if a file was passed in. if not, generate random file.
	while ((opt = getopt(argc, argv, "f:")) != -1){
        switch (opt){
        case 'f':
			filename = optarg;
			flag = 1;
            break;
        case '?':
			printf("Usage error: %s <ip_serv> <port_serv> [-f file]\n",argv[0]);
            exit(EXIT_FAILURE);
        }
    }

	// main loop to continuosly ping random amounts of data at the server
	while(1){
		// initialise random number generator
		srand((unsigned) time(&t));
		// if a filename was passed in
		if(flag){
			// obtains information about the input file and saves it in a stat struct
			// sets the file size variable from the struct, prints an error if unsuccessful
			if (stat(filename,&filestat)==-1){
				perror("stat fail");
				return EXIT_FAILURE;
			}
			sz = filestat.st_size;
			rem_sz = rand() % sz;	// will choose a random amount of the bytes to transmit
		}
		// if a filename wasn't passed in, use /dev/urandom as the file
		else{
			filename = "/dev/urandom";
			sz = rand() % 10000; 	// obtain a random number of bytes (max is 9999)
			rem_sz = sz;
		}
		// opens the file containing the text to be sent, prints an error if unsuccessful
		if((fd = open(filename,O_RDONLY))==-1){
			perror("open fail");
			return EXIT_FAILURE;
		}

		printf("******Pinging %s:%d*******\n",ipaddr,port);
		// calls the create local socket function, using the port and ip address of the destination server 
		// stores the returned file descriptor in sfd
		sfd=create_client_socket(port, ipaddr);
		printf("%d\n",stdout->_fileno);
		printf("%d\n",stderr->_fileno);
		printf("%d\n",fd);
		printf("%d\n",sfd);

		// bzero sets all the bytes in the buffer to zero
		bzero(buffer,BUFFER);

		// the read and send function for sending data the the server
		// read data from the local file
		// limited by either end of file or rem_sz, whatever comes first
		if(rem_sz > BUFFER){
			n = read(fd,buffer,BUFFER);
			rem_sz -= n;
		} else {
			n = read(fd,buffer,rem_sz);
			rem_sz -= n;
		}
		// read and send while not at end of file or rem_sz != 0
		while(n){
			// check for read errors
			if(n==-1){
				perror("read fails");
				return EXIT_FAILURE;
			}
			// send the buffer of data to the destination server
			// sfd is the local sending socket, buffer is where the data is, n is the size of the data in buffer to send
			// sock_serv is the destination address, and l is the type of address of sock_serv (by it's size)/
			// m will hold the number of bytes sent each time
			// sendto will also bind this socket to an ip/port, automatically assigned, so can be used to recv later on
			m=sendto(sfd,buffer,n,0,(struct sockaddr*)&sock_serv,l);
			// check for send errors
			if(m==-1){
				perror("send error");
				return EXIT_FAILURE;
			}
			// add to the total bytes sent
			count+=m;
			// print what's in the buffer for this read
			//fprintf(stdout,"----\n%s\n----\n",buffer);
			// zero everything in the buffer
			bzero(buffer,BUFFER);
			// read the next section of the file into the buffer
			if(rem_sz > BUFFER){
				n = read(fd,buffer,BUFFER);
				rem_sz -= n;
			} else {
				n = read(fd,buffer,rem_sz);
				rem_sz -= n;
			}
		}
		// read has returned 0 = end of file
		
		// unlock the server by sending 0 bytes, signifying end of file
		m=sendto(sfd,buffer,0,0,(struct sockaddr*)&sock_serv,l);
		
		// print bytes transferred
		printf("Number of bytes transferred: %lld\n",count);

		// zero the buffer	
		bzero(buffer,BUFFER);

		// wait for a response from the server
		// reads BUFFER bytes from sfd into buffer, don't bother to save server details (don't need them)
		// ref: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
		n=recvfrom(sfd,buffer,BUFFER,0,NULL,NULL);
		printf("Message from server: %s\n", buffer);
		
		// sleep for 2 seconds to make sure the server is ready to receive the next connection
		sleep(2);

		//close the local socket
		close(sfd);
		close(fd);
		count = 0;
	}
	return EXIT_SUCCESS;
}

/* Function allowing the creation of a socket. Returns a file descriptor. */
int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd;
    // *****setting up the local socket
	// uses the ip4 address family and the UDP connectionless sock datagram (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for use in main
	sfd = socket(AF_INET,SOCK_DGRAM,0);
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
