#include <stdio.h>
#include "dwunion.h"

#define	ONE_FOURTH	((int)1 << (32 - 2))
#define	ONE_HALF	(ONE_FOURTH * 2.0)
#define	ONE		(ONE_FOURTH * 4.0)
#define UQUAD_MAX   0xffffffffffffffffll


/*
 * Convert double to (unsigned) quad.
 * Not sure what to do with negative numbers---for now, anything out
 * of range becomes UQUAD_MAX.
 */
__mips32__
unsigned long long
__fixunsdfdi(double x)
{
	DWunion t;
	unsigned int tmp;
myputs("__fixunsdfdi");
	if (x < 0)
		return (unsigned long long)(UQUAD_MAX);	/* ??? should be 0?  ERANGE??? */

	if (x >= 18446744073709551615.0)	/* XXX */
		return (unsigned long long)(UQUAD_MAX);
	/*
	 * Now we know that 0 <= x <= 18446744073709549568.  The upper
	 * limit is one ulp less than 18446744073709551615 tested for above.
	 * Dividing this by 2^32 will *not* round irrespective of any
	 * rounding modes (except if the result is an IEEE denorm).
	 * Furthermore, the quotient will fit into a 32-bit integer.
	 */
	tmp = x / ONE;
	t.u.low = (unsigned int) (x - tmp * ONE);
	t.u.high = tmp;
	return (t.ll);
}

#if 0
int main()
{
    unsigned long long l;
    double d = 1234567890123.123;
    l = d;

    printf("%lld %lld\n", l, __fixunsdfdi(d));

    d = 1e23;
    l = d;

    printf("%llu %llu\n", l, __fixunsdfdi(d));
    return 0;
}
#endif
