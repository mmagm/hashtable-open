#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "hashtable.h"

#ifdef _WIN32
static ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = (char*)realloc(*lineptr, *n);
        if (*lineptr == NULL) {
            return -1;
        }
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n * 2;
            char *new_ptr = (char*)realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *lineptr = new_ptr;
            *n = new_size;
        }

        (*lineptr)[pos++] = (char)c;
        if (c == '\n') {
            break;
        }
    }

    if (pos == 0 && c == EOF) {
        return -1;
    }

    (*lineptr)[pos] = '\0';
    return (ssize_t)pos;
}
#define getline my_getline
#endif

int comp_list_nodes(const void *elem1, const void *elem2) {
    const hashtable_item_t** n1 = (const hashtable_item_t**)elem1;
    const hashtable_item_t** n2 = (const hashtable_item_t**)elem2;

    int f = (int)(uintptr_t)(*n1)->value;
    int s = (int)(uintptr_t)(*n2)->value;

    return s - f;
}

int main(void) {
    char *buffer = NULL;
    size_t bufsize = 0;

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
                long int count = HT_GET_S(ht, word, long);
                HT_STORE_S(ht, word, count + 1);
            }
            word = strtok(NULL, " \t");
        }
    }

    printf("lines = %zu\n", lines);

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

    printf("words count: %zu\n", ht->items_num);

    for (size_t i = 0; i < top_num; i++) {
        hashtable_item_t *node = nodes[i];
        printf("%s : %ld\n", node->key, (long int)(uintptr_t)node->value);
    }

    free(buffer);
    free(nodes);

    free_hashtable(ht);

    return 0;
}