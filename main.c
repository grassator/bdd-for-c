#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "bdd.h"

#define __BDD_TRUE__ 1
#define __BDD_FALSE__ 0

enum __bdd_run_type__ {
    __BDD_INIT_RUN__,
    __BDD_TEST_RUN__,
    __BDD_BEFORE_EACH_RUN__,
    __BDD_AFTER_EACH_RUN__
} ;

const int __bdd_max_it_count__ = 1000;

typedef struct __bdd_config_type__ {
    enum __bdd_run_type__ run;
    int test_index;
    char** test_list;
    char* error;
} __bdd_config_type__;

const char* __bdd_describe_name__;
void __bdd_test_main__ (__bdd_config_type__* __bdd_config__);

void __bdd_run__(__bdd_config_type__* config, char* name) {
    __bdd_test_main__(config);

    if (config->error == NULL) {
        if (config->run == __BDD_TEST_RUN__) {
            printf("    %s (OK)\n", name);
        }
    } else {
        printf("    %s (FAIL)\n", name);
        printf("        %s\n", config->error);
        free(config->error);
        config->error = NULL;
    }
}

int main (void) {
    struct __bdd_config_type__ config = {
        .run = __BDD_INIT_RUN__,
        .test_index = 0,
        .test_list = NULL,
        .error = NULL
    };

    // During the first run we just gather the
    // count of the tests and their descriptions
    config.test_list = malloc(sizeof(char*) * __bdd_max_it_count__);
    __bdd_test_main__(&config);

    // Outputting the name of the suit
    printf("%s\n", __bdd_describe_name__);

    for (int i = 0; config.test_list[i]; ++i) {
        config.run = __BDD_BEFORE_EACH_RUN__;
        __bdd_run__(&config, "before each");

        config.run = __BDD_TEST_RUN__;
        config.test_index = i;
        __bdd_run__(&config, config.test_list[i]);

        config.run = __BDD_AFTER_EACH_RUN__;
        __bdd_run__(&config, "before each");
    }

    return 0;
}

#define describe(name) \
const char* __bdd_describe_name__ = (name);\
void __bdd_test_main__ (__bdd_config_type__* __bdd_config__)\


#define it(name) \
if (__bdd_config__->run == __BDD_INIT_RUN__) {\
    __bdd_config__->test_list[__bdd_config__->test_index] = name;\
    __bdd_config__->test_list[++__bdd_config__->test_index] = 0;\
} else if (__bdd_config__->run == __BDD_TEST_RUN__ && __bdd_config__->test_index-- == 0)


#define after_each() \
if (__bdd_config__->run == __BDD_AFTER_EACH_RUN__)

#define before_each() \
if (__bdd_config__->run == __BDD_BEFORE_EACH_RUN__)

#define __bdd_ensure_message__(condition, message) if (!(condition))\
{\
    __bdd_config__->error = malloc(2048);\
    if (strlen(message) > 2000) { message[2000] = '\0'; }\
    sprintf(__bdd_config__->error, "Assertion failed: %s", message);\
    return;\
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

    static int a;
    static int b = 3;

    after_each() {
        b = 3;
    }

    it("should not work") {
        a = 2;
        b = 2;
        ensure(a + b == 6, "math shouldn't matter");
    }

    it("should work") {
        ensure(a + b == 6, "math shouldn't matter");
    }

    before_each() {
        a = 3;
    }
}



