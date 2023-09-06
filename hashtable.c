#include "hashtable.h"

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

void free_hashtable(hashtable_t *ht) {
    free(ht->buckets);
    free(ht);
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

void rehash(hashtable_t *ht) {
    size_t buckets_num = 0;

    size_t load_factor = 100 * ht->items_num / ht->buckets_num;

    if (load_factor > 75) {
        buckets_num = ht->buckets_num * 2;
    } else if (load_factor < 25) {
        buckets_num = ht->buckets_num / 2;
    } else {
        return;
    }

    printf("rehashing from %lu to %lu, items: %lu, load_factor: %lu\n",
        ht->buckets_num,
        buckets_num,
        ht->items_num,
        load_factor);

    hashtable_item_t **buckets = calloc(sizeof(hashtable_item_t*), buckets_num);

    for (int i = 0; i < ht->buckets_num; i++) {
        hashtable_item_t *item = ht->buckets[i];

        if (item == NULL || item->deleted) {
            continue;
        }

        int index = 0;
        int bucket_id = 0;
        uint32_t hash = ht->hf(item->key, item->key_size);

        while (index < buckets_num) {
            bucket_id = (hash + index) % buckets_num;

            hashtable_item_t *newitem = buckets[bucket_id];

            if (newitem == NULL)
                break;

            index++;
        }

        assert(buckets[bucket_id] == NULL);

        buckets[bucket_id] = item;
    }

    free(ht->buckets);

    ht->buckets = buckets;
    ht->buckets_num = buckets_num;
}

void ht_store(hashtable_t *ht, char *key, size_t key_size, void* value) {
    int bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    assert(ht->items_num < ht->buckets_num);

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
        rehash(ht);
        return;
    }

    if (item->deleted) {
        item->deleted = false;
        item->key = key;
        item->value = value;
        ht->items_num++;
    }

    rehash(ht);

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

    rehash(ht);
}