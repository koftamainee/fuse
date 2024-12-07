#include "../trie.h"

#include <stdlib.h>

#include "../errors.h"
#include "../logger.h"

err_t trie_init(Trie *t, String alphabet, int (*is_in_alphabet)(char c)) {
    if (t == NULL) {
        log_error("trie is NULL");
        return DEREFERENCING_NULL_PTR;
    }
    if (alphabet == NULL) {
        log_error("alphabet is NULL");
        return DEREFERENCING_NULL_PTR;
    }
    if (is_in_alphabet == NULL) {
        log_error("is_in_alphabet function is NULL");
    }

    return EXIT_SUCCESS;
}
err_t trie_insert(Trie *t, const String key, tvalue value);
err_t trie_search(const Trie *t, const String key, tvalue *result);
err_t trie_delete(Trie *t, const String key);
void trie_free(Trie *t);
