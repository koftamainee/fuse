#include "../trie.h"

#include <stdlib.h>
#include <string.h>

#include "../logger.h"

err_t trie_init(trie **t, const char *alphabet) {
    if (t == NULL || alphabet == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    size_t alphabet_size = strlen(alphabet);
    int i = 0;

    *t = (trie *)malloc(sizeof(trie));
    if (*t == NULL) {
        log_fatal("failed to allocate memory");
        return MEMORY_ALLOCATION_ERROR;
    }

    trie *tr = *t;
    tr->alphabet_size = alphabet_size;
    tr->alphabet = string_from(alphabet);
    if (tr->alphabet == NULL) {
        log_fatal("memory allocateion error");
        free(*t);
        return MEMORY_ALLOCATION_ERROR;
    }

    tr->root = NULL;

    return EXIT_SUCCESS;
}

void __trie_free_inner(trie *tr, trie_node *t) {
    int i = 0;
    for (i = 0; i < tr->alphabet_size; ++i) {
        // TODO: ...
    }
}

void trie_free(trie *t) {
    if (t == NULL) {
        return;
    }

    trie_node *current = NULL;

    __trie_free_inner(t, t->root);
    string_free(t->alphabet);
    free(t);
}

err_t trie_set(trie *t, String key, int value);
err_t trie_get(trie *t, String key, int *value_placeholder);
err_t trie_dispose(trie *t, String key);
