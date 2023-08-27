#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"

#define WORD_MAX_LEN 100

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("need filename for read");
        return EXIT_FAILURE;
    }

    FILE *fp=fopen(argv[1],"r");
    if ( fp == NULL ) {
        perror("open error");
        return EXIT_FAILURE;
    }

    hashmap_t h = hashmap_new();
    if (h == NULL) {
        perror("hashmap not created");
        return EXIT_FAILURE;
    }

    char buf[WORD_MAX_LEN];
    int c;
    int cur = 0;
    int inc = 1;
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) {
            if (cur!=0) {
                buf[cur++] = '\0';
                hashmap_add(h, buf, &inc);
                cur = 0;
            }
        } else {
            if (cur < WORD_MAX_LEN - 1) {
                buf[cur++] = c;
            }
        }
    }

    hashmap_elem_p_t *iv, *cursor = h;
    while ((iv = hashmap_iterator(&h, &cursor)) != NULL) {
        printf("\"%s\"\t%d\n", (*iv)->key, (*iv)->val);
    }

    hashmap_delete(h);
    fclose(fp);
    return EXIT_SUCCESS;
}
