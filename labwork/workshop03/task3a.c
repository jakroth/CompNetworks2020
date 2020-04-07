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
    int index;
    
    if(argc < 2) {
        printf("Usage: ./task3a -f <FILE>\n");
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

    fd = open(filename, O_RDONLY);  // read only open

    buffer = (char *)malloc(BUFFER_SIZE);
    if(buffer == NULL){
        exit(1);    
    }
    
    read(fd,buffer,BUFFER_SIZE);

    write(STDOUT_FILENO,buffer,BUFFER_SIZE);

    free(buffer);
    buffer = NULL;

    close(fd);

    return 0;
}