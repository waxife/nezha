/**
 *  @file   div64.c
 *  @brief  64bit/32bit signed/unsigned dividor and module
 *  $Id: div64.c,v 1.4 2014/02/07 02:23:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  jedy      New file.
 *
 */
#include <stdio.h>


int nlz64(unsigned long long x) {
   int n;

   if (x == 0) return(64);
   n = 0;
   if (x <= 0x00000000FFFFFFFFLL) {n = n + 32; x = x << 32;}
   if (x <= 0x0000FFFFFFFFFFFFLL) {n = n + 16; x = x << 16;}
   if (x <= 0x00FFFFFFFFFFFFFFLL) {n = n +  8; x = x <<  8;}
   if (x <= 0x0FFFFFFFFFFFFFFFLL) {n = n +  4; x = x <<  4;}
   if (x <= 0x3FFFFFFFFFFFFFFFLL) {n = n +  2; x = x <<  2;}
   if (x <= 0x7FFFFFFFFFFFFFFFLL) {n = n +  1;}
   return n;
}

// ---------------------------- udivdi3 --------------------------------

   /* The variables u0, u1, etc. take on only 32-bit values, but they
   are declared long long to avoid some compiler warning messages and to
   avoid some unnecessary EXTRs that the compiler would put in, to
   convert long longs to ints.

   First the procedure takes care of the case in which the divisor is a
   32-bit quantity. There are two subcases: (1) If the left half of the
   dividend is less than the divisor, one execution of DIVU is all that
   is required (overflow is not possible). (2) Otherwise it does two
   divisions, using the grade school method, with variables used as
   suggested below.

       q1 q0
    ________
   v)  u1 u0
     q1*v
     ____
        k u0   */

/* These macros must be used with arguments of the appropriate type
(unsigned long long for DIVU and long long for DIVS. They are
simulations of the presumed machines ops. I.e., they look at only the
low-order 32 bits of the divisor, they return garbage if the division
overflows, and they return garbage in the high-order half of the
quotient doubleword.
   In practice, these would be replaced with uses of the machine's DIVU
and DIVS instructions (e.g., by using the GNU "asm" facility). */
#if 0
#define DIVU(u, v) ({unsigned long long __v = (v) & 0xFFFFFFFF; \
   long long __q = (u)/(v); \
   __q > 0xFFFFFFFF ? 0xdeaddeadbeefbeefll : \
   __q | 0xdeadbeef00000000ll;})
#define DIVS(u, v) ({long long __v = (v) << 32 >> 32; \
   long long __q = (u)/(__v); \
   __q < (long long)0xFFFFFFFF80000000ll || \
   __q > (long long)0x000000007FFFFFFFll ? \
   0xfadefadedeafdeafll : __q | 0xfadedeaf00000000ll;})
#endif


unsigned long long __udivdi3(unsigned long long u, unsigned long v)
{
    unsigned long u1, u0;
    unsigned long q1;
    unsigned long k, n; 
    unsigned long long q = 0;
    

    while(u >= v) {
        n = nlz64(u);
        u = (u << n);
        u1 = u >> 32;
        u0 = u & 0xffffffff;
        if (u1 >= v) {
            q1 = u1 / v ;
            k  = u1 - q1*v;
            q += (((unsigned long long)q1 << 32) >> n);
            u = (((unsigned long long)k << 32) + u0) >> n;
        } else { /* v must > 0x7fffffff */
            u0 = u & 0x7fffffff;
            u = u >> 31;
            q1 = 1;
            k = u - v;
            q += ((unsigned long long)q1 << 31) >> n;
            u = (((unsigned long long)k << 31) + u0) >> n;
        }
    }
    

    return q; 
}
#define llabs(x) \
({unsigned long long t = (x) >> 63; ((x) ^ t) - t;})

#define abs(x) \
({unsigned long t = (x) >> 31; ((x)^t) -t; })

