
#ifndef _TEST_RBTREE_H
#define _TEST_RBTREE_H

#include "compare.h"
#include "rbtree.h"

#include <stdio.h>

#define putch putchar
#define printf printf

typedef struct {
    int value;
    rbnode_t tree_link;
} node_t;

static inline int node_comparison(const rbnode_t* rb1, const rbnode_t* rb2) {
    node_t* n1 = rbtree_item(rb1, node_t, tree_link);
    node_t* n2 = rbtree_item(rb2, node_t, tree_link);

    return comparison_default(n1->value, n2->value);
}

static inline int node_value(const rbnode_t* rbnode) {
    node_t* node = rbtree_item(rbnode, node_t, tree_link);
    return node->value;
}

static void dump_tree(const rbnode_t* node, const rbnode_t* parent) {
    if (node != NULL) {
        printf("(%c:%d ", node->color == COLOR_RED ? 'R' : 'B', node_value(node));
        dump_tree(node->left, node);
        putch(' ');
        dump_tree(node->right, node);
        putch(')');
    } else {
        putch('B');
    }

    if (parent == NULL) { putch('\n'); }
}

static bool has_red_property(const rbnode_t* node, const rbnode_t* parent) {
    if (node != NULL) {
        if (parent && node->color == COLOR_RED && parent->color == COLOR_RED) {
            return false;
        } else {
            return has_red_property(node->left, node) && has_red_property(node->right, node);
        }
    } else {
        return true;
    }
}

static int has_depth_property(const rbnode_t* node, const int depth) {
    if (node != NULL) {
        int left_depth = has_depth_property(node->left, node->color == COLOR_RED ? depth : depth + 1);
        int right_depth = has_depth_property(node->right, node->color == COLOR_RED ? depth : depth + 1);
        if (left_depth == 0 || left_depth != right_depth) {
            return 0;
        } else {
            return right_depth;
        }
    } else {
        return depth + 1;
    }
}

static bool has_order_property(const rbnode_t* node, rbnode_comparison_t const cmp) {
    if (node == NULL) {
        return true;
    } else if ((node->left == NULL || compare(cmp, node->left, <=, node))
            && (node->right == NULL || compare(cmp, node, <=, node->right))) {
        return has_order_property(node->left, cmp) && has_order_property(node->right, cmp);
    } else {
        return false;
    }
}

static int node_count(const rbnode_t* node) {
    if (node != NULL) {
        return 1 + node_count(node->left) + node_count(node->right);
    } else {
        return 0;
    }
}

#endif
