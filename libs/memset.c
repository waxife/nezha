#include <string.h>

void *
memset (void *s, int c, int n)
{
	char *ptr = (char *) s;

	while (n--)
		*ptr++ = c;

	return s;
}
