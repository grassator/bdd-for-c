#include <stdio.h>
#include <stdlib.h>

#include "bdd.h"

const int __bdd_max_it_count__ = 1000;

const char* __bdd_describe_name__;
void __bdd_test_main__ (
    int __bdd_is_init_run__,
    int __bdd_index__,
    char** __bdd_test_list__,
    char** __bdd_error__
);

int main (void) {
    const int INIT_RUN = 1;
    const int TEST_RUN = 0;

    char* error;

    // During the first run we just gather the
    // count of the tests and their descriptions
    char** list = malloc(sizeof(char*) * __bdd_max_it_count__);
    __bdd_test_main__(INIT_RUN, 0, list, &error);

    // Outputting the name of the suit
    printf("%s\n", __bdd_describe_name__);

    for (int i = 0; list[i]; ++i) {
        error = NULL;
        __bdd_test_main__(TEST_RUN, i, list, &error);
        if (error == NULL) {
            printf("    %s (OK)\n", list[i]);
        } else {
            printf("    %s (FAIL)\n", list[i]);
            printf("        %s\n", error);
            free(error);
        }
    }

    return 0;
}

#define describe(name) \
const char* __bdd_describe_name__ = (name);\
void __bdd_test_main__ (\
    int __bdd_is_init_run__,\
    int __bdd_index__,\
    char** __bdd_test_list__,\
    char** __bdd_error__\
)\



#define it(name) \
if (__bdd_is_init_run__) {\
    __bdd_test_list__[__bdd_index__] = name;\
    __bdd_test_list__[++__bdd_index__] = 0;\
} else if (__bdd_index__-- == 0)

#define __bdd_ensure_message__(condition, message) if (!(condition))\
{\
    *__bdd_error__ = malloc(2048);\
    sprintf(*__bdd_error__, "Assertion failed: %s", message);\
}
#define __bdd_ensure_simple__(condition) __bdd_ensure_message__(condition, #condition)

// The interim macro that simply strips the excess and ends up with the required macro
#define __bdd_macro_chooser_1_2__(_, _1, _2, CHOSEN_MACRO, ...) CHOSEN_MACRO

#define ensure(...)\
__bdd_macro_chooser_1_2__(,\
    ##__VA_ARGS__,\
    __bdd_ensure_message__(__VA_ARGS__),\
    __bdd_ensure_simple__(__VA_ARGS__),\
)



describe("some feature") {
    it("should work") {
        int a = 2;
        int b = 3;
        ensure(a + b == 6, "math shouldn't matter");
    }
    it("should also work") {

    }
}



