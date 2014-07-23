#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "util.h"

void buffer_put(symbol c, symbol_buffer *buf){
  if(buf->buffer==NULL){
    buf->buffer = (symbol *)malloc_or_die(buf->size*sizeof(symbol));
  }
  if(buf->dataLength==buf->size){
    buf->size *= 2;
    buf->buffer =
      (symbol *)realloc_or_die(buf->buffer, buf->size*sizeof(symbol));
  }

  buf->buffer[buf->dataLength++] = c; 
}

void *malloc_or_die(size_t size){
  void *m = malloc(size);
  if(m==NULL)
    die("Not enough memory.");
  return m;
}

void *realloc_or_die(void* ptr, size_t size){
  void *m = realloc(ptr, size);
  if(m==NULL){
    free(ptr);
    die("Not enough memory.");
  }
  return m;
}

void die_eof(){
  die("Unexpected end of file.");
}

void die(char *message){
  fprintf(stderr, "bis: %s\n", message);
  finalize();
  exit(1);
}
