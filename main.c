#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "hash.h"

typedef uint32_t (*hashfunc_t)(const uint8_t*, size_t);

// typedef struct {
//     char *key;
//     size_t size;
// } ht_key_t;

// typedef struct {
//     char *data;
//     size_t size;
// } ht_data_t;

typedef struct {
    bool deleted;
    char *key;
    size_t key_size;
    void *value;
} hashtable_item_t;

typedef struct {
    size_t items_num;
    size_t buckets_num;
    hashfunc_t hf;
    hashtable_item_t **buckets; 
} hashtable_t;

hashtable_item_t* create_item(const char *key, size_t key_size, void *value) {
    hashtable_item_t *item = (hashtable_item_t *)malloc(sizeof(hashtable_item_t));
    item->deleted = false;
    item->key = (char *)malloc(key_size);
    item->key_size = key_size;
    item->value = value;

    memcpy(item->key, key, key_size);

    return item;
}

void free_hashtable_item(hashtable_item_t *item) {
    free(item->key);
    free(item);
}

hashtable_t* create_hashtable(size_t capacity, hashfunc_t hf) {
    hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));

    ht->buckets_num = capacity;
    ht->items_num = 0;
    ht->hf = hf;

    ht->buckets = calloc(sizeof(hashtable_item_t*), capacity);

    return ht;
}

int search_bucket(hashtable_t *ht, char *key, size_t key_size) {
    uint32_t hash = ht->hf(key, key_size);
    int index = 0;
    int bucket_id = 0;

    while (index < ht->buckets_num) {
        bucket_id = (hash + index) % ht->buckets_num;

        hashtable_item_t *item = ht->buckets[bucket_id];

        if (item == NULL)
            break;

        if (item->key_size == key_size && memcmp(item->key, key, key_size) == 0)
            break;

        index++;
    }

    return bucket_id;
}

int search_bucket_for_insert(hashtable_t *ht, char *key, size_t key_size) {
    uint32_t hash = ht->hf(key, key_size);
    int index = 0;
    int bucket_id = 0;

    while (index < ht->buckets_num) {
        bucket_id = (hash + index) % ht->buckets_num;

        hashtable_item_t *item = ht->buckets[bucket_id];

        if (item == NULL)
            break;

        if (item->deleted)
            break;

        if (item->key_size == key_size && memcmp(item->key, key, key_size) == 0)
            break;

        index++;
    }

    return bucket_id;
}

void ht_store(hashtable_t *ht, char *key, size_t key_size, void* value) {
    int bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    // already in
    if (item != NULL && item->key_size == key_size && memcmp(item->key, key, key_size) == 0) {
        item->value = value;
        return;
    }

    bucket_id = search_bucket_for_insert(ht, key, key_size);
    item = ht->buckets[bucket_id];

    if (item == NULL) {
        ht->buckets[bucket_id] = create_item(key, key_size, value);
        ht->items_num++;
        return;
    }

    if (item->deleted) {
        item->deleted = false;
        item->key = key;
        item->value = value;
        ht->items_num++;
    }

    return;
}

void *ht_get(hashtable_t *ht, char *key, size_t key_size) {
    int bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    if (item != NULL)
        return item->value;

    return NULL;
}

void ht_delete(hashtable_t *ht, char *key, size_t key_size) {
    int bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    if (item != NULL) {
        item->deleted = true;
        free(item->key);
        item->key = NULL;
        item->key_size = 0;
        item->value = NULL;

        ht->items_num--;
    }
}

int test_hashmap() {
    hashtable_t *map = create_hashtable(131, murmur3_32);

    ht_store(map, "key1", 4, (void *)1);

    assert(map->items_num == 1);

    ht_store(map, "key2", 4, (void *)2);

    assert(map->items_num == 2);

    long int val = (long int)ht_get(map, "key1", 4);

    assert(val == 1);

    val = (long int)ht_get(map, "key2", 4);

    assert(val == 2);

    ht_delete(map, "key2", 4);

    assert(map->items_num == 1);

    val = (long int)ht_get(map, "key2", 4);

    assert(val == 0);

    return 0;
}


int main(int argc, char *argv[]) {
    test_hashmap();
}