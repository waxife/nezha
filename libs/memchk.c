#include <string.h>

#define MIN(A, B)   ((A) < (B) ? (A) : (B))
    
int
memchk (void *mem, char c, int n)
{
    char *ptr = (char *)mem;
    int len;

    while (((unsigned int)ptr % 4) && n-- > 0) {
        if (*ptr++ != c)
            return -1;
    }

    len = n / 4;
    unsigned int *ptr4 = (unsigned int *)ptr;
    unsigned int dword = c | (c << 8) | (c << 16) | (c << 24);
    while (n >= 4) {
        if (*ptr4++ != dword)
            return -1;
        n -= 4;
    }

    while (n-- > 0) {
        if (*ptr++ != c)
            return -1;
    }

    return 0;
}

#if 0
#include <stdio.h>
#define VERIFY_MEMCHK(SRC, BYTE, SIZE) printf ("memchk(0x%02x, 0x%02x, %d) >> %d\n", ((unsigned int)SRC) % 4, BYTE, SIZE, memchk (SRC, BYTE, SIZE)) 

int
main (int argc, char *argv[])
{
    char buf[32];
    memset (buf, 0x55, 32);

    VERIFY_MEMCHK (buf + 0, 0x56, 32);
    VERIFY_MEMCHK (buf + 2, 0x56, 30);
    VERIFY_MEMCHK (buf + 2, 0x55, 1);
    VERIFY_MEMCHK (buf + 2, 0x55, 4);
    VERIFY_MEMCHK (buf + 2, 0x55, 8);
    VERIFY_MEMCHK (buf + 2, 0x55, 28);
    VERIFY_MEMCHK (buf + 0, 0x55, 32);

    return 0;
}
#endif
