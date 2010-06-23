/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// firfilt : finite impulse response (FIR) filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIRFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

// firfilt object structure
struct FIRFILT(_s) {
    TC * h;             // filter coefficients array [size; h_len x 1]
    unsigned int h_len; // filter length

    WINDOW() w;         // window object (internal buffer)
    DOTPROD() dp;       // dot product object

#if 0
    fir_prototype p;    // prototype object
#endif
};

// create firfilt object
//  _h      :   coefficients (filter taps) [size: _n x 1]
//  _n      :   filter length
FIRFILT() FIRFILT(_create)(TC * _h,
                           unsigned int _n)
{
    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: firfilt_xxxt_create(), filter length must be greater than zero\n");
        exit(1);
    }

    // create filter object and initialize
    FIRFILT() q = (FIRFILT()) malloc(sizeof(struct FIRFILT(_s)));
    q->h_len = _n;
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));

    // create window (internal buffer)
    q->w = WINDOW(_create)(q->h_len);

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        q->h[i-1] = _h[_n-i];

    // create dot product object
    q->dp = DOTPROD(_create)(q->h, q->h_len);

    // reset filter state (clear buffer)
    FIRFILT(_clear)(q);

    return q;
}

// create firfilt object from prototype
FIRFILT() FIRFILT(_create_prototype)(unsigned int _n)
{
    fprintf(stderr,"error: firfilt_xxxt_create_prototype(), not yet implemented\n");
    exit(1);

    FIRFILT() q = (FIRFILT()) malloc(sizeof(struct FIRFILT(_s)));
    q->h_len = _n;
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));

    // use firdespm here

    return q;
}

// re-create firfilt object
//  _q      :   original firfilt object
//  _h      :   new coefficients [size: _n x 1]
//  _n      :   new filter length
FIRFILT() FIRFILT(_recreate)(FIRFILT() _q,
                             TC * _h,
                             unsigned int _n)
{
    unsigned int i;

    // reallocate memory array if filter length has changed
    if (_n != _q->h_len) {
        // reallocate memory
        _q->h_len = _n;
        _q->h = (TC*) realloc(_q->h, (_q->h_len)*sizeof(TC));

        // recreate window object, preserving internal state
        _q->w = WINDOW(_recreate)(_q->w, _q->h_len);
    }

    // load filter in reverse order
    for (i=_n; i>0; i--)
        _q->h[i-1] = _h[_n-i];

    // re-create dot product object
    DOTPROD(_destroy)(_q->dp);
    _q->dp = DOTPROD(_create)(_q->h, _q->h_len);

    return _q;
}

// destroy firfilt object
void FIRFILT(_destroy)(FIRFILT() _q)
{
    WINDOW(_destroy)(_q->w);
    DOTPROD(_destroy)(_q->dp);
    free(_q->h);
    free(_q);
}

// reset internal state of filter object
void FIRFILT(_clear)(FIRFILT() _q)
{
    WINDOW(_clear)(_q->w);
}

// print filter object internals (taps, buffer)
void FIRFILT(_print)(FIRFILT() _q)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _q->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_q->h[n-i-1],%12.8f);
        printf("\n");
    }

    WINDOW(_print)(_q->w);
}

// push sample into filter object's internal buffer
//  _q      :   filter object
//  _x      :   input sample
void FIRFILT(_push)(FIRFILT() _q,
                    TI _x)
{
    WINDOW(_push)(_q->w, _x);
}

// compute output sample (dot product between internal
// filter coefficients and internal buffer)
//  _q      :   filter object
//  _y      :   output sample pointer
void FIRFILT(_execute)(FIRFILT() _q,
                       TO *_y)
{
    // read buffer (retrieve pointer to aligned memory array)
    TI *r;
    WINDOW(_read)(_q->w, &r);

    // execute dot product
    DOTPROD(_execute)(_q->dp, r, _y);
}

// get filter length
unsigned int FIRFILT(_get_length)(FIRFILT() _q)
{
    return _q->h_len;
}
