#include "../trie.h"

#include <stdlib.h>

#include "../errors.h"
#include "../logger.h"

Trie *trie_init() {
    int i;
    Trie *root;
    root = (Trie *)malloc(sizeof(Trie));
    if (root == NULL) {
        log_error("Error allocating memory for Trie node");
        return NULL;
    }
    root->data = NULL;
    root->is_end_of_word = 0;
    for (i = 0; i < ALPHABET_SIZE; ++i) {
        root->childs[i] = NULL;
    }
    return root;
}
err_t trie_insert(Trie *root, const String key, const String value) {
    if (root == NULL) {
        log_error("root is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (key == NULL) {
        log_error("key is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (value == NULL) {
        log_error("value is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    return EXIT_SUCCESS;
}
err_t trie_search(const Trie *root, const String key, String *result) {
    if (root == NULL) {
        log_error("root is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (key == NULL) {
        log_error("key is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (result == NULL) {
        log_error("result is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (*result != NULL) {
        log_warn("*result is not NULL. Potencial memory leak");
    }

    return EXIT_SUCCESS;
}
err_t trie_delete(Trie *root, const String key) {
    if (root == NULL) {
        log_error("root is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    if (key == NULL) {
        log_error("key is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    return EXIT_SUCCESS;
}
void trie_free(Trie *root) {
    if (root == NULL) {
        return;
    }

    return;
}
