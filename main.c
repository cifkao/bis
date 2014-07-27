#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "fileutil.h"
#include "rle.h"
#include "bwt.h"
#include "mtf.h"
#include "huffman.h"

//#define DEBUG

#ifdef DEBUG
  #define DUMPDATA(label) dump_data(data, len, label)
#else
  #define DUMPDATA(label)
#endif

const char *SIGNATURE = "BIS";

enum {COMPRESS, DECOMPRESS, HELP} action = COMPRESS;
size_t blockSize = 200*1024;
char *inFilename = NULL;
char *outFilename = NULL;
char *suffix = ".bis";
bool startedWriting = false;
bool forceStdout = false;
FILE *_in = NULL;
FILE *_out = NULL;

void compress(){
  file out = file_wrap(_out);

  int i;
  for(i=0; i<strlen(SIGNATURE); i++){
    file_put_char(&out, SIGNATURE[i]);
  }
  file_flush(&out);

  while(!ferror(_in) && !feof(_in)){
    unsigned char *block =
      (unsigned char *)malloc_or_die(blockSize*sizeof(unsigned char));
    symbol *data = (symbol *)malloc_or_die(blockSize*sizeof(symbol));
    size_t len = fread(block, sizeof(unsigned char), blockSize, _in);
    
    for(i=0; i<len; i++){
      data[i] = (symbol)block[i];
    }
    free(block);

    DUMPDATA("raw");

    { // Run-length encoding
      symbol *output;
      len = rle(data, &output, len, RAW_SYMBOLS);
      free(data);
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
  
  if(ferror(_in))
    die("Error reading from file.");
}

void decompress(){
  file in  = file_wrap(_in);
  file out = file_wrap(_out);

  int i;
  for(i=0; i<strlen(SIGNATURE); i++){
    if(file_get_char(&in) != SIGNATURE[i])
      die_format();
  }

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

    for(i=0; i<len; i++){
      file_put_char(&out, (unsigned char)data[i]);
    }
    free(data);
  }
  file_flush(&out);
}

void help(){
  fprintf(stderr, "Usage: bis [OPTION]... [FILE]\n\n"
      "Options are:\n"
      "  -b SIZE[k]   specify block size (default: 200k)\n"
      "  -c           write to standard output\n"
      "  -d           decompress\n"
      "  -h           display this help message\n"
      "  -o OUTFILE   specify output file\n\n"
      "By default, append the .bis suffix when compressing and strip\n"
      "it when decompressing (if possible). If no input file is\n"
      "specified, read standard input and write to standard output.\n");
}

void finalize(bool success){
  if(_in)
    fclose(_in);
  if(_out)
    fclose(_out);
  if(!success && outFilename && startedWriting)
    remove(outFilename);
}

int main(int argc, char **argv){
  set_finalize(&finalize);

  int i;
  bool endOfOptions = false;
  for(i=1; i<argc; i++){
    if(argv[i][0] != '-' || endOfOptions){
      if(inFilename)
        err("Warning: Can't compress more files.");
      inFilename = argv[i];
    }else if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0){
      action = HELP;
    }else if(strcmp(argv[i], "-d")==0){
      action = DECOMPRESS;
    }else if(strcmp(argv[i], "-b")==0){
      if(i+1>=argc)
        die("Missing parameter: -b");

      char unit;
      int n = sscanf(argv[++i], "%d%c", &blockSize, &unit);
      if(n==2 && unit=='k')
        blockSize *= 1024;
      else if(n!=1 || blockSize<1)
        die("Invalid block size: %s", argv[i]);
    }else if(strcmp(argv[i], "-o")==0){
      if(i+1>=argc)
        die("Missing parameter: -o");
      outFilename = argv[++i];
    }else if(strcmp(argv[i], "-c")==0){
      forceStdout = true;
    }else if(strcmp(argv[i], "--")==0){
      endOfOptions = true;
    }else{
      die("Unknown option: %s", argv[i]); 
    }
  }

  if(action == COMPRESS || action == DECOMPRESS){
    if(inFilename!=NULL){
      _in = fopen(inFilename, "rb");
      if(_in==NULL)
        die("Error opening %s for reading.", inFilename);

      if(outFilename==NULL && !forceStdout){
        outFilename = (char *)malloc_or_die(
            (strlen(inFilename)+strlen(suffix))*sizeof(char));

        strcpy(outFilename, inFilename);
        if(action == COMPRESS){
          strcat(outFilename, suffix); // add the .bis suffix
        }else if(action == DECOMPRESS){
          char *suf = strrchr(outFilename, '.');
          if(suf != NULL && strcmp(suf, suffix)==0){ 
            *suf = '\0'; // strip the .bis suffix
          }else{
            die("Please specify an output file.");
          }
        }
      }
    }else{
      _in = fsetbin("rb", stdin);
      if(_in==NULL)
        die("Error opening standard input as binary.");
    }

    if(outFilename && !forceStdout){
      _out = fopen(outFilename, "wb");
      if(_out==NULL)
        die("Error opening %s for writing.", outFilename);
    }else{
      _out = fsetbin("wb", stdout);
      if(_out==NULL)
        die("Error opening standard output as binary.");
    }

    startedWriting = true;
  }

  if(action == COMPRESS){
    compress();
  }else if(action == DECOMPRESS){
    decompress();
  }else{
    help();
  }

  finalize(true);

  return EXIT_SUCCESS;
}
