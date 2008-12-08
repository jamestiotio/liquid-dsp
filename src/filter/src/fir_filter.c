//
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filter.h"
#include "../../dotprod/src/dotprod.h"
#include "../../buffer/src/window.h"

// defined:
//  FIR_FILTER  name-mangling macro
//  T           coefficients type
//  W           window type
//  DOTPROD     dot product API

#define FIR_FILTER(name)    FILTER_CONCAT(fir_filter,name)
#define T                   float
#define WINDOW(name)        FILTER_CONCAT(fwindow,name)
#define DOTPROD(name)       FILTER_CONCAT(fdotprod,name)

struct FIR_FILTER(_s) {
    T * h;
    unsigned int h_len;

    WINDOW() w;

    fir_prototype p;
};

FIR_FILTER() FIR_FILTER(_create)(T * _h, unsigned int _n)
{
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (T *) malloc((f->h_len)*sizeof(T));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

    f->w = WINDOW(_create)(f->h_len);
    WINDOW(_clear)(f->w);

    return f;
}

FIR_FILTER() FIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (T *) malloc((f->h_len)*sizeof(T));

    // use remez here

    return f;
}

void FIR_FILTER(_destroy)(FIR_FILTER() _f)
{
    WINDOW(_destroy)(_f->w);
    free(_f->h);
    free(_f);
}

void FIR_FILTER(_print)(FIR_FILTER() _f)
{
    printf("filter coefficients:\n");
    //unsigned int i, n = _f->h_len;
    //for (i=0; i<n; i++)
    //    printf(" h(%u) = %12.4E;\n", i+1, _f->h[n-i-1]);
}

void FIR_FILTER(_push)(FIR_FILTER() _f, T _x)
{
    WINDOW(_push)(_f->w, _x);
}

void FIR_FILTER(_execute)(FIR_FILTER() _f, T *_y)
{
    T *r;
    WINDOW(_read)(_f->w, &r);
    *_y = DOTPROD(_run)(_f->h, r, _f->h_len);
}

unsigned int FIR_FILTER(_get_length)(FIR_FILTER() _f)
{
    return _f->h_len;
}

