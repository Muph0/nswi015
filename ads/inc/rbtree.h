/*
 * An intrusive/embedded red-black tree without parent pointer.
 */

#ifndef _DATA_RBTREE_H
#define _DATA_RBTREE_H

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#include "compare.h"
#include "struct.h"

/**
 * Get typed structure from a tree node.
 *
 * @param rbnode  Pointer to the tree node.
 * @param type    The type of the structure the node is embedded in.
 * @param member  The name of the member of the node within
 *                the containing structure.
 *
 * @return Typed structure corresponding to the tree node.
 *
 */
#define rbtree_item(rbnode, type, member) struct_container_of(rbnode, type, member)


/**
 * TODO: Iterate over tree members in order
 *
 * @param tree     The tree to iterate over.
 * @param type     The type of the structure the item is embedded in.
 * @param member   The name of the member of the item within
 *                 the containing structure.
 * @param iterator The name of the iterator to declare.
 *
 */
#define rbtree_foreach(tree, type, member, iterator) ((void)0)

enum rbtree_node_color { COLOR_BLACK, COLOR_RED };
typedef enum rbtree_node_color rbnode_color_t;

/**
 * Polymorphic rbtree node.
 */
typedef struct rbtree_node {
    struct rbtree_node* left;
    struct rbtree_node* right;
    rbnode_color_t color;
} rbnode_t;

/**
 * Represents a comparison function that compares two rbnodes.
 */
typedef comparison_t(rbnode_t*, rbnode_comparison_t);

/**
 * Polymorphic red-black tree
 */
typedef struct rbtree {
    rbnode_t* root;
    rbnode_comparison_t comparison;
} rbtree_t;


/**
 * Checked tree node initialization.
 * Initialize a tree node. Initially, every node is red.
 *
 * @param link The tree node to initialize.
 */
static inline void rbnode_init(rbnode_t* node) {
    assert(node != NULL);

    node->color = COLOR_RED;
    node->left = NULL;
    node->right = NULL;
}

/**
 * Initialize a red-black tree.
 *
 * @param tree The tree to initialize.
 * @param comparison Function that compares two nodes.
 */
static inline void rbtree_init(rbtree_t* tree, rbnode_comparison_t comparison) {
    assert(tree != NULL);
    tree->root = NULL;
    tree->comparison = comparison;
}

/**
 * Get the color of a node. Use this to mask NULLs as black.
 *
 * @param node The node to examine. *
 * @return The color of the node. NULL node returns black.
 */
static inline rbnode_color_t rbnode_color(rbnode_t* node) {
    return node != NULL ? node->color : COLOR_BLACK;
}

/**
 * Test whether a tree is empty
 *
 * @param list The list to examine.
 * @return True if the list is empty.
 */
static inline bool rbtree_is_empty(rbtree_t* tree) {
    assert(tree != NULL);
    return (tree->root == NULL);
}

/**
 * Insert a node into a tree.
 *
 * @param tree The tree to insert into.
 * @param node Pointer to (initialized) node to insert.
 */
void rbtree_insert(rbtree_t* tree, rbnode_t* node);

/**
 * Remove a node from a tree.
 *
 * @param tree The tree to remove from.
 * @param node The node to remove.
 *
 * @return True if remove succeeded.
 */
bool rbtree_remove(rbtree_t* tree, rbnode_t* node);

#endif
