/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "autotest/autotest.h"
#include "liquid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

float qs1dsearch_autotest_utility_min(float _v, void * _context)
{
    float v_opt = *(float*)(_context);
    float v = _v - v_opt;
    return tanhf(v)*tanhf(v);
}

//
void autotest_qs1dsearch_min()
{
    // create qs1dsearch object
    float v_opt  = 0.0f;
    qs1dsearch q = qs1dsearch_create(qs1dsearch_autotest_utility_min,
        &v_opt, LIQUID_OPTIM_MINIMIZE);

    //qs1dsearch_init_bounds(q, -20, 10);
    qs1dsearch_init(q, -50);

    // run search
    unsigned int i;
    for (i=0; i<32; i++) {
        qs1dsearch_step(q);
        if (liquid_autotest_verbose)
            qs1dsearch_print(q);
    }

    // check result
    CONTEND_DELTA( qs1dsearch_get_opt_v(q), v_opt, 1e-3f );
    CONTEND_LESS_THAN( qs1dsearch_get_opt_u(q), 1e-3f );

    if (liquid_autotest_verbose) {
        printf("%3u : u(%12.8f) = %12.4e, v_opt=%12.4e (error=%12.4e)\n",
            i,
            qs1dsearch_get_opt_v(q),
            qs1dsearch_get_opt_u(q),
            v_opt,
            v_opt - qs1dsearch_get_opt_v(q));
    }

    qs1dsearch_destroy(q);
}
