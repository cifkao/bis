#ifndef BWT_H
#define BWT_H

#include "util.h"

/* Apply the Burrows-Wheeler transform on the data, writing the result
 * to the output array. The length of the output array must be length+1.
 */
void bwt(symbol *data, symbol *output, int length);

/* Apply the inverse Burrows-Wheeler transform on the data, writing the
 * result to the output array. The length of the output array shall be
 * length-1.
 */
void unbwt(symbol *data, symbol *output, int length);

#endif
