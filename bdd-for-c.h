/*!
The MIT License (MIT)

Copyright (c) 2016 Dmitriy Kubyshkin <dmitriy@kubyshkin.name>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BDD_FOR_C_H
#define BDD_FOR_C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum __bdd_run_type__ {
    __BDD_INIT_RUN__ = 1,
    __BDD_TEST_RUN__ = 2,
    __BDD_BEFORE_EACH_RUN__ = 3,
    __BDD_AFTER_EACH_RUN__ = 4,
    __BDD_BEFORE_RUN__ = 5,
    __BDD_AFTER_RUN__ = 6
} ;

const int __bdd_max_it_count__ = 1000;

typedef struct __bdd_config_type__ {
    enum __bdd_run_type__ run;
    unsigned int test_index;
    unsigned int failed_test_count;
    char** test_list;
    char* error;
} __bdd_config_type__;

const char* __bdd_describe_name__;
void __bdd_test_main__ (__bdd_config_type__* __bdd_config__);

void __bdd_run__(__bdd_config_type__* config, char* name) {
    __bdd_test_main__(config);

    if (config->error == NULL) {
        if (config->run == __BDD_TEST_RUN__) {
            printf("  %s (OK)\n", name);
        }
    } else {
        ++config->failed_test_count;
        printf("  %s (FAIL)\n", name);
        printf("    %s\n", config->error);
        free(config->error);
        config->error = NULL;
    }
}

int main (void) {
    struct __bdd_config_type__ config = {
        .run = __BDD_INIT_RUN__,
        .test_index = 0,
        .failed_test_count = 0,
        .test_list = NULL,
        .error = NULL
    };

    // During the first run we just gather the
    // count of the tests and their descriptions
    config.test_list = malloc(sizeof(char*) * __bdd_max_it_count__);
    __bdd_test_main__(&config);

    const unsigned int test_count = config.test_index;

    // Outputting the name of the suit
    printf("%s\n", __bdd_describe_name__);

    config.run = __BDD_BEFORE_RUN__;
    __bdd_run__(&config, "before");

    for (unsigned int i = 0; config.test_list[i]; ++i) {
        config.run = __BDD_BEFORE_EACH_RUN__;
        __bdd_run__(&config, "before each");

        config.run = __BDD_TEST_RUN__;
        config.test_index = i;
        __bdd_run__(&config, config.test_list[i]);

        config.run = __BDD_AFTER_EACH_RUN__;
        __bdd_run__(&config, "before each");
    }

    config.run = __BDD_AFTER_RUN__;
    __bdd_run__(&config, "after");

    if (config.failed_test_count > 0) {
        printf(
            "\n  %i test%s run, %i failed.\n",
            test_count, test_count == 1 ? "" : "s", config.failed_test_count
        );
        return 1;
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


// Unfortunately some IDEs can't properly resolve enum names after macro substitution
// for code completion so in the next 4 macros we are using raw values
#define before_each() if (__bdd_config__->run == 3)
#define after_each() if (__bdd_config__->run == 4)
#define before() if (__bdd_config__->run == 5)
#define after() if (__bdd_config__->run == 6)


#define __bdd_check_message__(condition, message) if (!(condition))\
{\
    __bdd_config__->error = malloc(2048);\
    if (strlen(message) > 2000) { message[2000] = '\0'; }\
    sprintf(__bdd_config__->error, "Assertion failed: %s", message);\
    return;\
}
#define __bdd_check_simple__(condition) __bdd_check_message__(condition, #condition)

// The interim macro that simply strips the excess and ends up with the required macro
#define __bdd_macro_chooser_1_2__(_, _1, _2, CHOSEN_MACRO, ...) CHOSEN_MACRO

#define check(...)\
__bdd_macro_chooser_1_2__(,\
    ##__VA_ARGS__,\
    __bdd_check_message__(__VA_ARGS__),\
    __bdd_check_simple__(__VA_ARGS__),\
)

#endif //BDD_FOR_C_H
