#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "rle.h"

#define NONE symbols // a value that does not occur in the data

int rle(symbol *data, symbol **output, int length, int symbols){
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

int buffSize;
int buffPtr;
void put(symbol c, symbol **output){
  if(*output==NULL){
    *output = (symbol *)malloc_or_die(buffSize*sizeof(symbol));
  }else if(buffPtr==buffSize){
    buffSize *= 2;
    *output = (symbol *)realloc_or_die(*output, buffSize*sizeof(symbol));
  }

  (*output)[buffPtr++] = c;
}

int unrle(symbol *data, symbol **output, int length, int symbols){
  *output = NULL;
  buffSize = 2*length;
  buffPtr = 0;

  symbol last = NONE;
  int j;
  for(j=0; j<length; j++){
    symbol c = data[j];
    put(c, output);

    if(c==last){
      int count = data[++j];
      int i;
      for(i=0; i<count; i++){
        put(c, output);
      }
      last = NONE;
    }else{
      last = c;
    }
  }

  return j;
}
