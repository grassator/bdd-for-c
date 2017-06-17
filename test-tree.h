#ifndef BDD_TEST_TREE_H
#define BDD_TEST_TREE_H

#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include "array.h"

typedef struct _bdd_node {
    char*  name;
    _bdd_array*  before;
    _bdd_array*  after;
    _bdd_array*  before_each;
    _bdd_array*  after_each;
    _bdd_array*  children;
} _bdd_node;

_bdd_node* _bdd_node_create(char *name) {
    _bdd_node* n = malloc(sizeof(_bdd_node));
    n->name = name;
    n->before = _bdd_array_create();
    n->after = _bdd_array_create();
    n->before_each = _bdd_array_create();
    n->after_each = _bdd_array_create();
    n->children = _bdd_array_create();
    return n;
}

bool _bdd_node_is_leaf(_bdd_node * node) {
    return node->children->size == 0;
}

void _bdd_node_flatten_internal(
    _bdd_node * node, _bdd_array * names, _bdd_array * before_each_lists, _bdd_array * after_each_lists
) {
    if (_bdd_node_is_leaf(node)) {

        for (size_t listIndex = 0; listIndex < before_each_lists->size; ++listIndex) {
            _bdd_array *list = before_each_lists->values[listIndex];
            for (size_t i = 0; i < list->size; ++i) {
                _bdd_array_push(names, ((_bdd_node *) list->values[i])->name);
            }
        }

        _bdd_array_push(names, node->name);

        for (size_t listIndex = 0; listIndex < after_each_lists->size; ++listIndex) {
            _bdd_array *list = after_each_lists->values[listIndex];
            for (size_t i = 0; i < list->size; ++i) {
                _bdd_array_push(names, ((_bdd_node *) list->values[i])->name);
            }
        }
        return;
    }

    for (size_t i = 0; i < node->before->size; ++i) {
        _bdd_array_push(names, ((_bdd_node *) node->before->values[i])->name);
    }

    _bdd_array_push(before_each_lists, node->before_each);
    _bdd_array_push(after_each_lists, node->after_each);

    for (size_t i = 0; i < node->children->size; ++i) {
        _bdd_node_flatten_internal(node->children->values[i], names, before_each_lists, after_each_lists);
    }

    _bdd_array_pop(before_each_lists);
    _bdd_array_pop(after_each_lists);

    for (size_t i = 0; i < node->after->size; ++i) {
        _bdd_array_push(names, ((_bdd_node *) node->after->values[i])->name);
    }
}

_bdd_array * _bdd_node_flatten(_bdd_node * node, _bdd_array * names) {
    if (node == NULL) {
        return names;
    }

    _bdd_node_flatten_internal(node, names, _bdd_array_create(), _bdd_array_create());

    return names;
}

#endif //BDD_TEST_TREE_H
