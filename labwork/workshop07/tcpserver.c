/*
 * File : tcpserver.c
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
/* Client queue size*/
#define BACKLOG 1

/* Command to generate a test file (uses Linux' built in randomiser and the dd command)
 * dd if=/dev/urandom of=fichier count=8
 */

/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_server_socket (int port);

// Structs to hold details about the server and client socket details
struct sockaddr_in sock_serv,sock_clt;

int main(int argc,char** argv){
    int sfd,fd;
    unsigned int length=sizeof(struct sockaddr_in);
    long long count = 0, n;
    long m;
    unsigned int nsid;
    ushort clt_port;
    char buffer[BUFFER],filename[256];
    char dst[INET_ADDRSTRLEN];
    
    // variables to save the date
	time_t intps;
	struct tm* tmi;

    // checks there are exactly 2 arguments provided on the command line
    if(argc!=2) {
        printf("Usage error: %s <port_serv>\n",argv[0]);
        return EXIT_FAILURE;
    }
    
   	// calls the create server socket function, using the port address of the server 
	// and stores the returned socket file descriptor in sfd
    sfd = create_server_socket(atoi(argv[1]));
    
    // bzero sets all the bytes in the buffer to zero
    bzero(buffer,BUFFER);
    // market the local socket as a listening/passive socket, for incoming requests (using accept)
    listen(sfd,BACKLOG);
    // accepts the first connection to a listening (tcp) socket, setting up another socket to receive data
    // the new socket is saved in nsid
    // adds the client ip address and port number from the connection to the client sockaddr_in struct
    nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);
    // check for a failed accept
    if(nsid==-1){
        perror("accept fail");
        return EXIT_FAILURE;
    }
    else {
        // converts the ip address of the sending client server from the network address sockaddr_in struct and stores in the dst buffer
	    // the inet_pton function converts a character string ip address to an in_addr network address struct 
        if(inet_ntop(AF_INET,&sock_clt.sin_addr,dst,INET_ADDRSTRLEN)==NULL){
            perror("socket error");
            exit (4);
        }
        // gets the sending client port number from the sockaddr_in struct and stores in clt_port
        clt_port=ntohs(sock_clt.sin_port);
        // print out the details of the client server from which the connection originated
        printf("Connection started with: %s:%d\n",dst,clt_port);
        
        // Setting up a file to save transmitted data, labelling with the current time
        // grab the seconds since the Epoch (1970)
        intps = time(NULL);
        // convert those seconds to a tm struct called tmi
        tmi = localtime(&intps);
        // assign the time info into the string filename
        sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
        // print filename
        printf("Creating an output file : %s\n",filename);
        
        // open filename (create it first) in write only mode, but make the file read/write for the user
        // the file is opened on file descriptor fd
        if ((fd=open(filename,O_CREAT|O_WRONLY,0600))==-1)
        {
            perror("open fail");
            exit (3);
        }
        // preparing to receive packets
        // bzero sets all the bytes in the buffer to zero
        bzero(buffer,BUFFER);
        // reads BUFFER bytes from nsid into buffer
        n=recv(nsid,buffer,BUFFER,0);
        // while there were bytes to read
        while(n) {
            // error check
            if(n==-1){
                perror("recv fail");
                exit(5);
            }
            // print the number of bytes received
            printf("%lld bytes of data received \n",n);
            //write the bytes from buffer into the file fd
            if((m=write(fd,buffer,n))==-1){
                perror("write fail");
                exit (6);
            }
            // cumulative count of bytes read and written
            count=count+m;
            // clear the buffer
            bzero(buffer,BUFFER);
            // read the next packet from the connection socket
            n=recv(nsid,buffer,BUFFER,0);
        }
        // close the listening socket and the file socket
        close(sfd);
        close(fd);
    }
    // close the connection socket
    close(nsid);
    
    // print the close of the connection and the number of bytes received
    printf("End of the connection with: %s.%d\n",dst,clt_port);
    printf("Total number of bytes received: %lld \n",count);
    
    return EXIT_SUCCESS;
}

/* Function allowing the creation of a socket and its attachment to the system
 * Returns a file descriptor in the process descriptor table
 * bind() allows its definition with the system
 */
int create_server_socket (int port){
    int l;
	int sfd;
    int yes=1;

    // *****setting up the local socket
	// uses the ip4 address family and the TCP connection-oriented sock stream (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for use in main
	sfd = socket(AF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_SUCCESS;
	}

    /* Change some socket options for the listening socket
     * SOL_SOCKET: To manipulate options at the sockets API level
     * SO_REUSEADDR: When you have to restart a server after a hard stop this can be useful
     * not to have an error when creating the socket (the IP stack of the system has not
     * always had time to clean up).
     * Multiple servers listening on the same port... (?)
     */
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(int)) == -1 ) {
        perror("setsockopt error");
        exit(5);
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