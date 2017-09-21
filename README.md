# bdd-for-c

`bdd-for-c` is a [BDD](https://en.wikipedia.org/wiki/Behavior-driven_development) library for the [C language](https://en.wikipedia.org/wiki/C_%28programming_language%29).

## Quick Start

To start, simply download the framework header file which can be easily done with curl on Linux and OS X:
  
```bash
curl -O https://raw.githubusercontent.com/grassator/bdd-for-c/master/bdd-for-c.h
```

Then create a spec file, say named `strcmp_spec.c`, with some tests and the inclusion of the framework header, like the following: 

```c
#include <string.h>
#include "bdd-for-c.h"

spec("strcmp") {

    static const char* test_string = "foo";

    it("should return 0 when strings are equal") {
        check(strcmp(test_string, test_string) == 0);
        check(strcmp(test_string, "foo") == 0);
    }

    it("should work for empty strings") {
        check(strcmp("", "") == 0);
    }

    it("should return non-0 when strings are not equal") {
        check(strcmp("foo", "bar") != 0);
        check(strcmp("foo", "foobar") != 0);
    }

}
```

Assuming you have a C compiler like [Clang](http://clang.llvm.org) or [GCC](http://clang.llvm.org) set up and ready to go, just type in the following commands to compile and run the test:

```bash
cc strcmp_spec.c -o strcmp_spec
./strcmp_spec
```

You should then see a test output similar to the following:

```
strcmp
  should return 0 when strings are equal (OK)
  should work for empty strings (OK)
  should return non-0 when strings are not equal (OK)
```

## Project Motivation and Development Philosophy

In order for the testing to be truly useful, it needs to be really easy to setup and use, but also scalable for large projects. The tests should be very readable, but, ideally, have the same look-and-feel as the host language. The test output of the framework should be easy to read for humans and machines. Finally, if it's C, then the framework should only rely on ANSI-C99 features.
 
Unfortunately all of the existing frameworks, inspected before starting this project, lack one or more of the features mentioned above, with the most common problem being BASIC-style `BEGIN` / `END` delimiters for a test, like in [CSpec framework](https://github.com/arnaudbrejeon/cspec/wiki). The issue with `BEGIN` / `END` delimiters is not just that it "doesn't look like C", but also that it makes a different typing flow and screws up IDE auto-completion support.

The framework is currently exclusively targeting the model of "one spec"—"one executable", as this is the model providing the fastest compile times (given a correctly structured project). This also makes mocking much easier.


## Handling State and Fixtures

If you need to setup some state once before all the tests, or before each of the tests, `bdd-for-c` supports special `before` and `before_each` statements, and their counterparts `after` and `after_each`. As a bonus, you can define as many `before`, `after`, `before_each` and `after_each` statements in any order as you want, as long as they are on the same level as the corresponding `it` statement.

>The only caveat with handling state is that if you want to create some test-local variables, you need to mark them as `static`, otherwise the changes done `before` and `before_each` will not be persisted to the test itself, as each of the tests and these setup / teardown functions are implemented as separate function calls.

Here's how it all fits together:

```c
#include <stdio.h>
#include "bdd-for-c.h"

spec("some feature") {

    static int test_count;
    static int tests_failed;

    after() {
        printf("  All done!\n");
    }

    after() {
        printf("%i tests run. %i failed.\n", test_count, tests_failed);
    }

    before() {
        test_count = 0;
        tests_failed = 0;
    }

    before() {
        printf("  Starting the tests...\n\n");
    }

    before_each() {
        ++test_count;
        ++tests_failed;
    }

    after_each() {
        --tests_failed;
    }

    describe("broken math") {
        it("should not be equal") {
            check(3 + 2 != 6, "this is indeed broken!");
        }
    }

    context("when everything is right") {
        it("should work") {
            check(3 + 3 == 6);
        }
    }
}
```

## Output Colorization

By default, if the terminal correctly reports its color printing ability and the application is run in the interactive mode (from terminal), then the output is going to be colorized.
 
To disable this mode, simply add a define statement before you include the `bdd-for-c.h` file:

```c
#define BDD_USE_COLOR 0
#include "bdd-for-c.h"
```

> Note to [CLion](https://www.jetbrains.com/clion/) users. In order to get colored output when running tests from the IDE, you need to add `TERM=xterm-256color` to the `Environment Variables` field of your build configuration. 

## Support for [TAP](https://testanything.org)

`bdd-for-c` supports the [test anything protocol](https://testanything.org) which has an output that is easily readable by programs. This allows an easier integration with CI systems as well as the aggregation of the output of multiple executables that are using `bdd-for-c`.

To switch to TAP output mode you can either add a `define` statement before you include the framework:

```c
#define BDD_USE_TAP 1
#include "bdd-for-c.h"
```

...or just add an environment variable when you run a test:

```bash
BDD_USE_TAP=1 ./strcmp_spec
```

## Available Statements

The `bdd-for-c` framework uses macros to introduce several new statements to the C language that are similar to the built-in statements, such as `if`.

These macros are implemented to be syntactically identical to the built-in statements. Among other things, this means that for the statements that expect a body, the body can be either empty (by terminating it with a semicolon), contain one statement, or contain a code block (a list of statements):
  
```c
#include "bdd-for-c.h"

spec('statements') {
    it("should not do anything");
    it("should be in short form") check(1 + 1 = 2);
    it("should be have a code block") {
        check(1 + 1 = 2);
    }
}
```

Also, same as with the built-in statements, you have to maintain a certain structure that will be mentioned where appropriate in the subsections below.

### spec

`spec` must be a top-level statement and there must be exactly one `spec` statement in the test executable.  Trying to add more than one, will result in a compilation error.

`spec("some functionality")` is used to group a set of expectations and setup/teardown code together and gives this unit a name (in this case "some functionality"), that will be used for test reporting.

### it

`it` statements must be included directly inside of the `spec`, `describe` or `context` statement. Each `it` statements expects a string argument, typically starting with "should", that is used as a human readable explanation for the test and is used in reporting: `it("should behave in some manner")`.

The `it` statement is a basic structural block of the spec and is used to ensure a particular expectation that is validated using `check` statements described below.

### describe

`describe` statements must be included directly inside of the `spec`, `context`, or another `describe` statement.

The `describe` is used to group certain `it` statements together, usually based on the fact that they belong to the same functionality or happen in the same state.

### context

A `context` statement is completely equivalent to `describe` in its behavior and should be used when it better conveys the topic of a group of tests.

> NOTE: since `context` is a quite common global variable in C applications, it is possible to not expose it by setting a flag:
 `#define BDD_NO_CONTEXT_KEYWORD 1` before do you `#include "bdd-for-c.h"`

### check

`check` statements are used to check "truthfulness" of a given expression. In case of failures, they terminate the current spec block and report an error. These statements must be placed inside of `it` statements, either as direct or indirect children:

```c
#include "bdd-for-c.h"

spec("natural number") {
    it("should be non-negative") {
        for (int i = 0; i < 10; ++i) {
            check(i > 0);
        }
    }
}
```

By default, a `check` statement uses the provided expression for error reporting, so, if you run the code above, you will see the following line:

```
Check failed: i > 0
```

Depending on your variable naming, as in this case, this can be quite useless for figuring out the cause of the failure. To remedy this problem, you can provide a formatter and additional parameters in the same manner as with `printf`:

```
check(i > 0, "a natural number %i must greater than 0", i);
```

This will give you a much more readable output:

```
Check failed: a natural number 0 must greater than 0
```

> Due to limitations in the current implementation, the number of parameters to `check` is limited to 10. 

While `check` statements are mostly useful inside of `it` statements, you can also use them in the setup / teardown statements (`before`, `after`, `before_each`, `after_each`) to validate some pre- or post-conditions.


### before

`before()` statements, if needed, can be included directly inside of the `spec`, `describe` or `context` statements. It's run once before all of the `it` statements in the group/spec and can be useful to setup some state. There can be as many `before()` statements as necessary.


### after

`after()` statements, if needed, can be included directly inside of the `spec`, `describe` or `context` statements. It's run once after all of the `it` statements in the group/spec and can be useful to teardown some state. There can be as many `after()` statements as necessary.


### before_each

`before_each()` statements, if needed, can be included directly inside of the `spec`, `describe` or `context` statements. It's run before each `it` statement and can be useful to setup some state. There can be as many `before_each()` statements as necessary.


### after_each

`after_each()` statements, if needed, can be included directly inside of the `spec`, `describe` or `context` statements. It's run after each `it` statement and can be useful to setup some state. There can be as many `after_each()` statements as necessary.


## License

The MIT License (MIT)

Copyright (c) 2016 Dmitriy Kubyshkin

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




