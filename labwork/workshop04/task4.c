#include <unistd.h>     // posix library
#include <stdlib.h>     // standard library
#include <stdio.h>      // standard input/output
#include <fcntl.h>      // for open() constants
#include <sys/time.h>   // for gettimeofday()
#include <sys/types.h>  // for stat()
#include <sys/stat.h>   // for stat()
#include <time.h>       // for ctime()

void usage(const char *str){
    fprintf(stderr, "Usage: %s [-i filename]\n", str);
}

int main(int argc, char *argv[]){
    // I removed a bunch of variables and options that weren't being used anymore
    const char *TASK = "./task4";
    int flags, opt;
    char *filename;
    flags = 0;
    filename = "";

    while ((opt = getopt(argc, argv, "i:")) != -1){
        switch (opt){
        case 'i':
            filename = optarg;
            flags = 1;
            break;
        case '?':
            usage(TASK);
            exit(EXIT_FAILURE);
        default:
            usage(TASK);
            exit(EXIT_FAILURE);
        }
    }
    printf("flags=%d; optind=%d\n", flags, optind);

    if (flags){
        printf("filename argument = %s\n", filename);

        /* declare variables and get start time of operation */
        long totTime = 0;
        struct timeval t1;
        struct timeval t2;
        struct stat file;
        gettimeofday(&t1, NULL);
        
        // lots of the following section adapted from the stat() man page
        
        // check if stat returns an error (i.e. incorrect filename)
        if (stat(filename, &file) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        printf("ID of containing device:  [%lx,%lx]\n", (long)major(file.st_dev),(long) minor(file.st_dev));

        // switch on bitwise & on the bit mask S_IFMT and the filetype masks below
        printf("File type:                ");
        switch (file.st_mode & S_IFMT) {
            case S_IFBLK:  printf("block device\n");            break;
            case S_IFCHR:  printf("character device\n");        break;
            case S_IFDIR:  printf("directory\n");               break;
            case S_IFIFO:  printf("FIFO/pipe\n");               break;
            case S_IFLNK:  printf("symlink\n");                 break;
            case S_IFREG:  printf("regular file\n");            break;
            case S_IFSOCK: printf("socket\n");                  break;
            default:       printf("unknown?\n");                break;
        }

        // print all other details stored in the stat struct
        printf("I-node number:            %ld\n", (long) file.st_ino);
        printf("Mode:                     %lo (octal)\n", (unsigned long) file.st_mode);
        printf("Link count:               %ld\n", (long) file.st_nlink);
        printf("Ownership:                UID=%ld   GID=%ld\n", (long) file.st_uid, (long) file.st_gid);
        printf("Preferred I/O block size: %ld bytes\n", (long) file.st_blksize);
        printf("File size:                %lld bytes\n", (long long) file.st_size);
        printf("Blocks allocated:         %lld\n",(long long) file.st_blocks);

        // struct stat has three struct timespec elements: ctime, atime and mtime
        // these timespec structs hold time_t seconds and long nanoseconds which represent time since the "Epoch", in 1970. 
        // a macro defines calls to these structs as calls to the tv_sec time_t variable inside them
        // when passed to ctime, it converts the time_t variables into a date string (null terminated)
        printf("Last status change:       %s", ctime(&file.st_ctime));
        printf("Last file access:         %s", ctime(&file.st_atime));
        printf("Last file modification:   %s", ctime(&file.st_mtime));
            
        /* get finish time of operation */
        gettimeofday(&t2, NULL);

        /* calculate total time taken*/
        totTime = 1000000 * (t2.tv_sec - t1.tv_sec);
        totTime += (t2.tv_usec - t1.tv_usec); 

        /* display elapsed time for operations */
        printf("elapsed time (usecs): %ld\n",totTime);
    }
    else {
        printf("No file submitted\n");
        usage(TASK);
    }
    exit(EXIT_SUCCESS);
}