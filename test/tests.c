#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "hashtable.h"

#define N 10000

int test_hashmap(void) {
    hashtable_t *map = create_hashtable(1, murmur3_32);

    HT_STORE_S(map, "key1", 1);
    assert(map->items_num == 1);

    HT_STORE_S(map, "key2", 2);
    assert(map->items_num == 2);

    assert(HT_GET_S(map, "key1", long) == 1);
    assert(HT_GET_S(map, "key2", long) == 2);

    HT_DELETE_S(map, "key2");
    assert(map->items_num == 1);
    assert(HT_GET_S(map, "key2", void*) == NULL);

    free_hashtable(map);
    return 0;
}

void make_key(char *buf, int i) {
    snprintf(buf, 32, "key_%d", i);
}

int test_insert_get(void) {
    printf("test_insert_get... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        HT_STORE_S(ht, key, (long)i);
    }

    assert(ht->items_num == N);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        assert(HT_GET_S(ht, key, long) == i);
    }

    free_hashtable(ht);
    printf("ok\n");
    return 0;
}

int test_delete_reinsert(void) {
    printf("test_delete_reinsert... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        HT_STORE_S(ht, key, (long)i);
    }

    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        HT_DELETE_S(ht, key);
    }

    assert(ht->items_num == N / 2);

    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        assert(HT_GET_S(ht, key, void*) == NULL);
    }

    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        HT_STORE_S(ht, key, (long)(i * 10));
    }

    assert(ht->items_num == N);

    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        assert(HT_GET_S(ht, key, long) == i * 10);
    }

    free_hashtable(ht);
    printf("ok\n");
    return 0;
}

int test_update(void) {
    printf("test_update... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        HT_STORE_S(ht, key, (long)i);
    }

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        HT_STORE_S(ht, key, (long)(i * 2));
    }

    assert(ht->items_num == N);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        assert(HT_GET_S(ht, key, long) == i * 2);
    }

    free_hashtable(ht);
    printf("ok\n");
    return 0;
}

int main(void) {
    test_hashmap();
    test_insert_get();
    test_delete_reinsert();
    test_update();
    printf("all tests passed\n");
    return 0;
}