long long __divdi3(long long u, long v)
{
   unsigned long long au;
   unsigned long av;
   long long q;
   long t;

   au = llabs(u);
   av = abs(v);
   q = __udivdi3(au, av);
   t = ((u >> 32) ^ v) >> 31;

   return (q ^ t) - t;
}


unsigned long __umoddi3(unsigned long long u, unsigned long v)
{
    unsigned long long q;
    unsigned long r;
    q = __udivdi3(u, v);

    r = (unsigned long)(u - (q * v));

    return r;
}

long __moddi3(long long u, long v)
{
    long long q;
    long r;
    q = __divdi3(u, v);
    r = (long)(u - (q *v));

    return r;
}

 
#if 0
int main()
{
   /* First test unsigned division. */

   /* The entries in this table are used in all combinations. */

   static unsigned long long tabu[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
      0x7000000000000000ll, 0x7000000080000000ll, 0x7000000080000001ll,
      0x7fffffffffffffffll, 0x7fffffff8fffffffll, 0x7fffffff8ffffff1ll,
      0x7fffffff00000000ll, 0x7fffffff80000000ll, 0x7fffffff00000001ll,
      0x8000000000000000ll, 0x8000000080000000ll, 0x8000000080000001ll,
      0xc000000000000000ll, 0xc000000080000000ll, 0xc000000080000001ll,
      0xfffffffffffffffdll, 0xfffffffffffffffell, 0xffffffffffffffffll,
      };

   /* The entries in this table are used in all combinations, with
   + and - signs preceding them. */

   static long long tabs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
      0x7000000000000000ll, 0x7000000080000000ll, 0x7000000080000001ll,
      0x7fffffffffffffffll, 0x7fffffff8fffffffll, 0x7fffffff8ffffff1ll,
      0x7fffffff00000000ll, 0x7fffffff80000000ll, 0x7fffffff00000001ll,
      0x8000000000000000ll, 0x8000000080000000ll, 0x8000000080000001ll,
      0x0123456789abcdefll, 0x00000000abcdef01ll, 0x0000000012345678ll,
      };

   int n, i, j, k, errs = 0;
   unsigned long long uu, vu, qu, ru;
   long long u, v, q, r;

   printf("Testing unsigned division.\n");
   n = sizeof(tabu)/sizeof(tabu[0]);
   for (i = 0; i < n; i++) {
      for (j = 1; j < n; j++) {         // Skip tabu[0], which is 0.
         uu = tabu[i];
         vu = tabu[j];
         qu = udivdi3(uu, vu);          // Call the program being tested.
         ru = uu - qu*vu;
         if (qu > uu || ru >= vu) {
            printf("Error for %016llx/%016llx, got %016llx rem %016llx\n",
               uu, vu, qu, ru);
            errs = errs + 1;
         }
      }
   }
   if (errs == 0) printf("Passed all %d tests (unsigned).\n", n*(n-1));
   else printf("Failed %d cases (unsigned).\n", errs);
   if (errs != 0) return errs;

   /* Now test signed division. */

   printf("Testing signed division.\n");
   n = sizeof(tabs)/sizeof(tabs[0]);
   for (i = 0; i < n; i++) {
      for (j = 1; j < n; j++) {         // Skip tabs[0], which is 0.
         u = tabs[i];
         v = tabs[j];
         for (k = 0; k <= 3; k++) {     // Do all combinations of +, -.
            if (k & 1)  u = -u;
            if (k >= 2) v = -v;
            q = divdi3(u, v);           // Call the program being tested.
            r = u - q*v;
            if (llabs(q) > llabs(u) ||
               llabs(r) >= llabs(v) ||
               (r != 0 && (r ^ u) < 0)) {
               printf("Error for %016llx/%016llx, got %016llx rem %016llx\n",
                  u, v, q, r);
               errs = errs + 1;
            }
         }
      }
   }
   if (errs == 0) printf("Passed all %d tests (signed).\n", n*(n-1)*4);
   else printf("Failed %d cases (signed).\n", errs);
   return errs;
}
#endif

