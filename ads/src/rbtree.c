
#include "rbtree.h"

#undef rbtree_insert
#define is_black(NODE) (rbnode_color(NODE) == COLOR_BLACK)
#define is_red(NODE) (rbnode_color(NODE) == COLOR_RED)

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

static inline rbnode_t* sibling_of(rbnode_t* node, rbnode_t* parent) {
    return node == parent->left ? parent->right : parent->left;
}

static inline rbnode_t* only_child_of(rbnode_t* node) {
    return node->left == NULL ? node->right : node->left;
}

static inline rbnode_t* rotate_left(rbnode_t* node) {
    rbnode_t* newroot = node->right;
    node->right = node->right->left;
    newroot->left = node;
    return newroot;
}

static inline rbnode_t* rotate_right(rbnode_t* node) {
    rbnode_t* newroot = node->left;
    node->left = node->left->right;
    newroot->right = node;
    return newroot;
}

static int __rbtree_insert_repair(
        rbtree_t*, const rbnode_comparison_t, rbnode_t*, rbnode_t*, rbnode_t*);
static bool __rbtree_remove(rbtree_t*, const rbnode_t*, rbnode_t**, rbnode_t*, rbnode_t*);
static int __rbtree_remove_repair(int, rbnode_t**, rbnode_t**);

void rbtree_insert(rbtree_t* tree, rbnode_t* node) {
    assert(tree != NULL && node != NULL);
    __rbtree_insert_repair(tree, tree->comparison, node, NULL, NULL);
}

bool rbtree_remove(rbtree_t* tree, rbnode_t* node) {
    return __rbtree_remove(tree, node, &tree->root, NULL, NULL);
}

//
// insert
//

enum insert_action {
    ACTION_ROTATE_GPARENT = -1,
    INSERT_DONE = 0,
    ACTION_REPAIR = 1,
};

static int __rbtree_insert_repair(rbtree_t* tree, const rbnode_comparison_t cmp,
        rbnode_t* insertedNode, rbnode_t* parent, rbnode_t* gparent) {
    assert(insertedNode != NULL);

    // represents node to which parent, uncle and gparent relate
    rbnode_t** node = &tree->root;

    bool goLeft = false;
    if (parent != NULL) {
        // if inserted node is less than current parent
        if ((goLeft = compare(cmp, insertedNode, <, parent))) {
            node = &parent->left;
        } else {
            node = &parent->right;
        }
    }

    // represents action to take on the way back from recursion
    enum insert_action action;

    if (*node != NULL) { // if node not leaf
        // continue down the tree
        action = __rbtree_insert_repair(tree, cmp, insertedNode, *node, parent);
    } else {
        // the leaf is null, do the insertion assignment
        *node = insertedNode;
        action = ACTION_REPAIR;
    }

    if (action == ACTION_REPAIR) {
        // repaired node should be always red
        assert((*node)->color == COLOR_RED);
        rbnode_t* uncle;

        // case 1: the node is root
        if (parent == NULL) {
            // color the node black
            (*node)->color = COLOR_BLACK;
        }
        // case 2: parent is black
        else if (is_black(parent)) {
            // do nothing; tree is valid
        }
        // case 3: parent and uncle is red -> recolor
        // (parent red => gparent != NULL)
        else if (is_red(uncle = sibling_of(parent, gparent))) {
            // (uncle red => uncle != NULL)
            uncle->color = COLOR_BLACK;
            parent->color = COLOR_BLACK;
            gparent->color = COLOR_RED;

            // repair after 2 returns
            // (i.e. repair gparent)
            return ACTION_REPAIR + 1;
        }
        // case 4: need to rotate
        else {
            // parent red, uncle black
            // first, rotate our node to outer subtree
            if (*node == parent->right && parent == gparent->left) {
                gparent->left = rotate_left(parent);
            } else if (*node == parent->left && parent == gparent->right) {
                gparent->right = rotate_right(parent);
            }

            // now we just need to rotate gparent and recolor
            // but to do that, we need its parent,
            // so we step out of one recursion and do it there
            return ACTION_ROTATE_GPARENT;
        }
    } else if (action == ACTION_ROTATE_GPARENT) {
        // now just rotate parent,
        // for that we need to know, who owns it
        rbnode_t** gparent_owner = gparent == NULL
                ? &tree->root
                : (parent == gparent->left ? &gparent->left : &gparent->right);
        if (goLeft) {
            *gparent_owner = rotate_right(parent);
        } else {
            *gparent_owner = rotate_left(parent);
        }
        parent->color = COLOR_RED;
        (*gparent_owner)->color = COLOR_BLACK;

        // since parent ends up BLACK, no need for more repairing
        return INSERT_DONE;
    }

    // decrement the action for the caller
    return action <= 0 ? INSERT_DONE : action - 1;
}

//
// remove
//

enum remove_repair_action {
    NOT_FOUND = -1,
    REMOVE_REPAIR_DONE = 0,
    REMOVE_CASE_1,
    REMOVE_CASE_2,
    REMOVE_CASE_3,
    REMOVE_CASE_4,
    REMOVE_CASE_5,
    REMOVE_CASE_6
};

