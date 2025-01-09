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

    *t = (trie *)malloc(sizeof(trie));
    if (*t == NULL) {
        log_fatal("failed to allocate memory");
        return MEMORY_ALLOCATION_ERROR;
    }

    trie *tr = *t;

    // TODO: finish this funxtion
    tr->root = NULL;

    return EXIT_SUCCESS;
}

void trie_free(trie *t);

err_t trie_set(trie *t, String key, int value);
err_t trie_get(trie *t, String key, int *value_placeholder);
err_t trie_dispose(trie *t, String key);
