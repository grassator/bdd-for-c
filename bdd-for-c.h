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
#include <stdbool.h>

#ifndef _WIN32
#include <unistd.h>
#include <term.h>

#define __BDD_IS_ATTY__() isatty(fileno(stdin))
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#define __BDD_IS_ATTY__() _isatty(_fileno(stdin))
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS
#endif

#ifndef BDD_USE_COLOR
#define BDD_USE_COLOR 1
#endif

#ifndef BDD_USE_TAP
#define BDD_USE_TAP 0
#endif

#define __BDD_COLOR_RESET__       "\x1B[0m"
#define __BDD_COLOR_RED__         "\x1B[31m"             /* Red */
#define __BDD_COLOR_GREEN__       "\x1B[32m"             /* Green */
#define __BDD_COLOR_BOLD__        "\x1B[1m"              /* Bold White */

bool __bdd_same_string__(const char* str1, const char* str2) {
    size_t str1length = strlen(str1);
    size_t str2length = strlen(str2);
    if (str1length != str2length) {
        return 0;
    }
    return strncmp(str1, str2, str1length) == 0;
}

typedef struct __bdd_array__ {
    void ** values;
    size_t capacity;
    size_t size;
} __bdd_array__;

__bdd_array__* __bdd_array_create__() {
    __bdd_array__ *arr = malloc(sizeof(__bdd_array__));
    arr->capacity = 4;
    arr->size = 0;
    arr->values = calloc(arr->capacity, sizeof(void *));
    return arr;
}

__bdd_array__* __bdd_array_push__(__bdd_array__ *arr, void *item) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->values = realloc(arr->values, sizeof(void *) * arr->capacity);
    }
    arr->values[arr->size++] = item;
    return arr;
}

void* __bdd_array_last__(__bdd_array__ *arr) {
    if (arr->size == 0) {
        return NULL;
    }
    return arr->values[arr->size - 1];
}

void* __bdd_array_pop__(__bdd_array__ *arr) {
    if (arr->size == 0) {
        return NULL;
    }
    void* result = arr->values[arr->size - 1];
    --arr->size;
    return result;
}

void __bdd_array_free__(__bdd_array__ *arr) {
    free(arr->values);
    free(arr);
}

typedef struct __bdd_node__ {
    char*  name;
    __bdd_array__*  list_before;
    __bdd_array__*  list_after;
    __bdd_array__*  list_before_each;
    __bdd_array__*  list_after_each;
    __bdd_array__*  list_children;
} __bdd_node__;

__bdd_node__ * __bdd_node_create__(char *name) {
    __bdd_node__ * n = malloc(sizeof(__bdd_node__));
    n->name = name;
    n->list_before = __bdd_array_create__();
    n->list_after = __bdd_array_create__();
    n->list_before_each = __bdd_array_create__();
    n->list_after_each = __bdd_array_create__();
    n->list_children = __bdd_array_create__();
    return n;
}

bool __bdd_node_is_leaf__(__bdd_node__ * node) {
    return node->list_children->size == 0;
}

void __bdd_node_flatten_internal__(
    __bdd_node__ * node, __bdd_array__ * names, __bdd_array__ * before_each_lists, __bdd_array__ * after_each_lists
) {
    if (__bdd_node_is_leaf__(node)) {

        for (size_t listIndex = 0; listIndex < before_each_lists->size; ++listIndex) {
            __bdd_array__ *list = before_each_lists->values[listIndex];
            for (size_t i = 0; i < list->size; ++i) {
                __bdd_array_push__(names, ((__bdd_node__ *) list->values[i])->name);
            }
        }

        __bdd_array_push__(names, node->name);

        for (size_t listIndex = 0; listIndex < after_each_lists->size; ++listIndex) {
            __bdd_array__ *list = after_each_lists->values[listIndex];
            for (size_t i = 0; i < list->size; ++i) {
                __bdd_array_push__(names, ((__bdd_node__ *) list->values[i])->name);
            }
        }
        return;
    }

    for (size_t i = 0; i < node->list_before->size; ++i) {
        __bdd_array_push__(names, ((__bdd_node__ *) node->list_before->values[i])->name);
    }

    __bdd_array_push__(before_each_lists, node->list_before_each);
    __bdd_array_push__(after_each_lists, node->list_after_each);

    for (size_t i = 0; i < node->list_children->size; ++i) {
        __bdd_node_flatten_internal__(node->list_children->values[i], names, before_each_lists, after_each_lists);
    }

    __bdd_array_pop__(before_each_lists);
    __bdd_array_pop__(after_each_lists);

    for (size_t i = 0; i < node->list_after->size; ++i) {
        __bdd_array_push__(names, ((__bdd_node__ *) node->list_after->values[i])->name);
    }
}

