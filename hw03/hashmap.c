#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "hashmap.h"

static const int k = 37;
static const long hashmap_size = 10000;

static long hash(hashmap_t hm, char *key) {
    long h = 0, m = 1;
    uint8_t *p = (uint8_t *)key;
    uint8_t c;

    while ( (c = *p++) != '\0') {
        h = (h * m + c) % hashmap_size;
        m = (m * k) % hashmap_size;
    }

    while (hm[h] != NULL && strcmp(hm[h]->key,key) != 0)
        h = (h+1) % hashmap_size;

    return h;
}

hashmap_t hashmap_new(void) {
    hashmap_t hm;

    // +1 for terminator
    hm = (hashmap_t) calloc(hashmap_size+1, sizeof(hashmap_elem_p_t));

    return hm;
}

hashmap_elem_p_t hashmap_get(hashmap_t hm, char *key) {
    return hm[hash(hm, key)];
}

hashmap_elem_p_t hashmap_set(hashmap_t hm, char *key, int *value) {
    long hv = hash(hm, key);

    if (hm[hv] == NULL) {
        hm[hv] = (hashmap_elem_p_t) malloc(sizeof(hashmap_elem_t));
        hm[hv]->key = malloc(strlen(key) + 1);
        strcpy(hm[hv]->key, key);
    }

    // NOTE: если бы нужно было переопределение еще одной строки в структуре, тут должен быть еще free()
    hm[hv]->val = *value;

    return hm[hv];
}

hashmap_elem_p_t hashmap_unset(hashmap_t hm, char *key) {
    long hv = hash(hm, key);

    free(hm[hv]->key);
    free(hm[hv]);

    return hm[hv];
}

hashmap_elem_p_t hashmap_add(hashmap_t hm, char *key, int *value) {
    long hv = hash(hm, key);

    if (hm[hv] == NULL) {
        return hashmap_set(hm, key, value);
    }

    hm[hv]->val += *value;

    return hm[hv];
}

hashmap_elem_p_t *hashmap_iterator(hashmap_t *hp, hashmap_elem_p_t **start) {
    hashmap_elem_p_t *cursor = *start;

    while (*cursor == NULL && cursor != &(*hp)[hashmap_size])
        cursor++;

    *start = cursor;
    (*start)++;

    if (cursor == &(*hp)[hashmap_size])
        return NULL;

    return cursor;
}

void hashmap_delete(hashmap_t hm) {
    hashmap_elem_p_t *cursor = &hm[0];
    while (cursor != &hm[hashmap_size]) {
        if (*cursor != NULL) {
            free((*cursor)->key);
            free(*cursor);
        }
        cursor++;
    }
    free(hm);
    return;
}
