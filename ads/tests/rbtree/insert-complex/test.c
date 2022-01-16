
#include "test.h"
#include "compare.h"
#include "rbtree.h"

#include "../test_rbtree.h"

static uint32_t lfsr32(uint32_t* state) {
    for (int i = 0; i < 32; i++) {
        int bit = (*state >> 0) ^ (*state >> 10) ^ (*state >> 30) ^ (*state >> 31);
        *state = (*state >> 1) | (bit << 31);
    }

    return *state;
}

void test_main(void) {
    test_start("rbtree/insert-complex");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    uint32_t seed = 123456;

#define TEST_RANGE 1000

    node_t* nodes = (node_t*)malloc(TEST_RANGE * sizeof(node_t));
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

    for (int i = 0; i < TEST_RANGE; i++) {
        rbtree_insert(&tree, &nodes[i].tree_link);
    }

    test_assertf(tree.root->color == COLOR_BLACK, "Root property violated.");
    test_assertf(has_red_property(tree.root, NULL), "Red property violated.");
    test_assertf(has_depth_property(tree.root, 0), "Depth property violated.");
    test_assertf(has_order_property(tree.root, node_comparison), "Order property violated.");
    test_assertf(node_count(tree.root) == TEST_RANGE, "Node count doesn't match.");

    free(nodes);

    test_passed();
}