__bdd_array__ * __bdd_node_flatten__(__bdd_node__ * node, __bdd_array__ * names) {
    if (node == NULL) {
        return names;
    }

    __bdd_node_flatten_internal__(node, names, __bdd_array_create__(), __bdd_array_create__());

    return names;
}

enum __bdd_run_type__ {
    __BDD_INIT_RUN__ = 1,
    __BDD_TEST_RUN__ = 2,
    __BDD_BEFORE_EACH_RUN__ = 3,
    __BDD_AFTER_EACH_RUN__ = 4,
    __BDD_BEFORE_RUN__ = 5,
    __BDD_AFTER_RUN__ = 6
};

typedef struct __bdd_config_type__ {
    enum __bdd_run_type__ run;
    size_t test_index;
    size_t test_tap_index;
    size_t failed_test_count;
    char * current_test;
    __bdd_array__* node_stack;
    char* error;
    bool use_color;
    bool use_tap;
} __bdd_config_type__;

char* __bdd_spec_name__;
void __bdd_test_main__(__bdd_config_type__* __bdd_config__);

bool __bdd_is_internal_step__(char * step) {
    return strlen(step) > 2 && step[0] == '-' && step[1] == '-';
}

void __bdd_run__(__bdd_config_type__* config) {
    char *name = config->current_test;
    __bdd_test_main__(config);

    if (__bdd_is_internal_step__(name)) {
        return;
    }

    ++config->test_tap_index;

    if (config->error == NULL) {
        if (config->run == __BDD_TEST_RUN__) {
            if (config->use_tap) {
                // We only to report tests and not setup / teardown success
                if (config->test_tap_index) {
                    printf("ok %zu - %s\n", config->test_tap_index, name);
                }
            } else {
                printf(
                    "  %s %s(OK)%s\n", name,
                    config->use_color ? __BDD_COLOR_GREEN__ : "",
                    config->use_color ? __BDD_COLOR_RESET__ : ""
                );
            }
        }
    } else {
        ++config->failed_test_count;
        if (config->use_tap) {
            // We only to report tests and not setup / teardown errors
            if (config->test_tap_index) {
                printf("not ok %zu - %s\n", config->test_tap_index, name);
            }
        } else {
            printf(
                "  %s %s(FAIL)%s\n", name,
                config->use_color ? __BDD_COLOR_RED__ : "",
                config->use_color ? __BDD_COLOR_RESET__ : ""
            );
            printf("    %s\n", config->error);
        }
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

bool __bdd_is_supported_term__() {
    bool result;
    const char *term = getenv("TERM");
    result = term && strcmp(term, "") != 0;
#ifndef _WIN32
    return result;
#else
    if (result) {
        return 1;
    }

    // Attempt to enable virtual terminal processing on Windows.
    // See: https://msdn.microsoft.com/en-us/library/windows/desktop/mt638032(v=vs.85).aspx
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return 0;
    }

    dwMode |= 0x4; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    if (!SetConsoleMode(hOut, dwMode)) {
        return 0;
    }

    return 1;
#endif
}

int main(void) {
    struct __bdd_config_type__ config = {
        .run = __BDD_INIT_RUN__,
        .test_index = 0,
        .test_tap_index = 0,
        .failed_test_count = 0,
        .node_stack = __bdd_array_create__(),
        .error = NULL,
        .use_color = 0,
        .use_tap = 0
    };

    const char *tap_env = getenv("BDD_USE_TAP");
    if (BDD_USE_TAP || (tap_env && strcmp(tap_env, "") != 0 && strcmp(tap_env, "0") != 0)) {
        config.use_tap = 1;
    }

    if (!config.use_tap && BDD_USE_COLOR && __BDD_IS_ATTY__() && __bdd_is_supported_term__()) {
        config.use_color = 1;
    }

    __bdd_array_push__(config.node_stack, __bdd_node_create__(__bdd_spec_name__));

    // During the first run we just gather the
    // count of the tests and their descriptions
    __bdd_test_main__(&config);

    __bdd_array__ * names = __bdd_array_create__();
    __bdd_node_flatten__(config.node_stack->values[0], names);

    size_t test_count = 0;
    for (size_t i = 0; i < names->size; ++i) {
        if(!__bdd_is_internal_step__(names->values[i])) {
            ++test_count;
        }
    }

    // Outputting the name of the suite
    if (config.use_tap) {
        printf("TAP version 13\n1..%zu\n", test_count);
    } else {
        printf(
            "%s%s%s\n",
            config.use_color ? __BDD_COLOR_BOLD__ : "",
            __bdd_spec_name__,
            config.use_color ? __BDD_COLOR_RESET__ : ""
        );
    }

    config.run = __BDD_TEST_RUN__;

    for (size_t i = 0; i < names->size; ++i) {
        config.node_stack->size = 0;
        __bdd_array_push__(config.node_stack, __bdd_node_create__(__bdd_spec_name__));
        config.current_test = names->values[i];
        __bdd_run__(&config);
    }

    if (config.failed_test_count > 0) {
        if (!config.use_tap) {
            printf(
                "\n  %zu test%s run, %zu failed.\n",
                test_count, test_count == 1 ? "" : "s", config.failed_test_count
            );
        }
        return 1;
    }

    return 0;
}

#define spec(name) \
char* __bdd_spec_name__ = (name);\
void __bdd_test_main__ (__bdd_config_type__* __bdd_config__)\

#define __BDD_LAST_NODE__ ((__bdd_node__ *) __bdd_array_last__(__bdd_config__->node_stack))

#define __BDD_STEP__(node_list, name)\
for(\
    void * __bdd_index__ = 0,\
         * __bdd_node_name__ = (name);\
    (\
        (\
            __bdd_config__->run == __BDD_INIT_RUN__ &&\
            __bdd_array_push__((node_list), __bdd_node_create__(__bdd_node_name__)) &&\
            false \
        ) || \
        (\
            __bdd_config__->run == __BDD_TEST_RUN__ &&\
            (int) __bdd_index__ < 1 &&\
            __bdd_same_string__(__bdd_node_name__, __bdd_config__->current_test)\
        )\
    );\
    ++__bdd_index__\
)

#define it(name) __BDD_STEP__(\
  __BDD_LAST_NODE__->list_children,\
  name\
)

