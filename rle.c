#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "rle.h"

#define NONE symbols // a value that does not occur in the data

size_t rle(symbol *data, symbol **output, size_t length, size_t symbols){
  symbol_buffer buf;
  buf.size = length*3/2;
  buf.dataLength = 0;
  buf.buffer = NULL;

  symbol last = NONE;
  int count = 0;
  int i;
  for(i=0; i<length; i++){
    symbol s = data[i];
    if(s==last){
      count++;
      if(count<2){
        buffer_put(s, &buf);
      }
      if(count==symbols){
        buffer_put(symbols-1, &buf);
        last = NONE;
        count = 0;
      }
    }else{
      if(count>=1){
        buffer_put(count-1, &buf);
      }
      count = 0;
      last = s;

      buffer_put(s, &buf);
    }
  }

  // last run length
  if(count>=1){
    buffer_put(count-1, &buf);
  }

  *output = buf.buffer;
  return buf.dataLength;
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