static bool __rbtree_remove(rbtree_t* tree, const rbnode_t* removedNode, rbnode_t** node,
        rbnode_t* parent, rbnode_t* gparent) {
    assert(removedNode != NULL);

    enum remove_repair_action action = REMOVE_REPAIR_DONE;
    rbnode_t** parent_link = NULL;
    if (parent != NULL) {
        parent_link = gparent != NULL ? (parent == gparent->left ? &gparent->left : &gparent->right)
                                      : &tree->root;
    }

    if (unlikely(*node == removedNode && removedNode != NULL)) {
        // node hit

        // if node has both children
        if ((*node)->left != NULL && (*node)->right != NULL) {
            // we replace it with its succesor
            rbnode_t** succ = &(*node)->right;
            while ((*succ)->left != NULL) {
                succ = &(*succ)->left;
            }

            // store copy of for swapping:
            rbnode_t succ_copy = **succ;
            rbnode_t* succ_ptr = *succ;
            rbnode_t* node_ptr = *node;

            // relink succ's parent's link to node
            *succ = *node;
            // link node's parent to old succ position
            *node = succ_ptr;

            // swap tree links data
            *succ_ptr = *node_ptr;
            *node_ptr = succ_copy;

            // and then hunt for the original node in the right subtree
            // which is now guaranteed to have only one child
            action = __rbtree_remove(tree, node_ptr, &succ_ptr->right, succ_ptr, parent);
        } else {
            // node has 0 or 1 child

            // remove the node by linking its parent to its only child
            if ((*node)->left != NULL) {
                *node = (*node)->left;
            } else {
                *node = (*node)->right;
            }

            // only bother ourselvs with repair if the removed node is black
            if (removedNode->color == COLOR_BLACK) {
                // if child is red, recolor it and done
                if (is_red(*node)) {
                    (*node)->color = COLOR_BLACK;
                } else {
                    return __rbtree_remove_repair(REMOVE_CASE_1, node, parent_link);
                }
            }
        }

    } else if (*node != NULL) {
        // continue down the tree
        rbnode_t** nextNode;
        if (compare(tree->comparison, removedNode, <, *node)) {
            nextNode = &(*node)->left;
        } else {
            nextNode = &(*node)->right;
        }

        action = __rbtree_remove(tree, removedNode, nextNode, *node, parent);
    } else {
        // hit leaf NULL without finding a node
        return NOT_FOUND;
    }

    if (action != REMOVE_REPAIR_DONE) { return __rbtree_remove_repair(action, node, parent_link); }

    return REMOVE_REPAIR_DONE;
}

static int __rbtree_remove_repair(
        enum remove_repair_action action, rbnode_t** node, rbnode_t** parent) {

    rbnode_t* sibling;
    if (parent != NULL) { sibling = sibling_of(*node, *parent); }

    switch (action) {
    default: break;
    case REMOVE_CASE_1:
        if (parent == NULL || *parent == NULL) {
            // node is root, we are done
            break;
        }
        // node isnt root, move on to next case
        action = REMOVE_CASE_2;
        // fall through
    case REMOVE_CASE_2:
        if (is_red(sibling)) {
            (*parent)->color = COLOR_RED;
            sibling->color = COLOR_BLACK;
            if (*node == (*parent)->left) {
                *node = rotate_left(*node);
            } else {
                *node = rotate_right(*node);
            }
        }
        action = REMOVE_CASE_3;
        // fall through
    case REMOVE_CASE_3:
        // parent NULL ruled out by case 1
        if ((*parent)->color == COLOR_BLACK && is_black(sibling) && is_black(sibling->left)
                && is_black(sibling->right)) {
            sibling->color = COLOR_RED;
            // test case 1 on parent
            return REMOVE_CASE_1;
        }
        // else fall through to case 4
        action = REMOVE_CASE_4;
        // fall through
    case REMOVE_CASE_4:
        if (is_red(*parent) && is_black(sibling) && is_black(sibling->left)
                && is_black(sibling->right)) {
            sibling->color = COLOR_RED;
            (*parent)->color = COLOR_BLACK;
            break; // terminal case
        }
        // else fall through to case 5
        action = REMOVE_CASE_5;
        // fall through
    case REMOVE_CASE_5:
        if (is_black(sibling)) {
            // rotate the red node to outer subtree
            if (node == &(*parent)->left && is_black(sibling->right) && is_red(sibling->left)) {
                sibling->color = COLOR_RED;
                sibling->left->color = COLOR_BLACK;
                sibling = (*parent)->right = rotate_right(sibling);
            } else if (node == &(*parent)->right && is_black(sibling->left)
                    && is_red(sibling->right)) {
                sibling->color = COLOR_RED;
                sibling->right->color = COLOR_BLACK;
                sibling = (*parent)->left = rotate_left(sibling);
            }
        }
        action = REMOVE_CASE_6;
        // fall through
    case REMOVE_CASE_6:
        sibling->color = (*parent)->color;
        (*parent)->color = COLOR_BLACK;
        if (node == &(*parent)->left) {
            if (sibling->right != NULL) { sibling->right->color = COLOR_BLACK; }
            *parent = rotate_left(*parent);
        } else {
            if (sibling->left != NULL) { sibling->left->color = COLOR_BLACK; }
            *parent = rotate_right(*parent);
        }
        // terminal case.
        break;
    }

    return REMOVE_REPAIR_DONE;
}