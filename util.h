#ifndef UTIL_H
#define UTIL_H

#include <limits.h>
#include <stdbool.h>

#define RAW_SYMBOLS (UCHAR_MAX+1)
#define BWT_SYMBOLS (RAW_SYMBOLS+1)
#define MTF_SYMBOLS BWT_SYMBOLS
#define HUF_SYMBOLS (MTF_SYMBOLS+1)

typedef unsigned short symbol;

typedef struct {
  symbol *buffer;
  size_t size;
  size_t dataLength;
} symbol_buffer;

void buffer_put(symbol s, symbol_buffer *buf);

void *malloc_or_die(size_t size);

void *realloc_or_die(void* ptr, size_t size);

void die_format();

void die(char *message, ...);

void set_finalize(void (*f)(bool));


void dump_data(symbol *data, size_t length, char *label);

#endif
