#include <string.h>

void
*memcpy (void *_dst, const void *_src, int n)
{
	int i;
	char *dst = (char *) _dst;
	char *src = (char *) _src;

	if ((unsigned int) dst < (unsigned int) src) {
        if (((unsigned int)dst & 0x03) == 0 && ((unsigned int)src & 0x03) == 0 && n >= 16) {
            long *ld = (long *)dst;
            long *ls = (long *)src;
            for (; n >= 4; n-=4) {
                *ld++ = *ls++;
            }    
            dst = (char *)(ld);
            src = (char *)(ls);
        }

		for (i = 0; i < n; i++)
			dst[i] = src[i];
	} else
	if ((unsigned int) dst > (unsigned int) src) {
        long *ld = (long *)(dst+n-4);
        long *ls = (long *)(src+n-4);

        if (((unsigned int)ld & 0x03) == 0 && ((unsigned int)ls & 0x03) == 0 && n >= 16) {
            for (; n >= 4; n-=4) {
                *ld-- = *ls--;
            }    
        }

		for (i = n - 1; i >= 0; i--)
			dst[i] = src[i];
	}

	return _dst;
}

#if 0

char mem1[32] = "abcdefghijklmno1234567890ABCDEF\0";
char mem2[32] = "\0";
char mem3[32] = "ABCDEFGHIJKLMNO1234567890abcdef\0";

int main () {
	printf ("mem1(%p) -> mem2(%p): %s\n",
		mem1, mem2, (char *) memcpy (mem2, mem1, 32));

    memset(mem2, 0, 32);

	printf ("mem1(%p) -> mem2(%p): %s\n",
		mem1, mem2, (char *) memcpy (mem2, mem1, 30));

	
	printf ("mem3(%p) -> mem2(%p): %s\n",
		mem3, mem2, (char *)memcpy (mem2, mem3, 32));



	printf ("mem2+4(%p) -> mem2(%p): %s\n",
		mem2+8, mem2, (char *)memcpy (mem2, mem2+4, 32));

	printf ("mem2(%p) -> mem2+4(%p): %s\n",
		mem2, mem2+4, (char *)memcpy (mem2+4, mem2, 32));


}
#endif
