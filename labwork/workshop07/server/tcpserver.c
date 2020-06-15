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

/* Declaration of functions*/
int create_server_socket (int port, char *ipaddr);

// Structs to hold details about the server and client socket details
struct sockaddr_in sock_serv,sock_clt;

int main(int argc,char** argv){
    // file descriptors
	int fd, sfd, port, opt, flag = 0, conn_count = 1;
	long long count = 0, n, sz = 0;
	off_t m; // long type
	char buffer[BUFFER], filename[BUFFER], rtn_string[64];
    unsigned int length=sizeof(struct sockaddr_in);
	char *ip_address = "127.0.0.1";
    unsigned int nsid;
    ushort clt_port;
    char clt_ip[INET_ADDRSTRLEN];
    char *status = "OK";
	struct stat filestat;

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
		port = 45678;
	}

	// checks that the port number is in the appropriate range, ports above 1024 can be accessed in user space
	if(port < 40000 || port > 59999){
		printf("Usage error: port_serv must be between 10000-59999\n");
		return EXIT_FAILURE;
	}

	printf("Listening on IP: %s and port: %d\n",ip_address,port);
    

	// loop to accept multiple connections
	while(1){

        // calls the create server socket function, using the port address of the server 
        // and stores the returned socket file descriptor in sfd
        sfd = create_server_socket(port, ip_address);

        // mark the local socket as a listening/passive socket, waiting for incoming requests (using accept)
        listen(sfd,BACKLOG);

        // program waits here and accepts the first connection to a listening (tcp) socket, 
        // setting up another socket to receive data
        // the new socket is saved in nsid
        // adds the client ip address and port number from the connection to the client sockaddr_in struct
        nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);

        // check for a failed accept
        if(nsid==-1){
            perror("accept fail");
            return EXIT_FAILURE;
        }
        
        // gets the sending client ip address from the sockaddr_in struct and stores it in clt_ip
        // the inet_ntop function converts an in_addr network address struct to a character string ip address
        if(inet_ntop(AF_INET,&sock_clt.sin_addr,clt_ip,INET_ADDRSTRLEN)==NULL){
            perror("socket error");
            exit(4);
        }
        // gets the sending client port number from the sockaddr_in struct and stores it in clt_port
        clt_port=ntohs(sock_clt.sin_port);


        // ***preparing to receive packets
        // bzero sets all the bytes in the filename to zero
        bzero(filename,BUFFER);

        // reads BUFFER bytes from nsid into filename
        // this should now contain the name of the file requested by the client
        n=recv(nsid,filename,BUFFER,0);
        if(n==-1){
            perror("receive filename error");
            return EXIT_FAILURE;
        }

        // opens the file containing the text to be sent
        // if unsuccessful, sets status to Failed
        if((fd = open(filename,O_RDONLY))==-1){
            status = "Failed";
        }
        
        // obtains information about the input file and saves it in a stat struct
        // sets the file size variable from the struct
        if(stat(filename,&filestat)==-1){
            sz = 0;
        }
        else{
            sz=filestat.st_size;
        }

        // convert the filename and file size into a single string, separated by a colon, to be returned
        sprintf(rtn_string,"%s:%lld",filename,sz);
        //printf("%s\n",rtn_string);

        // return the filename and size of the requested file to the client
        // if the requested file doesn't exist, this will be the non-existant file name and 0 bytes for size
        // nsid is the server side socket of the connection, rtn_string is the data to send
        // because this socket is in a connected state, don't need to specify sendto address
        m=send(nsid,rtn_string,sizeof(rtn_string),0);
        if(m==-1){
            perror("send filename confirmation error");
            return EXIT_FAILURE;
        }

        // *** will only continue from here if the file exists
        if(sz != 0){
            // bzero sets all the bytes in the buffer to zero
            bzero(buffer,BUFFER);

            // grab the confirmation file size from the client
            // reads BUFFER bytes from nsid into buffer
            // this should now contain the size of the file requested by the client, as returned by the client
            n=recv(nsid,buffer,BUFFER,0);
            if(n==-1){
                perror("receive filesize confirmation error");
                return EXIT_FAILURE;
            }

            // check that the filesize returned from the client matches the actual filesize
            if(atoll(buffer) != sz){
                perror("filesize mismatch from client");
                return EXIT_FAILURE;
            } 

            // *** start sending the file to the client, as all checks passed to this point
            // bzero sets all the bytes in the buffer to zero
            bzero(buffer,BUFFER);  

            // read data from the local file
            n=read(fd,buffer,BUFFER);
            // continue sending data one BUFFER at a time, until all sent
            while(n>0){
                // check for read errors
                if(n==-1){
                    perror("file read fail");
                    return EXIT_FAILURE;
                }
                // print what's in the buffer each time
                //printf("%s\n", buffer);
                // send the buffer of data to the client server, via the local nsid socket
                // m will hold the number of bytes sent each time
                // won't continue till it receives something back
                m=send(nsid,buffer,n,0);
                if(m==-1){
                    perror("payload send error");
                    return EXIT_FAILURE;
                }
                // add to the total bytes sent
                count+=m;
                // zero everything in the buffer
                bzero(buffer,BUFFER);
                // read the next section of the file into the buffer
                n=read(fd,buffer,BUFFER);
            }
            // read has returned 0 = end of file

            // unlock the client by sending 0 bytes, signifying end of file
            send(nsid,buffer,0,0);
            

            // *** check that the client received all the data properly
            // bzero sets all the bytes in the buffer to zero
            bzero(buffer,BUFFER);

            // reads BUFFER bytes from nsid into buffer
            // this will contain "OK" if client received all data properly, otherwise "Failed"
            n=recv(nsid,buffer,BUFFER,0);
            if(n==-1){
                perror("receive payload confirmation error");
                return EXIT_FAILURE;
            }

            // set status to the final message received from the client
            status = buffer;
        }
        // close the listening socket and the file socket
        close(sfd);
        close(fd);
        
        // close the connection socket
        close(nsid);

		// print the connection number, connection ip and port, file retrieved, file size and status, IP address and port number of client
		printf("Connection: \"%d\", client: \"%s:%d\", file: \"%s\", size: \"%lld\", status: \"%s\"\n",conn_count,clt_ip,clt_port,filename,sz,status);

		// reset the data counter and connection loop counter
		conn_count++;
		count = 0;

        // reset the file size
        sz = 0;
        
		// sleep for 1 second to make sure the server is ready to receive the next connection
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
    int yes=1;

    // *****setting up the local socket
    // this sets up a socket, the first step in allowing a connection
	// uses the ip4 address family and the TCP connection-oriented sock stream (and the default protocol, 0)
	// assigns a file descriptor integer to sfd, for use in main
	sfd = socket(AF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}

    /* Change some socket options for the listening socket
     * SOL_SOCKET: To manipulate options at the sockets API level
     * SO_REUSEADDR: When you have to restart a server after a hard stop this can be useful
     * not to have an error when creating the socket (the IP stack of the system has not
     * always had time to clean up).
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

	// assign the identity (sockaddr_in struct) to the local socket created above
    // gives the socket an IP address and port interface to bind to, 
    // on which it will listen for incoming connections/packets
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
	// return the file descriptor number of the local socket    
    return sfd;
}