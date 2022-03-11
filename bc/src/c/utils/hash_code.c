#include "utils/hash_code.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t hash_code_string(const char *str, size_t bucket_count) {
    size_t hash = 0;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        hash += str[i];
    }
    return hash % bucket_count;
}