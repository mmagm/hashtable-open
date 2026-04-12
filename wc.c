#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "hashtable.h"

int comp_list_nodes(const void *elem1, const void *elem2) {
    const hashtable_item_t** n1 = (const hashtable_item_t**)elem1;
    const hashtable_item_t** n2 = (const hashtable_item_t**)elem2;

    int f = (long int)(*n1)->value;
    int s = (long int)(*n2)->value;

    return s - f;
}

void toLower(char* dest, char *source) {
    while (*source != 0) {
        *dest = tolower(*source);
        dest++;
        source++;
    }
    *dest = 0;
}

int main(void) {
    char *buffer;
    size_t bufsize = 64;

    buffer = (char *)calloc(sizeof(char), bufsize);
    if (buffer == NULL) {
        perror("Unable to allocate buffer\n");
        exit(1);
    }

    hashtable_t *ht = create_hashtable(1, murmur3_32);

    ssize_t read;

    size_t lines = 0;

    while ((read = getline(&buffer, &bufsize, stdin)) != -1) {
        lines++;

        size_t buffer_len = strlen(buffer);
        if (buffer_len <= 1)
            continue;

        if (buffer_len > 0 && buffer[buffer_len-1] == '\n')
            buffer[--buffer_len] = 0;
        if (buffer_len > 0 && buffer[buffer_len-1] == '\r')
            buffer[--buffer_len] = 0;

        char *word = strtok(buffer, " \t");
        while (word != NULL) {
            if (strlen(word) > 0) {
                size_t len = strlen(word)+1;
                long int count = (long int)ht_get(ht, word, len);
                ht_store(ht, word, len, (void *)(count + 1));
            }
            word = strtok(NULL, " \t");
        }
    }

    printf("lines = %lu\n", lines);

    hashtable_item_t **nodes = calloc(sizeof(hashtable_item_t*), ht->items_num);

    size_t item_id = 0;

    for (size_t i = 0; i < ht->buckets_num; i++) {
        hashtable_item_t *node = ht->buckets[i];

        if (node == NULL || node->deleted)
            continue;

        nodes[item_id] = node;
        item_id++;
    }

    qsort(nodes, ht->items_num, sizeof(*nodes), comp_list_nodes);

    size_t top_num = ht->items_num;

    printf("words count: %lu\n", ht->items_num);

    // if (top_num > 10)
    //     top_num = 10;

    for (size_t i = 0; i < top_num; i++) {
        hashtable_item_t *node = nodes[i];
        printf("%s : %ld\n", node->key, (long int)node->value);
    }

    free(buffer);
    free(nodes);

    free_hashtable(ht);

    return 0;
}
