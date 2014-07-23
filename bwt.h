#ifndef BWT_H
#define BWT_H

#include "util.h"

/* Apply the Burrows-Wheeler transform to the data, writing the result
 * to the output array. The length of the output array must be length+1.
 */
void bwt(symbol *data, symbol *output, size_t length);

/* Apply the inverse Burrows-Wheeler transform to the data, writing the
 * result to the output array. The length of the output array shall be
 * length-1.
 */
void unbwt(symbol *data, symbol *output, size_t length);

#endif
