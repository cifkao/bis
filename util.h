#ifndef UTIL_H
#define UTIL_H

#define RAW_SYMBOLS 256
#define BWT_SYMBOLS 257
#define MTF_SYMBOLS 257
#define HUF_SYMBOLS 258

typedef unsigned short symbol;

typedef struct {
  symbol *buffer;
  size_t size;
  size_t dataLength;
} symbol_buffer;

void buffer_put(symbol s, symbol_buffer *buf);

void *malloc_or_die(size_t size);

void *realloc_or_die(void* ptr, size_t size);

void die_eof();

void die(char *message);

void set_finalize(void (*f)());


void dump_data(symbol *data, size_t length, char *label);

#endif
