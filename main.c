#include <stdio.h>
#include <stdlib.h>

#include "bdd.h"


const char* __bdd_describe_name__;
void __bdd_test_main__ (
    int __bdd_is_init_run__,
    int __bdd_index__,
    char** __bdd_test_list__,
    char** __bdd_result__
);

int main (void) {
    const int INIT_RUN = 1;
    const int TEST_RUN = 0;

    char* error;

    // During the first run we just gather the
    // count of the tests and their descriptions
    char** list = malloc(sizeof(char*) * 1000);
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
        }
    }

    return 0;
}

#define describe(name) \
const char* __bdd_describe_name__ = (#name);\
void __bdd_test_main__ (\
    int __bdd_is_init_run__,\
    int __bdd_index__,\
    char** __bdd_test_list__,\
    char** __bdd_result__\
)\



#define it(name) \
if (__bdd_is_init_run__) {\
    __bdd_test_list__[__bdd_index__] = #name;\
    __bdd_test_list__[++__bdd_index__] = 0;\
} else if (--__bdd_index__ == 0)



describe(some feature) {
    it(should work) {

    }
    it(should also work) {

    }
}



