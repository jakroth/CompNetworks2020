#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#define BUFFER_SIZE 1000

int main(int argc, char * argv[]){

    int fd = 0; // file descriptor
    int c = 0; // command line option
    char * buffer = "";
    char * filename = "";
    int index = 0;
    int n = 0;
    
    if(argc < 2) {
        printf("Usage: ./task3b -f <FILE>\n");
        exit(1);
    }
    while((c = getopt(argc, argv, ":f:")) != -1){
        switch (c){  
            case 'f':
                filename = optarg;
                break; 
            case ':':  
                printf("option needs a value\n");           // when no argument is passed with an option requiring an argument
                break;  
            case '?':  
                printf("unknown option: %c\n", optopt);     // getopt stores error options (unknown, no argument) in optopt
                break;  
        }
    }  
    for (index = optind; index < argc; index++){            // optind is current argument number, increments with each getopt
        printf ("Non-option argument: %s\n", argv[index]);  // this will print any excess arguments
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;        // this mode sets the user access conditions for a newly created file
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, mode);    // write only open, create if doesn't exist, truncate existing file to 0

    buffer = (char *)malloc(BUFFER_SIZE);       // create buffer with dynamic memory allocation
    if(buffer == NULL){
        exit(1);    
    }
    
    while((n = read(STDIN_FILENO,buffer,BUFFER_SIZE)) > 0){     // read up to 1000 characters from stdin into buffer
        write(fd,buffer,n);                                     // write up to n characters into <filename>
    }
                   
    free(buffer);
    buffer = NULL;

    close(fd);

    return 0;
}