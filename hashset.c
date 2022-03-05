#include <hashset.h>



HashSet hashset_create(void **storage, size_t len, size_t (*hash)(void *key), bool (*eq)(void *key1, void *key2)) {
    HashSet hashset;
    
    hashset.storage = storage;
    hashset.len = len;
    hashset.hash = hash;
    hashset.eq = eq;

    return hashset;
}


void hashset_start_step(HashSet hashset, void *element, size_t *start, size_t *step) {
    size_t hash = hashset.hash(element);
    *step = (hash % (hashset.len -1)) +1;
    *start = (hash) % (hashset.len);
}


void *hashset_insert(HashSet hashset, void *element) { 
    size_t start, step;
    hashset_start_step(hashset, element, &start, &step);

    void **gap = NULL;

    size_t i = start;
    do {
        void **bucket = hashset.storage + i;
        void *element_pos = *bucket;

        if (element_pos == HASHSET_STOP) {
            if (gap == NULL) {
                gap = bucket;
            }

            *gap = element;

            continue;
        }

        if (element_pos == HASHSET_GAP(hashset)) {
            if (gap == NULL) {
                gap = bucket;
            }

            continue;
        }

        if (hashset.eq(element, element_pos)) { 
            void *prev_element = element_pos;

            if (gap == NULL) {
                gap = bucket;
            }
            else {
                *bucket = HASHSET_GAP(hashset);
            }

            *gap = element;
            
            return prev_element;
        }
    } HASHSET_WHILE(i, start, step, hashset.len);
    
    return HASHSET_GAP(hashset);
}


void *hashset_find(HashSet hashset, void *element) {
    size_t start, step;
    hashset_start_step(hashset, element, &start, &step);

    size_t i;
    do {
        void *element_pos = *(hashset.storage + i);

        if (element_pos == HASHSET_STOP) {
            return NULL;
        }

        if (element_pos == HASHSET_GAP(hashset)) {
            continue;
        }

        if (hashset.eq(element, element_pos)) {
            return element_pos;
        }
    } HASHSET_WHILE(i, start, step, hashset.len);

    return NULL;
}


void *hashset_pop(HashSet hashset, void *element) {
    size_t start, step;
    hashset_start_step(hashset, element, &start, &step);

    size_t i;

    do {
        void **bucket = hashset.storage + i;
        void *element_pos = *bucket;

        if (element_pos == HASHSET_STOP) {
            return NULL;
        }

        if (element_pos == HASHSET_GAP(hashset)) {
            continue;
        }

        if (hashset.eq(element, element_pos)) {
            *bucket = HASHSET_GAP(hashset);
            return element_pos;
        }
    } HASHSET_WHILE(i, start, step, hashset.len);

    return NULL;
}


size_t hashset_used(HashSet hashset) {
    size_t used = 0;

    for (size_t i = 0; i < hashset.len; i++) {
        void *element_pos = *(hashset.storage + i);
        if (element_pos != HASHSET_STOP && element_pos != HASHSET_GAP(hashset)) {
            used++;
        }
    }

    return used;
}


void hashset_clear(HashSet hashset) {
    for (size_t i = 0; i < hashset.len; i++) {
        hashset.storage[i] = NULL;
    }
}


void *hashset_drain(HashSet *hashset) {
    while ((hashset->len) > 0) {
        void *element_pos = *(hashset->storage + (--hashset->len));

        if (element_pos == HASHSET_STOP || element_pos == HASHSET_GAP(*hashset)) {
            continue;
        }

        return element_pos;
    }

    return NULL;
}


HashSet hashset_relocate(HashSet *hashset_old, void **storage_new, size_t len_new) {
    HashSet hashset_new = hashset_create(storage_new, len_new, hashset_old->hash, hashset_old->eq);
    size_t len_old = hashset_old->len;

    void* element;
    while ((element = hashset_drain(hashset_old)) != NULL) {
        if (hashset_insert(hashset_new, element) == HASHSET_GAP(hashset_new)) {
            return HASHSET_NULL;
        }
    }

    return hashset_new;
}