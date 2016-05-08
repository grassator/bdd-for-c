#include <stdio.h>
#include "bdd-for-c.h"

describe("some feature") {

    // These variable definitions must be static because each test
    // section (it / before / etc...) is a separate function call
    // and thus will just overwrite these otherwise.
    static int a;
    static int b;

    after() {
        printf("All done!\n");
    }

    after_each() {
        b = 3;
    }

    it("should not work") {
        a = 2;
        b = 2;
        check(a + b == 6, "Adding %i to %i did not equal %i", a, b, 6);
    }

    it("should work") {
        check(a + b == 6);
    }

    before_each() {
        a = 3;
    }

    before() {
        a = 3;
        b = 3;
    }
}
