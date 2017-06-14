#include "bdd-for-c.h"
#include "stdbool.h"

typedef struct _bdd_node {
    bool         is_leaf;
    const char*  name;
    char**       before;
    char**       after;
    char**       before_each;
    char**       after_each;
    struct _bdd_node** children;
} _bdd_node;

describe("test tree") {
    it("should allow to create a node") {
        _bdd_node *n = _bdd_node_create("foo", false);
        check(n->before != NULL);
        check(n->after != NULL);
        check(n->after_each != NULL);
        check(n->after_each != NULL);
    }
}
