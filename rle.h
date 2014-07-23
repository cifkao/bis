#ifndef RLE_H
#define RLE_H

#include "util.h"

/* Encode the data using a run-length encoding such that a sequence of two
 * identical symbols is followed by a repeat length. A pointer to the newly
 * allocated output array is passed to *output and its length is returned.
 */
size_t rle(symbol *data, symbol **output, size_t length, size_t symbols);

/* Decode the data encoded using the run-length encoding. A pointer to
 * the newly allocated output array is passed to *output and its length is
 * returned.
 */
size_t unrle(symbol *data, symbol **output, size_t length, size_t symbols);

#endif
