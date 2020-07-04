#include <stdlib.h>
#include "bdd-for-c.h"

spec("before and after hooks") {
    describe("call order") {
        static struct {
            int before_at, before_each_at, after_at, after_each_at;
        } outer1, outer2, inner1, inner2;
        static int it_at, call_order = 1;

        describe("outer") {
            before()            outer1.before_at = call_order++;
            after()             outer1.after_at = call_order++;
            before_each()       outer1.before_each_at = call_order++;
            after_each()        outer1.after_each_at = call_order++;

            describe("inner") {
                before()        inner1.before_at = call_order++;
                after()         inner1.after_at = call_order++;
                before_each()   inner1.before_each_at = call_order++;
                after_each()    inner1.after_each_at = call_order++;

                it("should be run") it_at = call_order++;

                before()        inner2.before_at = call_order++;
                after()         inner2.after_at = call_order++;
                before_each()   inner2.before_each_at = call_order++;
                after_each()    inner2.after_each_at = call_order++;
            }

            before()            outer2.before_at = call_order++;
            after()             outer2.after_at = call_order++;
            before_each()       outer2.before_each_at = call_order++;
            after_each()        outer2.after_each_at = call_order++;
        }

        // Note: these tests depends on the one test above having been run. 
        it("should execute 17 steps")
            check(call_order - 1 == 17, "got: %d", call_order - 1);

        it("1. outer before 1")
            check(1 == outer1.before_at, "got: %d", outer1.before_at);

        it("2. outer before 2")
            check(2 == outer2.before_at, "got: %d", outer2.before_at);

        it("3. inner before 1")
            check(3 == inner1.before_at, "got: %d", inner1.before_at);

        it("4. inner before 2")
            check(4 == inner2.before_at, "got: %d", inner2.before_at);

        it("5. outer before each 1")
            check(5 == outer1.before_each_at, "got: %d", outer1.before_each_at);

        it("6. outer before each 2")
            check(6 == outer2.before_each_at, "got: %d", outer2.before_each_at);

        it("7. inner before each 1")
            check(7 == inner1.before_each_at, "got: %d", inner1.before_each_at);

        it("8. inner before each 2")
            check(8 == inner2.before_each_at, "got: %d", inner2.before_each_at);

        it("9. it")
            check(9 == it_at, "got: %d", it_at);

        it("10. inner after each 1")
            check(10 == inner1.after_each_at, "got: %d", inner1.after_each_at);

        it("11. inner after each 2")
            check(11 == inner2.after_each_at, "got: %d", inner2.after_each_at);

        it("12. outer after each 1")
            check(12 == outer1.after_each_at, "got: %d", outer1.after_each_at);

        it("13. outer after each 2")
            check(13 == outer2.after_each_at, "got: %d", outer2.after_each_at);

        it("14. inner after 1")
            check(14 == inner1.after_at, "got: %d", inner1.after_at);

        it("15. inner after 2")
            check(15 == inner2.after_at, "got: %d", inner2.after_at);

        it("16. outer after 1")
            check(16 == outer1.after_at, "got: %d", outer1.after_at);

        it("17. outer after 2")
            check(17 == outer2.after_at, "got: %d", outer2.after_at);
    }
}
