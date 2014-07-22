#ifndef MTF_H
#define MTF_H

#include "util.h"

/* Encode the data using the move-to-front transform, writing the result
 * back into the original array.
 */
void mtf(symbol *data, int length);

/* Decode the data encoded using the move-to-front transform,
 * writing the result back into the original array.
 */
void unmtf(symbol *data, int length);


#endif
