// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Charles University

#include <adt/compare.h>
#include <adt/rbtree.h>
#include <ktest.h>
#include <mm/heap.h>

#include "../test_rbtree.h"

static uint32_t lfsr32(uint32_t* state) {
    for (int i = 0; i < 32; i++) {
        int bit = (*state >> 0) ^ (*state >> 10) ^ (*state >> 30) ^ (*state >> 31);
        *state = (*state >> 1) | (bit << 31);
    }

    return *state;
}

void kernel_test(void) {
    ktest_start("adt/rbtree/remove-complex");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    uint32_t seed = 123456;

#define TEST_RANGE 11
#define DEL_COUNT TEST_RANGE

    node_t* nodes = (node_t*)kmalloc(TEST_RANGE * sizeof(node_t));
    for (int i = 0; i < TEST_RANGE; i++) {
        rbnode_init(&nodes[i].tree_link);
        nodes[i].value = i + 1;
    }

    // now permutate the nodes
    for (uint32_t i = 1; i < TEST_RANGE; i++) {
        uint32_t j = lfsr32(&seed) % (i + 1);
        int temp = nodes[i].value;
        nodes[i].value = nodes[j].value;
        nodes[j].value = temp;
    }

    for (int i = 0; i < DEL_COUNT; i++) {
        printk("remove [%d]=%d:\n", i, nodes[i].value);
        rbtree_remove(&tree, &nodes[i].tree_link);

        ktest_assert(rbnode_color(tree.root) == COLOR_BLACK, "Root property violated.");
        ktest_assert(has_red_property(tree.root, NULL), "Red property violated.");
        ktest_assert(has_depth_property(tree.root, 0), "Depth property violated.");
        ktest_assert(has_order_property(tree.root, node_comparison), "Order property violated.");
        ktest_assert(node_count(tree.root) == TEST_RANGE - DEL_COUNT, "Node count doesn't match.");
    }

    kfree(nodes);

    ktest_passed();
}
