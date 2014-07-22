#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdbool.h>
#include "util.h"

/* Huffman tree node */
struct huf_node{
  symbol symbol;
  struct huf_node *left;
  struct huf_node *right;
};
typedef struct huf_node huf_node;

struct huf_tree{
  huf_node *root;
  huf_node *nodes; // the array of nodes
};
typedef struct huf_tree huf_tree;

/* Build a Huffman tree from the given data.
 */
huf_tree huf_build_tree(symbol* data, int length);

/* Write the representation of the given Huffman tree to a file.
 */
void huf_write_tree(file *f, huf_tree tree);

/* Encode the data using the given Huffman tree and write it to a file.
 */
void huf_encode(file *f, symbol *data, int length, huf_tree tree);

/* Build a Huffman tree from its representation stored in a file.
 */
huf_tree huf_read_tree(file *f);

/* Decode a block of data stored in a file using the given Huffman tree.
 * A pointer to the newly allocated output array is passed to *output and
 * the length of the decoded block is returned.
 */
int huf_decode(file *f, symbol **output, huf_tree tree);

/* Destroy the given huffman tree.
 */
void huf_free_tree(huf_tree tree);

#endif
