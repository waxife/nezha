#include <stdio.h>

#include "dwunion.h"

/*
 *  * Convert (signed) quad to double.
 *
 */
__mips32__
double
__floatdidf(long long x)
{
      double d; 
      DWunion u; 
      int neg;
      /*                   
       * Get an unsigned number first, by negating if necessary.        
       */
      if (x < 0)
          u.ll = -x, neg = 1;
      else
          u.ll = x, neg = 0;

      /* 
       * Now u.ul[H] has the factor of 2^32 (or whatever) and u.ul[L]
       * has the units.  Ideally we could just set d, add INT_BITS to
       * its exponent, and then add the units, but this is portable
       * code and does not know how to get at an exponent.  Machine-
       * specific code may be able to do this more efficiently.
       */
      d = (double)u.u.high * (((int)1 << (32 - 2)) * 4.0);
      d += u.u.low;

      return (neg ? -d : d);
}

#if 0
#include <stdio.h>
int main()
{
    double d;
    long long u = 12345678901234ll;
    d = u;
    printf("d %lf %lf\n", d, __floatdidf(u));
    return 0;
}
#endif

