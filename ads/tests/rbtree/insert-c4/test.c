
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
    test_start("rbtree/insert-c4");

    rbtree_t tree;
    rbtree_init(&tree, node_comparison);

    insert(tree, 1);
    insert(tree, 2);
    insert(tree, 3);

    test_expect(dump_tree(tree.root, NULL), "(B:2 (R:1 B B) (R:3 B B))");

    test_passed();
}