#define before_each() __BDD_STEP__(\
  __BDD_LAST_NODE__->list_before_each,\
  __bdd_format__("--before-each-%i", __BDD_LAST_NODE__->list_before_each->size)\
)

#define after_each() __BDD_STEP__(\
  __BDD_LAST_NODE__->list_after_each,\
  __bdd_format__("--after-each-%i", __BDD_LAST_NODE__->list_after_each->size)\
)

#define before() __BDD_STEP__(\
  __BDD_LAST_NODE__->list_before,\
  __bdd_format__("--before-%i", __BDD_LAST_NODE__->list_before->size)\
)

#define after() __BDD_STEP__(\
  __BDD_LAST_NODE__->list_after,\
  __bdd_format__("--after-%i", __BDD_LAST_NODE__->list_after->size)\
)


#define __BDD_MACRO__(M, ...) __BDD_OVERLOAD__(M, __BDD_COUNT_ARGS__(__VA_ARGS__)) (__VA_ARGS__)
#define __BDD_OVERLOAD__(macro_name, suffix) __BDD_EXPAND_OVERLOAD__(macro_name, suffix)
#define __BDD_EXPAND_OVERLOAD__(macro_name, suffix) macro_name##suffix

#define __BDD_COUNT_ARGS__(...) __BDD_PATTERN_MATCH__(__VA_ARGS__,_,_,_,_,_,_,_,_,_,ONE__)
#define __BDD_PATTERN_MATCH__(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N, ...) N

void __bdd_sprintf__(char* buffer, const char* fmt, const char* message) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS
#endif
    sprintf(buffer, fmt, message);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#define __BDD_CHECK__(condition, ...) if (!(condition))\
{\
    const char* message = __bdd_format__(__VA_ARGS__);\
    const char* fmt = __bdd_config__->use_color ?\
        (__BDD_COLOR_RED__ "Check failed: %s" __BDD_COLOR_RESET__ ) :\
        "Check failed: %s";\
    __bdd_config__->error = malloc(sizeof(char) * (strlen(fmt) + strlen(message) + 1));\
    __bdd_sprintf__(__bdd_config__->error, fmt, message);\
    return;\
}

#define __BDD_CHECK_ONE__(condition) __BDD_CHECK__(condition, #condition)

#define check(...) __BDD_MACRO__(__BDD_CHECK_, __VA_ARGS__)

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //BDD_FOR_C_H
