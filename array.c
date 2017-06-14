#include "bdd-for-c.h"
#include "array.h"

describe("array") {

    static _bdd_array* arr;

    before_each() {
        arr = _bdd_array_create();
    }

    after_each() {
        _bdd_array_free(arr);
    }

    it("should create with a default capacity of 4") {
        check(arr->values != NULL);
        check(arr->capacity == 4);
        check(arr->size == 0);
    }

    it("should allow to push values to the end") {
        _bdd_array_push(arr, "foo");
    }

    it("should allow to access the last item") {
        _bdd_array_push(arr, "foo");
        char* last = _bdd_array_last(arr);
        check(strcmp(last, "foo") == 0);
    }

    it("should allow to pop an array of the end") {
        _bdd_array_push(arr, "foo");
        _bdd_array_push(arr, "bar");
        char* popped = _bdd_array_pop(arr);
        check(strcmp(popped, "bar") == 0);
        char* last = _bdd_array_last(arr);
        check(strcmp(last, "foo") == 0);
    }

    it("should increase the capacity to twice the previous one") {
        _bdd_array_push(arr, "1");
        _bdd_array_push(arr, "2");
        _bdd_array_push(arr, "3");
        _bdd_array_push(arr, "4");
        _bdd_array_push(arr, "5");
        check(arr->capacity == 8);
        check(arr->size == 5);
    }
}
