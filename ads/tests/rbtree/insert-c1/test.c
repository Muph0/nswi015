// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Charles University

#include "../test_rbtree.h"
#include "test.h"
#include "rbtree.h"

void test_main(void) {
    test_start("rbtree/insert-c1");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    node_t node;
    node.value = 12;
    rbnode_init(&node.tree_link);

    rbtree_insert(&tree, &node.tree_link);

    test_expect(
        dump_tree(tree.root, NULL),
        "(B:12 B B)"
    );

    test_passed();
}
