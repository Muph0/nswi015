
#include "test.h"
#include "compare.h"
#include "rbtree.h"

#include "../test_rbtree.h"

#define insert(tree, VAL) \
    node_t node##VAL; \
    node##VAL.value = VAL; \
    rbnode_init(&node##VAL.tree_link); \
    rbtree_insert(&(tree), &node##VAL.tree_link)

void test_main(void) {
    test_start("rbtree/insert-c3");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    insert(tree, 4);
    insert(tree, 6);
    insert(tree, 2);
    insert(tree, 3);

    test_expect(dump_tree(tree.root, NULL), "(B:4 (B:2 B (R:3 B B)) (B:6 B B))");

    test_passed();
}
