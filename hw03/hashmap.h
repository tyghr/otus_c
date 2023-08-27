#pragma once
#include <stdbool.h>

typedef struct hashmap_val hashmap_elem_t, *hashmap_elem_p_t, **hashmap_t;
struct hashmap_val{
    char *key;
    int val;
};
hashmap_t hashmap_new(void);
void hashmap_delete(hashmap_t h);
hashmap_elem_p_t hashmap_get(hashmap_t h, char *key);
hashmap_elem_p_t hashmap_set(hashmap_t h, char *key, int *value);
hashmap_elem_p_t hashmap_unset(hashmap_t h, char *key);
hashmap_elem_p_t hashmap_add(hashmap_t h, char *key, int *value);
hashmap_elem_p_t *hashmap_iterator(hashmap_t *hp, hashmap_elem_p_t **start);
