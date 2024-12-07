#ifndef TRIE_H_
#define TRIE_H_

#include "cstring.h"
#include "errors.h"

typedef int tvalue;

typedef struct TrieNode {
    struct TrieNode **childs;
    tvalue data;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
    String alphabet;
    size_t alphabet_size;
    int (*is_in_alphabet)(char c);
} Trie;

err_t trie_init(Trie *t, String alphabet, int (*is_in_alphabet)(char c));
err_t trie_insert(Trie *t, const String key, tvalue value);
err_t trie_search(const Trie *t, const String key, tvalue *result);
err_t trie_delete(Trie *t, const String key);
void trie_free(Trie *t);

#endif
