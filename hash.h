#ifndef _HASH_H
#define _HASH_H

#include <stdlib.h>
#include <stdint.h>

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
uint32_t murmur3_32(const uint8_t* key, size_t len);

#endif