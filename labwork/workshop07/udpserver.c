/*
 * File : udpserver.c
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

// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

/* Buffer size used to send the file in several blocks */
#define BUFFER 512

/* Declaration of functions*/
int create_server_socket (int port, char *ipaddr);

// Structs to hold details about the server socket details and client details
struct sockaddr_in sock_serv,clt;

int main (int argc, char**argv){
    // file descriptors
	int fd, sfd, port, opt, flag = 0, conn_count = 1;
	char buffer[BUFFER];
	long long count = 0, n;
	off_t m; // long type
	char filename[256], rtn_string[50];
    unsigned int l=sizeof(struct sockaddr_in);
	char * ip_address = "127.0.0.1";
	
    // variables to save the date
	time_t intps;
	struct tm* tmi;
    
	// checks there are exactly 1 or 3 arguments provided on the command line
	if(!(argc == 3 || argc == 1)){
		printf("Usage error: %s [-p port_serv]\n",argv[0]);
		return EXIT_FAILURE;
	}
	// check to see if a port number was specified. if not, use 23456.
	while ((opt = getopt(argc, argv, "p:")) != -1){
        switch (opt){
        case 'p':
			port = atoi(optarg);
			flag = 1;
            break;
        case '?':
			printf("Usage error: %s [-p port_serv]\n",argv[0]);
            exit(EXIT_FAILURE);
        }
    }
	// uses a default port if not set in the command line
	if(flag == 0){
		port = 23456;
	}

	// checks that the port number is in the appropriate range, ports above 1024 can be accessed in user space
	if(port < 10000 || port > 59999){
		printf("Usage error: port_serv must be between 10000-59999");
	}

	printf("Listening on IP: %s and port: %d\n",ip_address,port);

	// loop to accept multiple connections
	while(1){
		// calls the create server socket function, using the port address of the server 
		// and stores the returned socket file descriptor in sfd
		sfd = create_server_socket(port, ip_address);

		// grab the current time
		intps = time(NULL);
		// convert those seconds to a tm struct called tmi
		tmi = localtime(&intps);
		// assign the time info into the string filename, this creates a unique filename for every connection
		sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
		
		// open filename (create it first) in write only mode, but make the file read/write for the user
		// the file is opened on file descriptor fd
		if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
			perror("open fail");
			return EXIT_FAILURE;
		}
		
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

		// opens the file just created, prints an error if unsuccessful
		if((fd = open(filename,O_RDONLY))==-1){
			perror("open fail");
			return EXIT_FAILURE;
		}

		// reads the first 512 bytes from the file into the buffer
		read(fd,buffer,BUFFER);

		// convert the first 512 bytes into a string so it can be sent to the client 
		sprintf(rtn_string,"%s",buffer);

		// send the first 512 bytes as a response to the client
		// ref: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
		sendto(sfd,rtn_string,sizeof(rtn_string),MSG_CONFIRM,(struct sockaddr*)&clt,l);

		// close the server socket 
		close(sfd);

		// get the IP address from the clt connection
		// need to convert from an in_addr network address struct to a character string 
		char clt_ip[20];
		inet_ntop(AF_INET,&clt.sin_addr, clt_ip, 20);

		// print the connection number, total number of bytes received, IP address and port number of client
		printf("Connection: \"%d\", data received: \"%s\", from IP: \"%s\", on port: \"%d\" \n",conn_count,rtn_string,clt_ip,clt.sin_port);

		// reset the data counter and connection loop counter
		conn_count++;
		count = 0;

		// sleep for 1 second to make sure the server is ready to receive the next connections
		sleep(1);
	}

	return EXIT_SUCCESS;
}

/* Function allowing the creation of a socket and its attachment to the system
 * Returns a file descriptor in the process descriptor table
 * bind() allows its definition with the system
 */
int create_server_socket (int port, char *ipaddr){
    int l;
	int sfd;

    // *****setting up the local socket
	// this sets up a socket, the first step in allowing a connection
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

	// *** currently not used ***
	// sets the ip address of the sock_serv struct to all local interfaces (rather than a specific local IP address)
    // needs to have a conversion from host byte order to network byte order, which htonl does
	//sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
	// sets the ip address of the sock_serv struct to the loopback address
	// the inet_pton function converts a character string ip address to an in_addr network address struct
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}

	// assign an identity (sockaddr struct) to the local socket created above
    // gives the socket an IP address and port interface to bind to, on which it will listen for incoming datagrams
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
	// return the file descriptor number of the local socket   
    return sfd;
}