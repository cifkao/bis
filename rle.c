#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "rle.h"

#define NONE symbols // a value that does not occur in the data

size_t rle(symbol *data, symbol **output, size_t length, size_t symbols){
  *output = (symbol *)malloc_or_die(length*3/2*sizeof(symbol));

  symbol last = NONE;
  int count = 0;
  int i, j=0; // we read from data[i] and write to *output[j]
  for(i=0; i<length; i++){
    symbol c = data[i];
    if(c==last){
      count++;
      if(count<2){
        (*output)[j++] = c;
      }
      if(count==symbols){
        (*output)[j++] = symbols-1;
        last = NONE;
        count = 0;
      }
    }else{
      if(count>=1){
        (*output)[j++] = count-1;
      }
      count = 0;
      last = c;

      (*output)[j++] = c;
    }
  }

  return j;
}

size_t unrle(symbol *data, symbol **output, size_t length, size_t symbols){
  symbol_buffer buf;
  buf.size = 2*length;
  buf.dataLength = 0;
  buf.buffer = NULL;

  symbol last = NONE;
  int j;
  for(j=0; j<length; j++){
    symbol c = data[j];
    buffer_put(c, &buf);

    if(c==last){
      int count = data[++j];
      int i;
      for(i=0; i<count; i++){
        buffer_put(c, &buf);
      }
      last = NONE;
    }else{
      last = c;
    }
  }

  *output = buf.buffer;
  return buf.dataLength;
}
