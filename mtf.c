#include <stdio.h>
#include "util.h"
#include "mtf.h"

void mtf(symbol *data, int length){
  symbol bookstack[MTF_SYMBOLS];

  int i, j;
  for(j=0; j<MTF_SYMBOLS; j++){
    bookstack[j] = j;
  }

  for(i=0; i<length; i++){
    // find the value
    for(j=0; j<MTF_SYMBOLS; j++){
      if(bookstack[j]==data[i])
        break;
    }

    // encode it
    data[i] = j;

    // move to front
    for(; j>0; j--){
      bookstack[j] = bookstack[j-1];
    }
    bookstack[0] = data[i];
  }
}

void unmtf(symbol *data, int length){
  symbol bookstack[MTF_SYMBOLS];

  int i, j;
  for(j=0; j<MTF_SYMBOLS; j++){
    bookstack[j] = j;
  }

  for(i=0; i<length; i++){
    // decode
    j = data[i];
    data[i] = bookstack[j];

    // move to front
    for(; j>0; j--){
      bookstack[j] = bookstack[j-1];
    }
    bookstack[0] = data[i];
  }
}

