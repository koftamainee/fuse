#ifndef TRIE_H_
#define TRIE_H_

#include "cstring.h"
#include "errors.h"

typedef struct trie_node {
    int value;
    int is_end_of_word;
    struct trie_node *subtrees;
} trie_node;

typedef struct trie {
    String alphabet;
    size_t alphabet_size;
    trie_node *root;
} trie;

err_t trie_init(trie **t, const char *alphabet);

void trie_free(trie *t);

err_t trie_set(trie *t, String key, int value);
err_t trie_get(trie *t, String key, int *value_placeholder);
err_t trie_dispose(trie *t, String key);

#endif  // !TRIE_H_
