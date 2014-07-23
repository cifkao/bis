#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <stdio.h>

#define CHAR_BUFFER_SIZE 4096

/* A data structure for buffered file I/O.
 */
struct file {
  FILE *file;
  unsigned char charBuff[CHAR_BUFFER_SIZE];
  int charBuffSize, charBuffPtr;
  unsigned char bitBuff;
  int bitBuffSize; // the number of bits in the bit buffer
};
typedef struct file file;

/* Create a buffered file data structure from a given FILE.
 */
file file_wrap(FILE *ff);

/* Get the next byte from the file.
 */
int file_get_char(file *f);

/* Get the next bit from the file, MSB-first.
 */
int bitfile_get_bit(file *f);

/* Empty the bit buffer and start reading from the next byte.
 */
void bitfile_discard_bits(file *f);

/* Read a symbol number from the file, bit by bit, MSB-first. The number
 * of bits used to represent the symbol is passed as a parameter.
 */
int bitfile_get_symbol(file *f, size_t count);

/* Write a char to the file.
 */
void file_put_char(file *f, unsigned char c);

/* Write a bit to the file, MSB-first.
 */
void bitfile_put_bit(file *f, int bit);

/* Write a symbol number to the file, bit by bit, MSB-first. The number
 * of bits used to represent the symbol is passed as a parameter.
 */
void bitfile_put_symbol(file *f, symbol s, size_t count);

/* Write any buffered bits, padding the byte with trailing zeros.
 */
void bitfile_flush_bits(file *f);

/* Write all buffered data immediately.
 */
void file_flush(file *f);

#endif
