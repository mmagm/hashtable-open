#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "hashtable.h"

#define N 10000

// вспомогательная функция для генерации ключа
void make_key(char *buf, int i) {
    snprintf(buf, 32, "key_%d", i);
}

// тест 1: вставка и получение N элементов
void test_insert_get() {
    printf("test_insert_get... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        ht_store(ht, key, strlen(key)+1, (void *)(long)i);
    }

    assert(ht->items_num == N);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        long val = (long)ht_get(ht, key, strlen(key)+1);
        assert(val == i);
    }

    free_hashtable(ht);
    printf("ok\n");
}

// тест 2: вставка, удаление, повторная вставка
void test_delete_reinsert() {
    printf("test_delete_reinsert... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        ht_store(ht, key, strlen(key)+1, (void *)(long)i);
    }

    // удаляем половину
    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        ht_delete(ht, key, strlen(key)+1);
    }

    assert(ht->items_num == N / 2);

    // проверяем что удалённые не находятся
    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        assert(ht_get(ht, key, strlen(key)+1) == NULL);
    }

    // вставляем заново
    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        ht_store(ht, key, strlen(key)+1, (void *)((long)i * 10));
    }

    assert(ht->items_num == N);

    // проверяем новые значения
    for (int i = 0; i < N; i += 2) {
        char key[32];
        make_key(key, i);
        long val = (long)ht_get(ht, key, strlen(key)+1);
        assert(val == i * 10);
    }

    free_hashtable(ht);
    printf("ok\n");
}

// тест 3: обновление существующего ключа
void test_update() {
    printf("test_update... ");
    hashtable_t *ht = create_hashtable(1, murmur3_32);

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        ht_store(ht, key, strlen(key)+1, (void *)(long)i);
    }

    // обновляем все значения
    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        ht_store(ht, key, strlen(key)+1, (void *)((long)i * 2));
    }

    assert(ht->items_num == N);  // количество не должно измениться

    for (int i = 0; i < N; i++) {
        char key[32];
        make_key(key, i);
        long val = (long)ht_get(ht, key, strlen(key)+1);
        assert(val == i * 2);
    }

    free_hashtable(ht);
    printf("ok\n");
}

int main(void) {
    test_insert_get();
    test_delete_reinsert();
    test_update();
    printf("all tests passed\n");
    return 0;
}