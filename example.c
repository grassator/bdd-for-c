#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

const static int __BDD_MODE_PRE_RUN__ = 0;
const static int __BDD_MODE_BEFORE__ = 1;
const static int __BDD_MODE_AFTER__ = 2;
const static int __BDD_MODE_BEFORE_EACH__ = 3;
const static int __BDD_MODE_AFTER_EACH__ = 4;
const static int __BDD_MODE_TEST_RUN__ = 5;

int __bdd_same_string__(const char* str1, const char* str2) {
    size_t str1length = strlen(str1);
    size_t str2length = strlen(str2);
    if (str1length != str2length) {
        return 0;
    }
    return strncmp(str1, str2, str1length) == 0;
}

int __bdd_prefix__(const char* str1, const char* str2) {
    // printf("%s - %s - [%s]\n\n", str1, str2, strncmp(str1, str2, strlen(str1)) == 0 ? "TRUE" : "FALSE");
    return strncmp(str1, str2, strlen(str1)) == 0;
}

typedef struct __bdd_string_list__ {
    char** values;
    char* last;
    size_t capacity;
    size_t size;
} __bdd_string_list__;

static __bdd_string_list__ __bdd_printed_strings__;


static int __bdd_string_compare__(const void *p1, const void *p2){
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

char* __bdd_concat__(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void __bdd_string_list_init__(__bdd_string_list__* result) {
    result->capacity = 1000;
    result->size = 0;
    result->last = NULL;
    result->values = malloc(sizeof(char*) * result->capacity);
    for (size_t i = 0; i < result->capacity; ++i) {
        result->values[i] = NULL;
    }
}

int __bdd_string_list_push__(__bdd_string_list__ *list, char* string) {
    list->values[list->size++] = string;
    list->last = string;
    return 0;
    // FIXME reallocate if overflow
}

int __bdd_string_list_pop__(__bdd_string_list__ *list) {
    list->values[--list->size] = NULL;
    list->last = list->size == 0 ? NULL : list->values[list->size - 1];
    return 0;
}

int __bdd_string_list_contains__(__bdd_string_list__ *list, char* str) {
    for (size_t i = 0; i < list->size; ++i) {
        if (__bdd_same_string__(list->values[i], str)) {
            return 1;
        }
    }
    return 0;
}

char* __bdd_string_list_concat__(__bdd_string_list__ *list) {
    size_t result_size = 0;
    for (size_t i = 0; i < list->size; ++i) {
        result_size += strlen(list->values[i]);
    }
    char* result = malloc(sizeof(char) * result_size + 1);
    result[0] = '\0';
    for (size_t i = 0; i < list->size; ++i) {
        result = strcat(result, list->values[i]);
    }
    return result;
}

int print_with_level(size_t level, char *message) {
    for (size_t i = 1; i < level; ++i) {
        printf("  ");
    }
    printf("%s\n", message);
    return 0;
}

int print_unique(size_t level, char *key, char *message) {
    if (__bdd_string_list_contains__(&__bdd_printed_strings__, key)) {
        return 0;
    }
    __bdd_string_list_push__(&__bdd_printed_strings__, key);
    return print_with_level(level, message);
}

#define check(something)\
if (!(something)) {\
    print_with_level(__context_stack__->size + 1, "This doesn't work");\
    continue;\
}

#define it(name)\
for(\
    int __bdd_index__ = 0,\
        __bdd_ignore_push_context__ = __bdd_string_list_push__(__context_stack__, __bdd_concat__("(it)", name)),\
        __bdd_ignore_pre_run__ = mode == __BDD_MODE_PRE_RUN__ && __bdd_string_list_push__(__test_list__, __bdd_string_list_concat__(__context_stack__))\
    ;\
    (\
        mode == __BDD_MODE_TEST_RUN__ &&\
        __bdd_index__ < 1 &&\
        __bdd_same_string__(__bdd_test_name__, __bdd_string_list_concat__(__context_stack__)) &&\
        !print_with_level(__context_stack__->size, name)\
    ) || __bdd_string_list_pop__(__context_stack__);\
    ++__bdd_index__\
)

#define context(name)\
for(\
    int __bdd_index__ = 0, \
        __bdd_ignore_push_context__ = __bdd_string_list_push__(__context_stack__, __bdd_concat__("(context)", name))\
    ;\
    (\
        __bdd_index__ < 1 &&\
        (mode == __BDD_MODE_PRE_RUN__ || (\
            mode == __BDD_MODE_TEST_RUN__ &&\
            __bdd_prefix__(__bdd_string_list_concat__(__context_stack__), __bdd_test_name__) && \
            !print_unique(__context_stack__->size, __context_stack__->last, name)\
        ))\
    ) || __bdd_string_list_pop__(__context_stack__);\
    ++__bdd_index__\
)

#define before()\
if(mode == __BDD_MODE_BEFORE__)

#define after()\
if(mode == __BDD_MODE_AFTER__)

#define before_each()\
if(mode == __BDD_MODE_BEFORE_EACH__)

#define after_each()\
if(mode == __BDD_MODE_AFTER__EACH_)

void test(
    int mode,
    __bdd_string_list__* __context_stack__,
    __bdd_string_list__* __test_list__,
    const char * __bdd_test_name__
) {
    context("foo") {
        it("1") {
            check(1 == 2);
        }
        context("bar") {
            it("2") {
                check(1 == 2);
            }
        }
    }
    context("buzz") {
        after() {
            printf("AFTER buzz\n");
        }
        it("should not work") {
            check(1 == 2);
        }
        before() {
            printf("BEFORE buzz\n");
        }
    }
    after() {
        printf("AFTER all\n");
    }
    before() {
        printf("BEFORE all\n");
    }
}

int main(void) {
    __bdd_string_list_init__(&__bdd_printed_strings__);
    __bdd_string_list__ context_stack;
    __bdd_string_list_init__(&context_stack);
    __bdd_string_list__ test_list;
    __bdd_string_list_init__(&test_list);
    test(__BDD_MODE_PRE_RUN__, &context_stack, &test_list, "");
    qsort(test_list.values, test_list.size, sizeof(char *), __bdd_string_compare__);
//    for (size_t i = 0; i < test_list.size; ++i) {
//        printf("%s\n", test_list.values[i]);
//    }
    test(__BDD_MODE_BEFORE__, &context_stack, NULL, "");
    for (size_t i = 0; i < test_list.size; ++i) {
        test(__BDD_MODE_TEST_RUN__, &context_stack, NULL, test_list.values[i]);
    }
    test(__BDD_MODE_AFTER__, &context_stack, NULL, "");
    return 0;
}

//#include "bdd-for-c.h"
//
//describe("some feature") {
//
//    // These variable definitions must be static because each test
//    // section (it / before / etc...) is a separate function call
//    // and thus will just overwrite these otherwise.
//    static int a;
//    static int b;
//
//    after_each() {
//        b = 3;
//    }
//
//    it("should not work") {
//        a = 2;
//        b = 2;
//        check(a + b == 6, "Adding %i to %i did not equal %i", a, b, 6);
//    }
//
//    it("should work") {
//        check(a + b == 6);
//    }
//
//    before_each() {
//        a = 3;
//    }
//
//    before() {
//        a = 3;
//        b = 3;
//    }
//}
