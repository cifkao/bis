#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "util.h"
#include "fileutil.h"
#include "huffman.h"

#define HUF_EOF 257
#define SYMBOL_LENGTH 9

struct heap_node{
  int freq;
  huf_node *tree;
};
typedef struct heap_node heap_node;

/* A bit array for storing a code for a symbol. In each byte, the bits
 * of the code are stored so that the least significant bit is the 
 * first one.
 */
struct code{
  unsigned char code[(HUF_SYMBOLS+CHAR_BIT-1)/CHAR_BIT];
  int length;
};
typedef struct code code;


void heap_push(heap_node **heap, int size, heap_node *node){
  heap[++size] = node;

  int i = size, j = i;
  i = i/2;
  while(i>0 && heap[j]->freq < heap[i]->freq){
    heap_node *tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
    j = i;
    i = i/2;
  }
}

heap_node *heap_pop(heap_node **heap, int size){
  heap_node *out = heap[1];
  heap[1] = heap[size--];

  int i = 1, j;
  while((j = 2*i) <= size){
    if(2*i+1 <= size && heap[2*i+1]->freq < heap[j]->freq) j++;
    if(heap[i]->freq > heap[j]->freq){
      heap_node *tmp = heap[i];
      heap[i] = heap[j];
      heap[j] = tmp;
    }else{
      break;
    }
    i = j;
  }

  return out;
}

huf_tree huf_build_tree(symbol *data, int length){
  huf_node *trees =
    (huf_node *)malloc_or_die(2*HUF_SYMBOLS*sizeof(huf_node));
  huf_node *newTree = trees;

  heap_node heapNodes[HUF_SYMBOLS];
  heap_node *newHeapNode = heapNodes;

  heap_node *_heap[HUF_SYMBOLS];
  heap_node **heap = _heap-1;
  int heapSize = 0;

  int freqs[HUF_SYMBOLS];
  int i;
  for(i=0; i<length; i++){
    freqs[data[i]]++;
  }
  freqs[HUF_EOF] = 1;

  symbol s;
  for(s=0; s<HUF_SYMBOLS; s++){
    if(freqs[s]>0){
      newTree->symbol = s;
      newTree->left = NULL;
      newTree->right = NULL;
      newHeapNode->tree = newTree++;
      newHeapNode->freq = freqs[s];
      heap_push(heap, heapSize++, newHeapNode++);
    }
  }

  for(; heapSize>=2; heapSize--){
    heap_node *n1 = heap_pop(heap, heapSize);
    heap_node *n2 = heap_pop(heap, heapSize-1);
    newTree->left = n1->tree;
    newTree->right = n2->tree;
    n2->tree = newTree++; // reuse the old heap node
    n2->freq += n1->freq;
    heap_push(heap, heapSize-2, n2);
  }
 
  huf_tree t = {heap_pop(heap, heapSize)->tree, trees};
  return t;
}

/* Write the Huffman tree recursively. */
void write_tree(file *f, huf_node *node){
  if(node->left){ // internal node
    bitfile_put_bit(f, 0);
    write_tree(f, node->left);
    write_tree(f, node->right);
  }else{ // leaf node
    bitfile_put_bit(f, 1);
    bitfile_put_symbol(f, node->symbol, SYMBOL_LENGTH);
  }
}

void huf_write_tree(file *f, huf_tree tree){
  write_tree(f, tree.root);
}

void build_code_table(huf_node *node, code *codes, code *currentCode){
  int byteOffset = currentCode->length / CHAR_BIT;
  int bitOffset = currentCode->length % CHAR_BIT;
  if(node->left){ // internal node
    currentCode->length++;
    currentCode->code[byteOffset] &= ~(1 << bitOffset);
    build_code_table(node->left, codes, currentCode);
    currentCode->code[byteOffset] |= (1 << bitOffset);
    build_code_table(node->right, codes, currentCode);
    currentCode->length--;
  }else{ // leaf node
    memcpy(&codes[node->symbol], currentCode, sizeof(code));
  }
}

void huf_encode(file *f, symbol *data, int length, huf_tree tree){
  code codes[HUF_SYMBOLS];
  code currentCode;
  currentCode.length = 0;
  build_code_table(tree.root, codes, &currentCode);

  int i, j;
  for(i=0; i<=length; i++){
    symbol s = i<length ? data[i] : HUF_EOF;
    for(j=0; j<codes[s].length; j++){
      int byteOffset = j / CHAR_BIT;
      int bitOffset = j % CHAR_BIT;
      bitfile_put_bit(f, (codes[s].code[byteOffset] >> bitOffset) & 1);
    }
  }
}

int decode_symbol(file *f, huf_tree tree){
  huf_node *node = tree.root;
  int bit = bitfile_get_bit(f);
  if(bit==EOF)
    return EOF;
  // read until we reach a leaf node
  while(node->left){
    if(bit==EOF) die_eof();
    if(!bit)
      node = node->left;
    else
      node = node->right;

    if(node->left)
      bit = bitfile_get_bit(f);
  }
  
  return node->symbol;
}

int huf_decode(file *f, symbol **data, huf_tree tree){
  int buffSize = 1024;
  int length = 0;
  *data = (symbol *)malloc_or_die(buffSize*sizeof(symbol));
  int s = decode_symbol(f, tree);
  if(s==EOF)
    return 0;
  do{
    if(s==EOF) die_eof();
    length++;
    if(length==buffSize){
      buffSize *= 2;
      *data = (symbol *)realloc_or_die(*data, buffSize*sizeof(symbol));
    }

    (*data)[length-1] = s;

    s = decode_symbol(f, tree);
  }while(s!=HUF_EOF);

  return length;
}

/* Build the Huffman tree recursively.
 */
void read_tree(file *f, huf_node *root){
  int bit = bitfile_get_bit(f);
  if(bit==EOF) die_eof();
  if(!bit){ // internal node
    huf_node *left = (huf_node *)malloc_or_die(2*sizeof(huf_node));
    huf_node *right = left+1;
    root->left = left;
    root->right = right;
    read_tree(f, left);
    read_tree(f, right);
  }else{ // leaf node
    int s = bitfile_get_symbol(f, SYMBOL_LENGTH);
    if(s==EOF) die_eof();
    root->symbol = s;
    root->left = NULL;
    root->right = NULL;
  }
}

huf_tree huf_read_tree(file *f){
  huf_node *root = (huf_node *)malloc_or_die(sizeof(huf_node));
  read_tree(f, root);
  huf_tree t = {root, root};
  return t;
}

void huf_free_tree(huf_tree tree){
  free(tree.nodes);
}

