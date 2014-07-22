#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "bwt.h"

#define BWT_EOF 256

symbol *data;
int length;

int bwtcmp(const void *A, const void *B){
  const int a = *(int *)A;
  const int b = *(int *)B;
  
  int num = a<b ? length-b : length-a;
  int i, cmp = 0;
  for(i=0; i<num; i++){
    if(data[a+i]!=data[b+i]){
      cmp = data[a+i]-data[b+i];
      break;
    }
  }
  return cmp;
}

int cmp(const void *A, const void *B){
  const int a = *(symbol *)A;
  const int b = *(symbol *)B;
  return a-b;
}

void bwt(symbol *_data, symbol *output, int _length){
  data = _data;
  length = _length+1;
  data[length-1] = BWT_EOF;

  // the permutation of rows
  int *perm = (int *)malloc_or_die(length*sizeof(int));

  int i;
  for(i=0; i<length; i++){
    perm[i] = i;
  }
  qsort(perm, length, sizeof(int), bwtcmp);

  for(i=0; i<length; i++){
    // find the last symbol of this row and write it to output
    int index = (perm[i]+length-1)%length;
    output[i] = data[index];
  }

  free(perm);
}

void unbwt(symbol *L, symbol *output, int length){
  /* P[i]:  the number of instances of L[i] in L[0,...,i-1]
   *        (L[i] is the P[i]-th instance, counting from 0)
   * C[s]:  the number of instances in L of symbols preceding s
   *        in the alphabet */
  int *P = (int *)malloc_or_die(length*sizeof(int));
  int *C = (int *)malloc_or_die(BWT_SYMBOLS*sizeof(int));

  int i, I;
  // count all occurences of s into C[s] for every symbol s, build P,
  // also find the EOF symbol
  for(i=0; i<length; i++){
    P[i] = C[L[i]]++;

    if(L[i]==BWT_EOF)
      I = i;
  }

  // finish building C
  int sum = 0; symbol s;
  for(s=0; s<BWT_SYMBOLS; s++){
    sum += C[s];
    C[s] = sum - C[s];
  }

  // decode the data
  i = P[I] + C[L[I]];
  int j;
  for(j=1; j<length; j++){
    output[length-j] = L[i];
    i = P[i] + C[L[i]];
  }

  free(P);
  free(C);
}
