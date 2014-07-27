#include <stdio.h>
#include <limits.h>
#include "util.h"
#include "fileutil.h"

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif


FILE *fsetbin(const char *mode, FILE *f){

#ifdef _MSC_VER
  // MSVC++ doesn't allow filename to be NULL, so we call _setmode instead
  if(_setmode(_fileno(f), _O_BINARY)==-1)
    return NULL;
  else
    return f;
#else
  return freopen(NULL, mode, f);
#endif

}


file file_wrap(FILE *ff){
  file f;
  f.file = ff;
  f.charBuffSize = 0;
  f.charBuffPtr = 0;
  f.bitBuffSize = 0;
  return f;
}

/* Input file functions. */

int file_get_char(file *f){
  if(f->charBuffPtr==f->charBuffSize){
    if(feof(f->file) || ferror(f->file)) return EOF;
    f->charBuffSize =
      fread(f->charBuff, sizeof(unsigned char), CHAR_BUFFER_SIZE, f->file);
    f->charBuffPtr = 0;

    if(ferror(f->file))
      die("Error reading from file.");
  }
  
  if(f->charBuffPtr==f->charBuffSize) return EOF;
  else return f->charBuff[f->charBuffPtr++];
}

int bitfile_get_bit(file *f){
  if(f->bitBuffSize==0){
    int c = file_get_char(f);
    if(c==EOF) return EOF;
    f->bitBuff = (unsigned char)c;
    f->bitBuffSize = CHAR_BIT;
  }

  f->bitBuffSize--;
  return (f->bitBuff >> f->bitBuffSize) & 1;
}

void bitfile_discard_bits(file *f){
  if(f->bitBuffSize != CHAR_BIT)
    f->bitBuffSize = 0;
}

int bitfile_get_symbol(file *f, size_t count){
  int bit;
  symbol s = 0;
  while(count>0 && (bit = bitfile_get_bit(f)) != EOF){
    s = (s << 1) | bit;
    count--;
  }
  if(count>0) return EOF;
  else return s;
}

/* Output file functions. */

void file_flush_chars(file *f);
void bitfile_flush_bits(file *f);

void file_put_char(file *f, unsigned char c){
  f->charBuff[f->charBuffSize++] = c;

  if(f->charBuffSize==CHAR_BUFFER_SIZE)
    file_flush_chars(f);
}

void bitfile_put_bit(file *f, int bit){
  f->bitBuff = (f->bitBuff << 1) | (bit && 1);
  f->bitBuffSize++;

  if(f->bitBuffSize==CHAR_BIT)
    bitfile_flush_bits(f);
}

void bitfile_put_symbol(file *f, symbol s, size_t count){
  int i;
  for(i=count-1; i>=0; i--){
    bitfile_put_bit(f, (s >> i) & 1);
  }
}

void file_flush_chars(file *f){
  fwrite(f->charBuff, sizeof(unsigned char), f->charBuffSize, f->file);
  f->charBuffSize = 0;

  if(ferror(f->file))
    die("Error writing to file.");
}

void bitfile_flush_bits(file *f){
  if(f->bitBuffSize>0){
    file_put_char(f, f->bitBuff << (CHAR_BIT - f->bitBuffSize));
    f->bitBuffSize = 0;
  }
}

void file_flush(file *f){
  bitfile_flush_bits(f);
  file_flush_chars(f);
}

