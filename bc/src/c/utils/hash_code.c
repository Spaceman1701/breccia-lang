#include "utils/hash_code.h"

#include <stdint.h>
#include <stdlib.h>

size_t hash_code_string(const char *str) {
    size_t hash = 0;
    const char *cur_char = str;
    while (cur_char != NULL) {
        hash += *cur_char;
        cur_char += 1;
    }
    return hash;
}