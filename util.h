#ifndef UTIL_H
#define UTIL_H

#define BWT_SYMBOLS 257
#define MTF_SYMBOLS 257
#define HUF_SYMBOLS 258

typedef unsigned short symbol;

void *malloc_or_die(size_t size);

void *realloc_or_die(void* ptr, size_t size);

void die_eof();

void die(char *message);

#endif
