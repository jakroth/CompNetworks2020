#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define BUFFER_SIZE 1000

void usage(const char *str){
    fprintf(stderr, "Usage: %s [-t nsecs] [-n] [-i filename] name\n", str);
}

int main(int argc, char *argv[]){
    const char *TASK = "./task2";
    int flags, fflags, opt;
    int nsecs, tfnd, fd, n;
    char *filename = "";
    char *buffer = "";
    fd = 0;
    n = 0;
    nsecs = 0;
    tfnd = 0;
    flags = 0;
    fflags = 0;

    if (argc < 2){
        usage(TASK);
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "nt:i:")) != -1){
        switch (opt){
        case 'n':
            flags = 1;
            break;
        case 't':
            nsecs = atoi(optarg);
            tfnd = 1;
            break;
        case 'i':
            filename = optarg;
            fflags = 1;
            break;
        case '?':
            usage(TASK);
            exit(EXIT_FAILURE);
        default:
            usage(TASK);
            exit(EXIT_FAILURE);
        }
    }
    printf("flags=%d; tfnd=%d; nsecs=%d; fflags=%d; optind=%d\n", flags, tfnd, nsecs, fflags, optind);

    if (optind >= argc){
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    printf("name argument = %s\n", argv[optind]);

    if (fflags){
        printf("filename argument = %s\n", filename);
        if((fd = open(filename, O_RDONLY)) == -1){
            fprintf(stderr, "Enter a valid filename\n");
            exit(EXIT_FAILURE);
        }
        
        buffer = (char *)malloc(BUFFER_SIZE); 
        if (buffer == NULL){
            exit(1);
        }

        while((n = read(fd, buffer, BUFFER_SIZE)) > 0){                         
            write(STDIN_FILENO, buffer, n); 
        }
        printf("\n");

        free(buffer);
        buffer = NULL;

        close(fd);
    }
    else {
        printf("No file submitted\n");
    }
    exit(EXIT_SUCCESS);
}