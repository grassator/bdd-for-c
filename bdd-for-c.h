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

#include <stdarg.h>
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

typedef struct __bdd_config_type__ {
    enum __bdd_run_type__ run;
    unsigned int test_index;
    unsigned int failed_test_count;
    size_t test_list_size;
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

char* __bdd_format__(const char* format, ...) {
    va_list va;
    va_start(va, format);

    // First we over-allocate
    const size_t size = 2048;
    char* result = malloc(sizeof(char) * size);
    vsnprintf(result, size - 1, format, va);

    // Then clip to an actual size
    result = realloc(result, strlen(result) + 1);

    va_end(va);
    return result;
}

int main (void) {
    struct __bdd_config_type__ config = {
        .run = __BDD_INIT_RUN__,
        .test_index = 0,
        .failed_test_count = 0,
        .test_list_size = 8,
        .test_list = NULL,
        .error = NULL
    };

    // During the first run we just gather the
    // count of the tests and their descriptions
    config.test_list = malloc(sizeof(char*) * config.test_list_size);
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
        __bdd_run__(&config, "after each");
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
    while (__bdd_config__->test_index >= __bdd_config__->test_list_size) {\
        __bdd_config__->test_list_size *= 2;\
        __bdd_config__->test_list = realloc(\
            __bdd_config__->test_list,\
            sizeof(char*) * __bdd_config__->test_list_size\
        );\
    }\
    __bdd_config__->test_list[__bdd_config__->test_index] = name;\
    __bdd_config__->test_list[++__bdd_config__->test_index] = 0;\
} else if (__bdd_config__->run == __BDD_TEST_RUN__ && __bdd_config__->test_index-- == 0)


// Unfortunately some IDEs can't properly resolve enum names after macro substitution
// for code completion so in the next 4 macros we are using raw values
#define before_each() if (__bdd_config__->run == 3)
#define after_each() if (__bdd_config__->run == 4)
#define before() if (__bdd_config__->run == 5)
#define after() if (__bdd_config__->run == 6)


#define __BDD_MACRO__(M, ...) __BDD_OVERLOAD__(M, __BDD_COUNT_ARGS__(__VA_ARGS__)) (__VA_ARGS__)
#define __BDD_OVERLOAD__(macro_name, suffix) __BDD_EXPAND_OVERLOAD__(macro_name, suffix)
#define __BDD_EXPAND_OVERLOAD__(macro_name, suffix) macro_name##suffix

#define __BDD_COUNT_ARGS__(...) __BDD_PATTERN_MATCH__(__VA_ARGS__,_,_,_,_,_,_,_,_,_,ONE__)
#define __BDD_PATTERN_MATCH__(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N, ...) N

#define __BDD_CHECK__(condition, ...) if (!(condition))\
{\
    const char* message = __bdd_format__(__VA_ARGS__);\
    const char* fmt = "Check failed: %s";\
    __bdd_config__->error = malloc(sizeof(char) * (strlen(fmt) + strlen(message) + 1));\
    sprintf(__bdd_config__->error, fmt, message);\
    return;\
}
#define __BDD_CHECK_ONE__(condition) __BDD_CHECK__(condition, #condition)

#define check(...) __BDD_MACRO__(__BDD_CHECK_, __VA_ARGS__)


#endif //BDD_FOR_C_H
