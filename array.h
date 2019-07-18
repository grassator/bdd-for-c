#ifndef BDD_ARRAY_H
#define BDD_ARRAY_H

#include "stdlib.h"

typedef struct _bdd_array {
    void ** values;
    size_t capacity;
    size_t size;
} _bdd_array;

_bdd_array* _bdd_array_create() {
    _bdd_array *arr = malloc(sizeof(_bdd_array));
    if (!arr) {
        perror("malloc(array)");
        abort();
    }
    arr->capacity = 4;
    arr->size = 0;
    arr->values = calloc(arr->capacity, sizeof(void *));
    return arr;
}

_bdd_array* _bdd_array_push(_bdd_array* arr, void *item) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        void *v = realloc(arr->values, sizeof(void *) * arr->capacity);
        if (!v) {
            perror("realloc(array)");
            abort();
        }
        arr->values = v;
    }
    arr->values[arr->size++] = item;
    return arr;
}

void* _bdd_array_last(_bdd_array* arr) {
    if (arr->size == 0) {
        return NULL;
    }
    return arr->values[arr->size - 1];
}

void* _bdd_array_pop(_bdd_array* arr) {
    if (arr->size == 0) {
        return NULL;
    }
    void* result = arr->values[arr->size - 1];
    --arr->size;
    return result;
}

void _bdd_array_free(_bdd_array* arr) {
    free(arr->values);
    free(arr);
}

#endif //BDD_ARRAY_H
