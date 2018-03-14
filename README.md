# bdd-for-c

The `bdd-for-c` library is a [BDD][bdd] test framework for the [C language][c].

## Quick Start

To start, simply download the framework header file which can be easily done
with curl on Linux and OS X:

```bash
curl -O https://raw.githubusercontent.com/grassator/bdd-for-c/master/bdd-for-c.h
```

Next, create a spec file, named something appropriate like `strncmp_spec.c` if
testing the `strncmp()` funtion.  Add some tests and include the framework
header, like the following:

```c
#include <string.h>
#include "bdd-for-c.h"

spec("strncmp") {
    static const char *test_string = "foo";

    it("should return 0 when strings are equal") {
        check(strncmp(test_string, test_string, 12) == 0);
        check(strncmp(test_string, "foo", 12) == 0);
    }

    it("should work for empty strings") {
        check(strncmp("", "", 12) == 0);
    }

    it("should return non-0 when strings are not equal") {
        check(strncmp("foo", "bar", 12) != 0);
        check(strncmp("foo", "foobar", 12) != 0);
    }

    it("should return 0 when strings match up to specified length") {
        check(strncmp("foobar", "foobaz", 3) == 0);
    }
}
```

Assuming you have a C compiler like [Clang][clang] or [GCC][gcc] set up and
ready to go, just type in the following commands to compile and run the test:

```bash
cc strncmp_spec.c -o strncmp_spec
./strncmp_spec
```

You should then see test output similar to the following:

```
strncmp
  should return 0 when strings are equal (OK)
  should work for empty strings (OK)
  should return non-0 when strings are not equal (OK)
  should return 0 when strings match up to specified length (OK)
```

## Project Motivation and Development Philosophy

In order for testing to be truly useful, it needs to be really easy to setup
and use, but also scalable for large projects.  The tests should be very
readable but, ideally, have the same look-and-feel as the host language.  The
framework's test output should be easy to read for both humans and machines.
Finally, if it's C, then the framework should only rely on ANSI-C99 features.

Unfortunately, all of the existing frameworks inspected before starting this
project lack one or more of those requirements, with the most common problem
being BASIC-style `BEGIN` / `END` delimiters for a test, like in [CSpec
framework][cspec].  The issue with `BEGIN` / `END` delimiters is not just that
it "doesn't look like C", but also that it imposes a different typing flow and
screws up auto-completion support for IDEs and programming text editors.

The framework is currently exclusively targeting the model of "one spec, one
executable", as this is the model providing the fastest compile times (given a
correctly structured project).  This also makes mocking much easier.


## Handling State and Fixtures

If you need to set up some state once before all the tests, or before each of
the tests, `bdd-for-c` supports special `before` and `before_each` statements,
and their counterparts `after` and `after_each`.  As a bonus, you can define as
many `before`, `after`, `before_each` and `after_each` statements in any order
as you want, as long as they are on the same level as the corresponding `it`
statement.

> The only caveat with handling state is that, if you want to create some
> test-local variables, you need to mark them as `static`.  Otherwise, the
> changes done `before` and `before_each` will not persist to the test itself,
> as each of the tests and these setup / teardown functions are implemented as
> separate function calls.

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

By default, if the terminal correctly reports its color printing ability and
the application is run in the interactive mode (from terminal), then the output
is going to be colorized.

To disable this mode, simply add a define statement before you include the
`bdd-for-c.h` file:

```c
#define BDD_USE_COLOR 0
#include "bdd-for-c.h"
```

> Note to [CLion][clion] users. In order to get colored output when running
> tests from the IDE, you need to add `TERM=xterm-256color` to the `Environment
> Variables` field of your build configuration.

## Support for [TAP][tap]

The `bdd-for-c` library supports the [test anything protocol][tap], or TAP,
which formats output to be easily readable by programs.  This allows easier
integration with continuous integration systems as well as the aggregation of
output from multiple executables that use `bdd-for-c`.

To switch to TAP output mode you can add a `define` statement before including
the framework:

```c
#define BDD_USE_TAP 1
#include "bdd-for-c.h"
```

You may also add an environment variable when you run a test, instead:

```bash
BDD_USE_TAP=1 ./strncmp_spec
```

## Available Statements

The `bdd-for-c` framework uses macros to introduce several new statements to
the C language that are similar to the built-in statements, such as `if`.

These macros are implemented to be syntactically identical to the built-in
statements.  Among other things, this means that for the statements that expect
a body, the body can be empty (by terminating it with a semicolon), contain one
statement, or contain a code block (a list of statements):

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

As with the built-in statements, you have to maintain a certain structure,
described where appropriate in the subsections below.

### spec

The `spec` statement must be a top-level statement and there must be exactly
one `spec` statement in the test executable.  Using more than one will result
in a compilation error.

Use `spec("some functionality")` to group a set of expectations and
setup/teardown code together and give the unit a name (in this case "some
functionality").  This will be used for test reporting.

### it

You must include `it` statements directly inside a `spec`, `describe`, or
`context` statement.  Each `it` statement expects a string argument, typically
starting with "should", used as a human readable explanation for the test, and
used in reporting: `it("should behave in some manner")`.

The `it` statement is a basic structural block of the spec and is used to
ensure a particular expectation, validated using `check` statements (described
below).

### describe

A `describe` statement must be included directly inside `spec` or `context`
statements, or within another `describe` statement.  It is used to group `it`
statements together, usually based on the fact that they belong to the same
unit of program functionality.

### context

A `context` statement is functionally identical to `describe`, and should be
used when it better conveys the topic of a group of tests.  It is usually used
to group `describe` statements based on the fact they depend on the same
program state, providing a way to easily set up the same program state for
several test conditions.

> NOTE: Because `context` is a quite common global variable in C applications,
> it is possible to not expose the `context` implementation in `bdd-for-c` by
> setting a flag before including the `bdd-for-c` library:

```c
#define BDD_NO_CONTEXT_KEYWORD 1
#include "bdd-for-c.h"
```

### check

A `check` statement is used to check "truthfulness" of a given expression.  In
case of failure, it terminates the current spec block and reports an error.
These statements must be placed inside of `it` statements, either as direct or
indirect children:

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



[bdd]: https://en.wikipedia.org/wiki/Behavior-driven_development
[c]: https://en.wikipedia.org/wiki/C_%28programming_language%29
[clang]: http://clang.llvm.org
[gcc]: http://clang.llvm.org
[cspec]: https://github.com/arnaudbrejeon/cspec/wiki
[clion]: https://www.jetbrains.com/clion/
[tap]: https://testanything.org
