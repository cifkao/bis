#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "util.h"

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
