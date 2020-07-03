#include <string.h>
#include "bdd-for-c.h"

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof((a)[0]))

int plus(int a, int b) { return a + b; }
int times(int a, int b) { return a * b; }

spec("dynamic tests") {
    describe("character spans") {
        // Sometimes it's natural to write a spec with a list of examples.
        // Starting with a table of examples...
        static const struct {
            const char *input;
            const char *accept;
            size_t expected;
        } strspn_cases[] = {
            { "food", "abc", 0 },
            { "back", "abc", 3 },
            { "abacus", "abc", 4 }
        };

        describe("strspn") {
            for (size_t i = 0; i < ARRAY_LENGTH(strspn_cases); i++) {
                const char *input = strspn_cases[i].input;
                const char *accept = strspn_cases[i].accept;
                size_t expected = strspn_cases[i].expected;

                // ...we can iterate over them inside a loop and define a test
                // for each. The test description can be a format string which
                // we use to make it clear which test cases fail.
                it("strspn(\"%s\", \"%s\") == %zd", input, accept, expected) {
                    size_t span = strspn(input, accept);
                    check(span == expected, "got %zd", span);
                }

                // Be careful when declaring tests inside loops. The loop
                // should be as simple as possible and always execute the same
                // way.
            }
        }
    }

    describe("functions that copy data") {
        static const struct {
            const char *name;
            void *(*fn)(void *, void const *, size_t);
        } copy_functions[] = {
            { "memcpy", memcpy },
            { "memmove", memmove }
        };

        // Similarly, describe(name) also accepts a format string. This can be
        // used when checking that several functions have a similar behavior.
        for (size_t i = 0; i < ARRAY_LENGTH(copy_functions); i++)
        describe("the %s function", copy_functions[i].name) {
            it("should copy a string to a buffer") {
                char data[] = "hello, world";
                char buffer[sizeof data];

                copy_functions[i].fn(buffer, data, sizeof data);
                check(memcmp(buffer, data, sizeof data) == 0);
            }
        }
    }

    describe("arithmetic") {
        // In a more complex example we can combine the two
        static const struct expression {
            int a, b, c;
        } plus_examples[] = {
            { 0, 1, 1 },
            { 1, 0, 1 },
            { 1, 1, 2 },
        }, times_examples[] = {
            { 0, 1, 0 },
            { 1, 0, 0 },
            { 1, 1, 1 },
            { 2, 2, 4 },
        };

        static const struct {
            const char *symbol;
            int (*operator)(int, int);
            size_t count;
            const struct expression *examples;
        } arithmetic_tests[] = {
            { "+", plus, ARRAY_LENGTH(plus_examples), plus_examples },
            { "*", times, ARRAY_LENGTH(times_examples), times_examples },
        };

        // It's also possible to combine the two arbitrarily
        for (size_t i = 0; i < ARRAY_LENGTH(arithmetic_tests); i++)
        describe("operator %s", arithmetic_tests[i].symbol) {
            int (*operator)(int, int) = arithmetic_tests[i].operator;

            for (size_t j = 0; j < arithmetic_tests[i].count; j++) {
                const struct expression *e = &arithmetic_tests[i].examples[j];
                int a = e->a, b = e->b, c = e->c;

                it("%d %s %d == %d", a, arithmetic_tests[i].symbol, b, c) {
                    int result = operator(a, b);
                    check(result == c, "got %d", result);
                }
            }

            it("should be associative") {
                int left = operator(operator(1, 2), 3);
                int right = operator(1, operator(2, 3));
                check(left == right);
            }

            it("should be commutative") {
                check(operator(3, 7) == operator(7, 3));
            }
        }
    }
}
