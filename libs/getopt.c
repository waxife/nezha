#include <stdio.h>
#include <string.h>

char *optarg = NULL;
int optind = 0;
int optopt = 0;

int
getopt (int argc, char *argv[], char *optstring)
{
    char *optchr;

    if (optind == 0)
        optind++;

    while (optind < argc) {
        if (argv[optind][0] == '-') {
            optopt = argv[optind++][1];
            optchr = strchr (optstring, optopt);
            if (optchr != NULL) {
                if (optchr[1] == ':') {
                    optarg = argv[optind++];
                }
                return optopt;
            } else {
                return '?';
            }
        } else
        if (optstring[0] == '-') {
            optarg = argv[optind++];
            return 1;
        } else {
            optind++;
        }
    }

    return -1;
}

#if 0
void
test_getopt (int argc, char *argv[])
{
    char c;
    int i;

    printf ("\ngetopt(%d", argc);
    for (i = 0; i < argc; i++)
        printf (", \"%s\"", argv[i]);
    printf (")\n");
    optind = 1;

    while ((c = getopt (argc, argv, "-ab:")) != -1) {
        switch (c) {
            case 'a':
                printf ("option %c\n", c);
                break;

            case 'b':
                printf ("option %c with value '%s'\n", c, optarg);
                break;

            case '?':
                printf ("undefined option '%c'\n", optopt);
                break;

            case 1:
                printf ("non-option argument: %s\n", optarg);
                break;

            default:
                printf ("?? getopt returned character code 0%o ??\n", c);
                break;
        }
    }
}

#define SIZEOF(ARGV) (sizeof(ARGV) / sizeof(ARGV[0]))
int
main ()
{
    char *argv1[] = {"", "-a"};
    test_getopt (SIZEOF(argv1), argv1);
    // option a

    char *argv2[] = {"", "-b", "123"};
    test_getopt (SIZEOF(argv2), argv2);
    // option b with value '123'

    char *argv3[] = {"", "-c"};
    test_getopt (SIZEOF(argv3), argv3);
    // undefined option 'c'

    char *argv4[] = {"", "xxx"};
    test_getopt (SIZEOF(argv4), argv4);
    // non-option argument: xxx

    char *argv5[] = {"", "-a", "-b", "123"};
    test_getopt (SIZEOF(argv5), argv5);
    // option a
    // option b with value '123'

    char *argv6[] = {"", "-b", "123", "-a"};
    test_getopt (SIZEOF(argv6), argv6);
    // option b with value '123'
    // option a

    char *argv7[] = {"", "-a", "-b", "123", "-c"};
    test_getopt (SIZEOF(argv7), argv7);
    // option a
    // option b with value '123'
    // undefined option 'c'

    char *argv8[] = {"", "-a", "-b", "123", "-c", "xxxx"};
    test_getopt (SIZEOF(argv8), argv8);
    // option a
    // option b with value '123'
    // undefined option 'c'
    // non-option argument: xxx

    return 0;
}
#endif
