#include "hashtable.h"

static void reinit_item(hashtable_item_t *item, const char *key, size_t key_size, void *value) {
    item->deleted = false;
    item->key = (char *)malloc(key_size);
    item->key_size = key_size;
    memcpy(item->key, key, key_size);
    item->value = value;
}

hashtable_item_t* create_item(const char *key, size_t key_size, void *value) {
    hashtable_item_t *item = (hashtable_item_t *)malloc(sizeof(hashtable_item_t));

    reinit_item(item, key, key_size, value);

    return item;
}

hashtable_t* create_hashtable(size_t capacity, hashfunc_t hf) {
    assert(capacity > 0);

    hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));

    ht->buckets_num = capacity;
    ht->items_num = 0;
    ht->hf = hf;

    ht->buckets = calloc(sizeof(hashtable_item_t*), capacity);

    return ht;
}

void free_hashtable_item(hashtable_item_t *item) {
    free(item->key);
    free(item);
}

void free_hashtable(hashtable_t *ht) {
    for (size_t i = 0; i < ht->buckets_num; i++) {
        hashtable_item_t *item = ht->buckets[i];
        if (item != NULL) {
            free_hashtable_item(item);
        }
    }
    free(ht->buckets);
    free(ht);
}

static size_t search_bucket(hashtable_t *ht, char *key, size_t key_size) {
    size_t index = 0;
    size_t bucket_id = 0;
    uint32_t hash = ht->hf((const uint8_t*)key, key_size);

    while (index < ht->buckets_num) {
        bucket_id = (hash + index) % ht->buckets_num;

        hashtable_item_t *item = ht->buckets[bucket_id];

        if (item == NULL)
            break;

        if (!item->deleted && 
            item->key_size == key_size && 
            memcmp(item->key, key, key_size) == 0)
            break;

        index++;
    }

    return bucket_id;
}

static size_t search_bucket_for_insert(hashtable_t *ht, char *key, size_t key_size) {
    size_t index = 0;
    size_t bucket_id = 0;
    uint32_t hash = ht->hf((const uint8_t*)key, key_size);

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

static void rehash(hashtable_t *ht) {
    size_t buckets_num = 0;

    size_t load_factor = 100 * ht->items_num / ht->buckets_num;

    if (load_factor > 75) {
        buckets_num = ht->buckets_num * 2;
    } else if (load_factor < 25 && ht->buckets_num > MINIMAL_BUCKETS_SIZE_FOR_SHRINKING) {
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

    for (size_t i = 0; i < ht->buckets_num; i++) {
        hashtable_item_t *item = ht->buckets[i];

        if (item == NULL) {
            continue;
        }

        if (item->deleted) {
            free_hashtable_item(item);
            continue;
        }

        size_t index = 0;
        size_t bucket_id = 0;
        uint32_t hash = ht->hf((const uint8_t*)item->key, item->key_size);

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
    assert(ht->items_num < ht->buckets_num);

    size_t bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    // already in
    if (item != NULL &&
        !item->deleted && 
        item->key_size == key_size && memcmp(item->key, key, key_size) == 0) {
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
        reinit_item(item, key, key_size, value);
        ht->items_num++;
    }

    rehash(ht);

    return;
}

void *ht_get(hashtable_t *ht, char *key, size_t key_size) {
    size_t bucket_id = search_bucket(ht, key, key_size);
    hashtable_item_t *item = ht->buckets[bucket_id];

    if (item != NULL && !item->deleted)
        return item->value;

    return NULL;
}

void ht_delete(hashtable_t *ht, char *key, size_t key_size) {
    size_t bucket_id = search_bucket(ht, key, key_size);
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