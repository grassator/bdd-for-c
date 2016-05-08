# bdd-for-c

`bdd-for-c` is a simple [BDD](https://en.wikipedia.org/wiki/Behavior-driven_development) library for the [C language](https://en.wikipedia.org/wiki/C_%28programming_language%29).

## Quick Start

To start simply download the framework header file, which can be easily done with curl on Linux and OS X:
  
```bash
curl -O https://raw.githubusercontent.com/grassator/bdd-for-c/master/bdd-for-c.h
```

Then create a spec file, say named `strcmp_spec.c`, with some tests and the inclusion of the framework header, like the following: 

```c
#include <string.h>
#include "bdd-for-c.h"

describe("strcmp") {

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

Assuming you have a C compiler like [Clang](http://clang.llvm.org) or [GCC](http://clang.llvm.org) setup and ready to go just type in the following commands to compile and run the test:

```bash
cc strcmp_spec.c -o strcmp_spec
./strcmp_spec
```

You should then the test output similar to the following:

```
strcmp
  should return 0 when strings are equal (OK)
  should work for empty strings (OK)
  should return non-0 when strings are not equal (OK)
```

## Project Motivation and Development Philosophy

In order for the testing to be truly useful it needs to be really easy to setup and use, but also scalable to large projects. The tests should be very readable, but, ideally, have the same look and feel as the host language. The test output of the framework should be easy to read for humans and machines. Finally, the framework should only rely on ANSI-C99 features. 
 
Unfortunately all of the existing frameworks, inspected before starting this project, lacked one more of the features mentioned above, with the most common problem being BASIC-style `BEGIN` / `END` delimiters for test, like in [CSpec framework](https://github.com/arnaudbrejeon/cspec/wiki). The issue with `BEGIN` / `END` delimiters is not just that it "doesn't look like C", but also that it makes a different typing flow and screws up IDE auto-completion support.

The framework is currently exclusively targeting the model of "one describe"—"one executable", as this is the model providing fastest compile times (given a correctly structured project), and making mocking much easier.


## Handling State and Fixtures

If you need to setup some state once before all the tests, or before each of the tests, `bdd-for-c` supports special `before` and `before_each` statements for setting up state, and their counterparts `after` and `after_each`. As a bonus here, you can define as many `before`, `after`, `before_each` and `after_each` statements, and in any order you want, so long as they are on the same level as the `it` statements.

>The only caveat with handling state is that if you want to create some test-local variables, you need to mark them as `static`, otherwise the changes done `before` and `before_each` will not be persisted to the test itself, as each of the tests and these setup / teardown functions are implemented as separate function calls.

Here's how it all fits together:

```c
#include <stdio.h>
#include "bdd-for-c.h"

describe("some feature") {

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

    it("should not be equal") {
        check(3 + 2 != 6, "wrong math!");
    }

    it("should work") {
        check(3 + 3 == 6, "wrong math!");
    }
}
```

## Output Colorization

By default if the terminal correctly reports it's color printing ability and the application is run in the interactive mode (from terminal), then the output is going to be colorized.
 
To disable this mode simply add a define statement before you include the `bdd-for-c.h` file:

```c
#define BDD_USE_COLOR 0
#include "bdd-for-c.h"
```

## Available Statements

The `bdd-for-c` framework uses macros to introduce several new statements to the C language that are similar to the built-in statements, such as `if`.

These macros are implemented to be syntactically identical to built-in statements. Among other things this means that for the statements that expect a body, the body can be either empty (by terminating it with a semicolon), contain one expression, or contain a code block (a list of expressions):
  
```c
#include "bdd-for-c.h"

describe('statements') {
    it("should not do anything");
    it("should be in short form") check(1 + 1 = 2);
    it("should be have a code block") {
        check(1 + 1 = 2);
    }
}
```

Also same as with the built-in statements, you have to maintain a certain structure, that will be mentioned where appropriate in the subsections below.

### describe

`describe` must be a top-level statement and there must be exactly one `describe` statement in the test executable. Trying to add more than one will result in a compilation error.

`describe("some functionality")` is used to group a set of expectations and setup / teardown code together and give it a name (in this case "some functionality"), that will be used for test reporting.

### it

`it` statements must be included directly inside of the `describe` statement. Each `it` statements expects a string argument, typically starting with "should", that is used as a human readable explanation for the test and is used in reporting: `it("should behave in some manner")`.

The `it` statement is basic structural block of the spec and is used to need a particular expectation, that is validated using `check` statements described below.

### check

`check` statements are used to check "truthfulness" of a given expression. In case of failure, they terminate the current spec block and add an error for reporting. These statements must be placed inside of `it` statements, but can be indirect children, as shown below:

```c
#include "bdd-for-c.h"

describe("natural number") {
    it("should be non-negative") {
        for (int i = 0; i < 10; ++i) {
            check(i > 0);
        }
    }
}
```

By default `check` statement uses provided expression for error reporting, so if you run the code above you will see the following line:

```
Check failed: i > 0
```

Depending on your variable naming, as in this case, this can be quite useless for figuring out the cause of the failure. To remedy this problem, you can provide a formatter and additional parameters in the same manner as with `printf`:

```
check(i > 0, "A natural number %i must greater than 0", i);
```

This will give you are much more readable output:

```
Check failed: A natural number 0 must greater than 0
```

> Due to the current internal implementation, the number of parameters to `check` is limited to 10. 

While `check` statements are mostly useful inside of `it` statements, you can also use them in the setup / teardown statements (`before`, `after`, `before_each`, `after_each`) to validate some pre- or post-conditions.


### before

`before()` statements, if needed, must be included directly inside of the `describe` statement. This statement is run once before any of the `it` statements and can useful to setup some state. There can be as many `before()` statements as necessary, however you will rarely need more than one.


### after

`after()` statements, if needed, must be included directly inside of the `describe` statement. This statement is run once after any of the `it` statements and can useful to teardown some state. There can be as many `after()` statements as necessary, however you will rarely need more than one.


### before_each

`before_each()` statements, if needed, must be included directly inside of the `describe` statement. This statement is run before each of the `it` statements and can useful to setup some state. There can be as many `before_each()` statements as necessary, however you will rarely need more than one.


### after_each

`after_each()` statements, if needed, must be included directly inside of the `describe` statement. This statement is run before each of the `it` statements and can useful to setup some state. There can be as many `after_each()` statements as necessary, however you will rarely need more than one.


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




