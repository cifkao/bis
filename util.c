#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "util.h"

void (*finalize_f)(bool) = NULL;

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

void die_format(){
  die("Invalid file format.");
}

void err(char *message, ...){
  va_list args;
  va_start(args, message);
  fprintf(stderr, "bis: ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  va_end(args);
}

void die(char *message, ...){
  va_list args;
  va_start(args, message);
  fprintf(stderr, "bis: ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  va_end(args);

  if(finalize_f != NULL)
    (*finalize_f)(false);
  exit(1);
}

void set_finalize(void (*f)(bool)){
  finalize_f = f;
}


void dump_data(symbol *data, size_t length, char *label){
  int i;
  fprintf(stderr, "%s: ", label);
  for(i=0; i<length; i++){
    fprintf(stderr, "%d ", data[i]);
  }
  fprintf(stderr, "\n");
}
