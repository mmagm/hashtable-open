#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "hashtable.h"

int test_hashmap() {
    hashtable_t *map = create_hashtable(1, murmur3_32);

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