#if 0 
int main()
{
   static unsigned long long tabu[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
      0x7000000000000000ll, 0x7000000080000000ll, 0x7000000080000001ll,
      0x7fffffffffffffffll, 0x7fffffff8fffffffll, 0x7fffffff8ffffff1ll,
      0x7fffffff00000000ll, 0x7fffffff80000000ll, 0x7fffffff00000001ll,
      0x8000000000000000ll, 0x8000000080000000ll, 0x8000000080000001ll,
      0xc000000000000000ll, 0xc000000080000000ll, 0xc000000080000001ll,
      0xfffffffffffffffdll, 0xfffffffffffffffell, 0xffffffffffffffffll,
      };
   static unsigned long tabv[] = {1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
   };

   /* The entries in this table are used in all combinations, with
   + and - signs preceding them. */

   static long long tabs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
      0x7000000000000000ll, 0x7000000080000000ll, 0x7000000080000001ll,
      0x7fffffffffffffffll, 0x7fffffff8fffffffll, 0x7fffffff8ffffff1ll,
      0x7fffffff00000000ll, 0x7fffffff80000000ll, 0x7fffffff00000001ll,
      0x8000000000000000ll, 0x8000000080000000ll, 0x8000000080000001ll,
      0x0123456789abcdefll, 0x00000000abcdef01ll, 0x0000000012345678ll,
      };

   static long long tabi[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 1000, 2003,
      32765, 32766, 32767, 32768, 32769, 32760,
      65533, 65534, 65535, 65536, 65537, 65538,
      0x7ffffffe, 0x7fffffff, 0x80000000, 0x80000001,
   }; 
   
   int n, m, i, j, k, errs = 0;
   unsigned long long uu, qu;
   unsigned long vu, ru;
   long long u, q;
   long v, r;

   printf("Testing unsigned division.\n");
   n = sizeof(tabu)/sizeof(tabu[0]);
   m = sizeof(tabv)/sizeof(tabv[0]);
   for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++) {      
         uu = tabu[i];
         vu = tabv[j];
         qu = __udivdi3(uu, vu);          // Call the program being tested.
         ru = (unsigned long)(uu - qu*vu);
         if (qu > uu || ru >= vu || qu != (uu/vu)) {
            printf("Error for %016llx/%08lx, got %016llx rem %08lx\n",
               uu, vu, qu, ru);
            printf("          %016llx/%08lx, got %016llx rem %08lx\n", 
                uu , vu, uu/vu, (unsigned long)(uu%vu));
            errs = errs + 1;
         } else {
            //printf("Pass  for %016llx/%08lx, got %016llx rem %08lx\n",
            //   uu, vu, qu, ru);
         }
      }
   }
   if (errs == 0) printf("Passed all %d tests (unsigned).\n", n*m);
   else printf("Failed %d cases (unsigned).\n", errs);
   if (errs != 0) return errs;

   /* Now test signed division. */

   printf("Testing signed division.\n");
   n = sizeof(tabs)/sizeof(tabs[0]);
   m = sizeof(tabi)/sizeof(tabi[0]);
   for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++) {         // Skip tabs[0], which is 0.
         u = tabs[i];
         v = tabi[j];
         for (k = 0; k <= 3; k++) {     // Do all combinations of +, -.
            if (k & 1)  u = -u;
            if (k >= 2) v = -v;
            q = __divdi3(u, v);           // Call the program being tested.
            r = u - q*v;
            if (llabs(q) > llabs(u) ||
               abs(r) >= abs(v) ||  
               (r != 0 && (r ^ (u >> 32)) < 0) || 
               q != (u/v)) {
               printf("Error for %016llx/%08lx, got %016llx rem %08lx\n",
                  u, v, q, r);
               printf("          %016llx/%08lx, got %016llx rem %08lx\n", 
                  u , v, u/v, (long)(u%v));
               errs = errs + 1;
            }
         }
      }
   }
   if (errs == 0) printf("Passed all %d tests (signed).\n", n*(n-1)*4);
   else printf("Failed %d cases (signed).\n", errs);
   return errs;
}
#endif
