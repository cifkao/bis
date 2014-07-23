#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "fileutil.h"
#include "rle.h"
#include "bwt.h"
#include "mtf.h"
#include "huffman.h"
#include "main.h"

#define BLOCK_SIZE 4096

#ifdef DEBUG
  #define DUMPDATA(label) dump_data(data, len, label)
#else
  #define DUMPDATA(label)
#endif

void compress(){
  file out = file_wrap(stdout);
  while(!ferror(stdin) && !feof(stdin)){
    unsigned char block[BLOCK_SIZE];
    symbol _data[BLOCK_SIZE];
    symbol *data = _data;
    size_t len = fread(block, sizeof(unsigned char), BLOCK_SIZE, stdin);
    
    int i;
    for(i=0; i<len; i++){
      data[i] = (symbol)block[i];
    }

    DUMPDATA("raw");

    { // Run-length encoding
      symbol *output;
      len = rle(data, &output, len, RAW_SYMBOLS);
      data = output;
    }

    DUMPDATA("rle");

    { // Burrows-Wheeler transform
      symbol *output = (symbol *)malloc_or_die((len+1)*sizeof(symbol));
      bwt(data, output, len++);
      free(data);
      data = output;
    }

    DUMPDATA("bwt");

    { // Move-to-front transform
      mtf(data, len);
    }

    DUMPDATA("mtf");

    { // Run-length encoding
      symbol *output;
      len = rle(data, &output, len, MTF_SYMBOLS);
      free(data);
      data = output;
    }

    DUMPDATA("rle");

    { // Huffman coding
      huf_tree tree = huf_build_tree(data, len);
      huf_write_tree(&out, tree);
      huf_encode(&out, data, len, tree);
      file_flush(&out); // also adds bit padding
      free(data);
      huf_free_tree(tree);
    }
  }
}

void decompress(){
  file in  = file_wrap(stdin);
  file out = file_wrap(stdout);

  while(1){
    symbol *data; size_t len;

    { // Huffman coding
      huf_tree tree = huf_read_tree(&in);
      len = huf_decode(&in, &data, tree);
      bitfile_discard_bits(&in); // skip bit padding
      huf_free_tree(tree);
    }

    if(len==0) break;

    DUMPDATA("rle");

    { // Run-length encoding
      symbol *output;
      len = unrle(data, &output, len, MTF_SYMBOLS);
      free(data);
      data = output;
    }

    DUMPDATA("mtf");

    { // Move-to-front encoding
      unmtf(data, len);
    }

    DUMPDATA("bwt");

    { // Burrows-Wheeler transform
      symbol *output = (symbol *)malloc_or_die((len-1)*sizeof(symbol));
      unbwt(data, output, len--);
      free(data);
      data = output;
    }

    DUMPDATA("rle");

    { // Run-length encoding
      symbol *output;
      len = unrle(data, &output, len, RAW_SYMBOLS);
      free(data);
      data = output;
    }

    DUMPDATA("raw");

    int i;
    for(i=0; i<len; i++){
      file_put_char(&out, (unsigned char)data[i]);
    }
    free(data);
  }
  file_flush(&out);
}

int main(int argc, char **argv){
  set_finalize(&finalize);

  if(argc<=1 || argv[1][0] == 'c')
    compress();
  else if(argv[1][0] == 'x')
    decompress();

  return EXIT_SUCCESS;
}

void finalize(){

}
