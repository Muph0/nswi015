// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Charles University

#include <adt/compare.h>
#include <adt/rbtree.h>
#include <ktest.h>

#include "../test_rbtree.h"

#define insert(tree,VAL) \
    node_t node##VAL; \
    node##VAL.value = VAL; \
    rbnode_init(&node##VAL.tree_link); \
    rbtree_insert(&(tree), &node##VAL.tree_link)

#define remove(tree,VAL) rbtree_remove(&(tree), &node##VAL.tree_link)

void kernel_test(void) {
    ktest_start("adt/rbtree/remove-c1");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    insert(tree, 1);
    insert(tree, 2);
    insert(tree, 3);

    tree.root->left->color = COLOR_BLACK;
    tree.root->right->color = COLOR_BLACK;

    dump_tree(tree.root, NULL);

    remove(tree, 2);

    puts(KTEST_EXPECTED "(B:3 (R:1 B B) B)");
    printk(KTEST_ACTUAL);
    dump_tree(tree.root, NULL);

    ktest_passed();
}
