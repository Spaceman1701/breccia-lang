#include "utils/list.h"

#include <stdlib.h>

int bc_list_new(size_t element_size, size_t inital_capactiy, Bc_List *list) {
    if (!list) {
        return -1; // need non-null list
    }
    if (element_size == 0) {
        return -2; // 0 size element is impossible
    }

    if (inital_capactiy == 0) {
        inital_capactiy =
            1; // on some implementations malloc of 0 size will return NULL
    }

    list->data = (Bc_ListElement *)malloc(inital_capactiy * element_size);
    if (!list->data) {
        list->capacity = 0;
        list->length = 0;
        list->element_size = 0;
        return -1;
    }

    list->capacity = inital_capactiy;
    list->length = 0;
    list->element_size = element_size;

    return 0;
}

void bc_list_free(Bc_List *list) {
    free(list->data);
    list->capacity = 0;
    list->length = 0;
}

int bc_list_expand_capacity(Bc_List *list, size_t new_capacity) {
    Bc_ListElement *new_data = (Bc_ListElement *)realloc(
        list->data, list->element_size * new_capacity);
    if (!new_data) {
        return -1;
    }
    list->data = new_data;
    list->capacity = new_capacity;
    return 0;
}

Bc_ListElement *bc_list_append_ptr(Bc_List *list) {
    if (list->capacity == list->length) {
        int err = bc_list_expand_capacity(list, list->capacity * 2);
        if (err) {
            return NULL; // properly you should be checking for null when using
                         // this...
        }
    }

    Bc_ListElement *append_ptr =
        list->data + (list->length * list->element_size);
    list->length++;
    return append_ptr;
}

Bc_ListElement *bc_list_get(Bc_List *list, size_t index) {
    if (index >= list->length) {
        return NULL;
    }
    return list->data + (index * list->element_size);
}

int bc_shrink_to_fit(Bc_List *list) {
    if (list->capacity <= list->length) {
        return 0;
    }

    return bc_list_expand_capacity(list, list->length);
}