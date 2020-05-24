#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void usage(char *str)
{
    fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", str);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage("./task1");
        exit(EXIT_FAILURE);
    }

    int flags, opt;
    int nsecs, tfnd;
    nsecs = 0;
    tfnd = 0;
    flags = 0;
    while ((opt = getopt(argc, argv, "nt:i:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            flags = 1;
            break;
        case 't':
            nsecs = atoi(optarg);
            tfnd = 1;
            break;
        // ? is returned by getopt if option character is not in opstring, or option had no argument that required one
        // seems a but redunant, will get picked up by default. better to use : at start of opstring to separate behaviours
        // a default error message is printed on stderr immediately if a ? is returned from getopt
        case '?':
            usage("./task1");
            exit(EXIT_FAILURE);
        default:
            usage("./task1");
            exit(EXIT_FAILURE);
        }
    }
    printf("flags=%d; tfnd=%d; nsecs=%d; optind=%d\n", flags, tfnd, nsecs, optind);

    if (optind >= argc)
    {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    printf("name argument = %s\n", argv[optind]);

    exit(EXIT_SUCCESS);
}