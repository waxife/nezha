#ifndef __DWUNION_H
#define __DWUNION_H

typedef long   word_type;
typedef union {
    long long  ll;
    struct {
        word_type   low;
        word_type   high;
    } s;
    struct {
        unsigned long low;
        unsigned long high;
    } u;
} DWunion;
#endif

