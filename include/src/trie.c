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
    trie *tr = NULL;

    *t = (trie *)malloc(sizeof(trie));
    if (*t == NULL) {
        log_fatal("failed to allocate memory");
        return MEMORY_ALLOCATION_ERROR;
    }

    tr = *t;
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
    if (t == NULL || t->subtrees == NULL) {
        return;
    }

    for (size_t i = 0; i < tr->alphabet_size; ++i) {
        __trie_free_inner(tr, t->subtrees + i);
    }

    free(t->subtrees);
}

void trie_free(trie *t) {
    if (t == NULL) {
        return;
    }

    trie_node *current = NULL;

    __trie_free_inner(t, t->root);
    free(t->root);
    string_free(t->alphabet);
    free(t);
}

err_t trie_set(trie *t, String key, int value) {
    if (t == NULL || key == NULL) {
        log_fatal("trie or key is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    int i = 0;
    trie_node *current = NULL;
    char c;
    int index = 0;

    if (t->root == NULL) {
        t->root = (trie_node *)calloc(1, sizeof(trie_node));
        if (t->root == NULL) {
            log_fatal("memory allocation error for root node");
            return MEMORY_ALLOCATION_ERROR;
        }
    }

    current = t->root;

    for (i = 0; i < string_len(key); ++i) {
        c = key[i];
        index = strchr(t->alphabet, c) - t->alphabet;

        if (index < 0 || index >= (int)t->alphabet_size) {
            log_error("invalid character '%c' in key", c);
            return KEY_NOT_FOUND;
        }

        if (current->subtrees == NULL) {
            current->subtrees =
                (trie_node *)calloc(t->alphabet_size, sizeof(trie_node));
            if (current->subtrees == NULL) {
                log_fatal("memory allocation error for subtrees");
                return MEMORY_ALLOCATION_ERROR;
            }
        }

        current = &current->subtrees[index];
    }

    current->value = value;
    current->is_end_of_word = 1;

    return EXIT_SUCCESS;
}

err_t trie_get(trie *t, String key, int *value_placeholder) {
    if (t == NULL || key == NULL || value_placeholder == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    trie_node *current = t->root;
    for (size_t i = 0; i < string_len(key); ++i) {
        char c = key[i];
        int index = strchr(t->alphabet, c) - t->alphabet;

        if (index < 0 || index >= t->alphabet_size || current == NULL ||
            current->subtrees == NULL) {
            return KEY_NOT_FOUND;
        }

        current = &current->subtrees[index];
    }

    if (current == NULL || !current->is_end_of_word) {
        return KEY_NOT_FOUND;
    }

    *value_placeholder = current->value;
    return EXIT_SUCCESS;
}

err_t trie_dispose(trie *t, String key) {
    if (t == NULL || key == NULL) {
        log_fatal("passed ptr is NULL");
        return DEREFERENCING_NULL_PTR;
    }

    // TODO: maybe later...

    return EXIT_SUCCESS;
}
