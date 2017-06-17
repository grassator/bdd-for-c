#include "bdd-for-c.h"
#include "test-tree.h"

bool _bdd_same_string(const char* str1, const char* str2) {
    size_t str1length = strlen(str1);
    size_t str2length = strlen(str2);
    if (str1length != str2length) {
        return 0;
    }
    return strncmp(str1, str2, str1length) == 0;
}

spec("test tree") {
    it("should allow to create a node") {
        _bdd_node *n = _bdd_node_create("foo");
        check(strcmp(n->name, "foo") == 0);
        check(n->before != NULL);
        check(n->after != NULL);
        check(n->before_each != NULL);
        check(n->after_each != NULL);
        check(n->children != NULL);
    }

    it("should say that the node is a leaf if it has no children") {
        _bdd_node *root = _bdd_node_create("foo");
        check(_bdd_node_is_leaf(root) == true);
        _bdd_node *child = _bdd_node_create("bar");
        _bdd_array_push(root->children, child);
        check(_bdd_node_is_leaf(root) == false);
    }

    it("should flatten NULL to an empty list") {
        _bdd_array * names = _bdd_array_create();
        _bdd_node_flatten(NULL, names);
        check(names->size == 0);
    }

    it("should add `before` strings before any other when flattening") {
        _bdd_node *root = _bdd_node_create("root");
        _bdd_array_push(root->before, _bdd_node_create("before1"));
        _bdd_array_push(root->children, _bdd_node_create("child"));
        _bdd_array_push(root->before, _bdd_node_create("before2"));

        _bdd_array * names = _bdd_array_create();
        _bdd_node_flatten(root, names);

        check(names->size == 3);
        check(_bdd_same_string((char *) names->values[0], "before1"));
        check(_bdd_same_string((char *) names->values[1], "before2"));
        check(_bdd_same_string((char *) names->values[2], "child"));
    }

    it("should add `before_each` and `after_each` strings before and after each leaf node") {
        _bdd_node *root = _bdd_node_create("root");
        _bdd_array_push(root->after_each, _bdd_node_create("after_each"));
        _bdd_array_push(root->children, _bdd_node_create("child1"));
        _bdd_array_push(root->children, _bdd_node_create("child2"));
        _bdd_array_push(root->before_each, _bdd_node_create("before_each"));

        _bdd_array * names = _bdd_array_create();
        _bdd_node_flatten(root, names);

        check(names->size == 6);
        check(_bdd_same_string((char *) names->values[0], "before_each"));
        check(_bdd_same_string((char *) names->values[1], "child1"));
        check(_bdd_same_string((char *) names->values[2], "after_each"));
        check(_bdd_same_string((char *) names->values[3], "before_each"));
        check(_bdd_same_string((char *) names->values[4], "child2"));
        check(_bdd_same_string((char *) names->values[5], "after_each"));
    }

    it("should support nested nodes") {
        _bdd_node *root = _bdd_node_create("root");
        _bdd_array_push(root->after_each, _bdd_node_create("after_each"));
        _bdd_array_push(root->children, _bdd_node_create("child1"));
        _bdd_array_push(((_bdd_node *) root->children->values[0])->children, _bdd_node_create("child"));
        _bdd_array_push(root->before_each, _bdd_node_create("before_each"));

        _bdd_array * names = _bdd_array_create();
        _bdd_node_flatten(root, names);

        check(names->size == 3);
        check(_bdd_same_string((char *) names->values[0], "before_each"));
        check(_bdd_same_string((char *) names->values[1], "child"));
        check(_bdd_same_string((char *) names->values[2], "after_each"));
    }
}
