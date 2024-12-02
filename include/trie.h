#ifndef TRIE_H_
#define TRIE_H_

#include "cstring.h"
#include "errors.h"

// a-z + A-Z + 0-9 + _
#define ALPHABET_SIZE 63

typedef struct TrieNode {
    struct TrieNode *childs[ALPHABET_SIZE];
    String data;
    int is_end_of_word;
} TrieNode;

typedef TrieNode Trie;

Trie *trie_init();
err_t trie_insert(Trie *root, const String key, const String value);
err_t trie_search(const Trie *root, const String key, String *result);
err_t trie_delete(Trie *root, const String key);
void trie_free(Trie *root);

#endif
