
#include "test.h"
#include "compare.h"
#include "rbtree.h"

#include "../test_rbtree.h"

void test_main(void) {
    test_start("rbtree/prop-test");

    node_t root, left, right;
    rbnode_t *tree = &root.tree_link;

    rbnode_init(tree);
    rbnode_init(&left.tree_link);
    rbnode_init(&right.tree_link);

    root.tree_link.left = &left.tree_link;
    root.tree_link.right = &right.tree_link;

    left.tree_link.color = COLOR_RED;
    root.tree_link.color = COLOR_RED;
    right.tree_link.color = COLOR_BLACK;

    test_assertf(!has_red_property(tree, NULL), "Red property check has returned false positive.");
    test_assertf(!has_depth_property(tree, 0), "Depth property check has returned false positive.");

    left.tree_link.color = COLOR_RED;
    root.tree_link.color = COLOR_BLACK;
    right.tree_link.color = COLOR_BLACK;

    test_assertf(has_red_property(tree, NULL), "Red property check has returned false negative.");
    test_assertf(!has_depth_property(tree, 0), "Depth property check has returned false positive.");

    left.value = 1;
    root.value = 2;
    right.value = 3;
    test_assertf(has_order_property(tree, node_comparison), "Order property check has returned false negative.");

    left.value = 2;
    root.value = 1;
    right.value = 3;
    test_assertf(!has_order_property(tree, node_comparison), "Order property check has returned false positive.");

    left.value = 1;
    root.value = 3;
    right.value = 2;
    test_assertf(!has_order_property(tree, node_comparison), "Order property check has returned false positive.");

    test_passed();
}
