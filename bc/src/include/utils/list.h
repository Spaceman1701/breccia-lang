#pragma once
#include <stdint.h>
#include <stdlib.h>

typedef void Bc_ListElement;

// list of generic elements
typedef struct {
    Bc_ListElement *data;
    size_t length;
    size_t capacity;
    size_t element_size;
} Bc_List;

int bc_list_new(size_t element_size, size_t inital_capactiy, Bc_List *list);

void bc_list_free(Bc_List *list);

int bc_list_expand_capacity(Bc_List *list, size_t new_capacity);

Bc_ListElement *bc_list_append_ptr(Bc_List *list);

Bc_ListElement *bc_list_get(Bc_List *list, size_t index);

int bc_shrink_to_fit(Bc_List *list);