#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "hash.h"

#define MINIMAL_BUCKETS_SIZE_FOR_SHRINKING 10

typedef uint32_t (*hashfunc_t)(const uint8_t*, size_t);

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

hashtable_t* create_hashtable(size_t capacity, hashfunc_t hf);

void free_hashtable(hashtable_t *ht);

void free_hashtable_item(hashtable_item_t *item);

hashtable_item_t* create_item(const char *key, size_t key_size, void *value);

void ht_store(hashtable_t *ht, char *key, size_t key_size, void* value);

void *ht_get(hashtable_t *ht, char *key, size_t key_size);

void ht_delete(hashtable_t *ht, char *key, size_t key_size);

#define HT_STORE_S(ht, key_str, value) do { \
    const char* _key = (key_str); \
    typeof(value) _val = (value); \
    ht_store((ht), (char*)_key, strlen(_key) + 1, (void*)(uintptr_t)_val); \
} while(0)

#define HT_GET_S(ht, key_str, type) ((type)(uintptr_t)ht_get((ht), (char*)(key_str), strlen(key_str) + 1))

#define HT_DELETE_S(ht, key_str) ht_delete((ht), (char*)(key_str), strlen(key_str) + 1)

#define HT_STORE_I(ht, key_int, value) do { \
    typeof(key_int) _key = (key_int); \
    typeof(value) _val = (value); \
    ht_store((ht), (char*)&_key, sizeof(_key), (void*)(uintptr_t)_val); \
} while(0)

#define HT_GET_I(ht, key_int, type) ((type)(uintptr_t)ht_get((ht), (char*)&(typeof(key_int)){key_int}, sizeof(typeof(key_int))))

#define HT_DELETE_I(ht, key_int) ht_delete((ht), (char*)&(typeof(key_int)){key_int}, sizeof(typeof(key_int)))