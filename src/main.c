#include <stdio.h>
#include <fcntl.h>
#include "YLZhashtable.h"
#include "bitio.h"
#include "YLZcompressor.h"
#include "YLZdecompressor.h"
#include <getopt.h>

#define VERSION "0.1.0"

static int verbose_flag;

void version()
{
    printf("Yalzi v%s, (C) 2012-2013 badnack@gmail.com nebirhos@aol.com pelldav@gmail.com\n", VERSION);
}

void usage()
{
    printf("Usage:\n  yalzi -[c|x] FILE [OUT_FILE]\n\n");
    printf("Options:\n");
    printf("  -c, [--compress=FILE]            # Compress FILE to OUT_FILE [default OUT_FILE: FILE.yz]\n");
    printf("  -x, [--extract=FILE]             # Extract from FILE to OUT_FILE\n");
    printf("  -v, [--verbose]                  # Print useless info\n");
    printf("  -h, [--help]                     # Show this help message and quit\n");
    printf("  -V, [--version]                  # Show Yalzi version number and quit\n");
}


int main(int argc, char *argv[])
{
    char *archive_path, *file_path = NULL;
    char mode = 0;
    BITIO* in_file, *out_file;
    int i;

    while (1) {
        static struct option long_options[] = {
            /* These options don't set a flag.
               We distinguish them by their indices. */
            {"compress", required_argument, 0, 'c'},
            {"extract",  required_argument, 0, 'x'},
            {"help",     no_argument,       0, 'h'},
            {"version",  no_argument,       0, 'V'},
            /* These options set a flag. */
            {"verbose",  no_argument,       &verbose_flag, 1},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        i = getopt_long(argc, argv, "c:x:vhV",
                        long_options, &option_index);

        /* Detect the end of the options. */
        if (i == -1)
            break;

        switch (i) {
        case 0:
            /* This option sets a flag. */
            break;

        case 'v':
            verbose_flag = 1;
            break;

        case 'c':
            mode = i;
            file_path = optarg;
            break;

        case 'x':
            mode = i;
            archive_path = optarg;
            break;

        case 'V':
            version();
            return 0;

        case 'h':
        case '?':
        default:
            /* getopt_long already printed an error message. */
            usage();
            return 0;
        }
    }

    /* Check options */
    if (verbose_flag) puts("Verbose flag is set");
    if (mode == 'c') {
        /* Check if ARCHIVE path is passed */
        if (optind < argc) {
            archive_path = argv[optind];
        }
        else {
            asprintf(&archive_path, "%s.yz", file_path);
        }
    }
    else if (mode == 'x') {
        /* Check if FILE path is passed */
        if (optind < argc) {
            file_path = argv[optind];
        }
        else {
            asprintf(&file_path, "%s.out", archive_path);
            char* extension = strstr(file_path, ".yz");
            if (extension)
                *extension = '\0';
        }
    }
    else {
        /* no mode specified */
        printf("Error: no mode specified!\n");
        usage();
        return -1;
    }
    if (verbose_flag) {
        printf("File path: %s\n", file_path);
        printf("Archive path: %s\n", archive_path);
    }


    /* Compression */
    if (mode == 'c') {
        if (verbose_flag) printf ("Compression mode\n");
        if ((in_file = bitio_open(file_path, O_RDONLY)) == NULL) {
            printf("Error: cannot open %s!\n", file_path);
            return -1;
        }
        if ((out_file = bitio_open(archive_path, O_WRONLY)) == NULL) {
            printf("Error: cannot open %s!\n", archive_path);
            return -1;
        }
        compress(in_file, out_file);
    }
    /* Decompression */
    else if (mode == 'x') {
        if (verbose_flag) printf ("Decompression mode\n");
        if ((in_file = bitio_open(archive_path, O_RDONLY)) == NULL) {
            printf("Error: cannot open %s!\n", archive_path);
            return -1;
        }
        if ((out_file = bitio_open(file_path, O_WRONLY)) == NULL) {
            printf("Error: cannot open %s!\n", file_path);
            return -1;
        }
        decompress(in_file, out_file);
    }

    bitio_close(in_file);
    bitio_close(out_file);
    return 0;
}
