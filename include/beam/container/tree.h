/// file      : container/tree.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Provides a type-safe tree implementation.

#ifndef BEAM_CONTAINER_TREE_H
#define BEAM_CONTAINER_TREE_H

// beam
#include <beam/container/vec.h>

typedef struct GenericTreeNode GenericTreeNode;
struct GenericTreeNode {
    GenericTreeNode *parent;
    Vec(GenericTreeNode *) children;
    void *data;
};

typedef struct {
    GenericTreeNode  *root;
    size_t            height;
    GenericCopyInit   copy_init;
    GenericCopyDeinit copy_deinit;
} GenericTree;

///
/// Cast any list to a generic list
///
#define GENERIC_TREE(t) ((GenericTree *)(void *)(t))

///
/// Cast any list node to a generic list node
///
#define GENERIC_TREE_NODE(node) ((GenericTreeNode *)(void *)(node))

///
/// A type specific tree node
///
#define TreeNode(T)                                                                                \
    struct {                                                                                       \
        GenericTreeNode *parent;                                                                   \
        Vec(GenericTreeNode *) chidlren;                                                           \
        T *data;                                                                                   \
    }

///
/// Type specific tree
///
#define Tree(T)                                                                                    \
    struct {                                                                                       \
        TreeNode(T) * root;                                                                        \
        size_t            height;                                                                  \
        GenericCopyInit   copy_init;                                                               \
        GenericCopyDeinit copy_deinit;                                                             \
    }

///
/// Get data type stored by this tree
///
#define TREE_DATA_TYPE(t) __typeof__((t)->root->data)

///
/// Get node type stored by this list
///
#define TREE_NODE_TYPE(t) TreeNode(TREE_DATA_TYPE((t)))

///
/// Initialize given tree object to a new one.
///
/// t[in,out] : Tree to be initialized
/// ci[in]    : Copy initializer.
/// cd[in]    : Copy de-initializer.
///
/// SUCCESS : `t`
/// FAILURE : NULL
///
#define TreeInit(t, ci, cd)                                                                        \
    ((__typeof__((t)))init_tree(                                                                   \
        GENERIC_TREE(t),                                                                           \
        sizeof(TREE_DATA_TYPE(t)),                                                                 \
        ((GenericCopyInit)(void *)(ci)),                                                           \
        ((GenericCopyDeinit)(void *)(cd))                                                          \
    ))

///
/// Deinitialize given tree object by freeing all resources.
///
/// t[in,out] : Tree to be initialized
///
/// SUCCESS : `t`
/// FAILURE : NULL
///
#define TreeDeinit(t) ((__typeof__((t)))deinit_tree(GENERIC_TREE(t), sizeof(TREE_DATA_TYPE(t))))

///
/// Insert a new node considering given tree node as parent node.
///
/// tn[in,out] : Tree node to insert into.
/// t[in,out]  : Tree containing the tree node.
/// data[in]   : Pointer to data to be inserted.
/// idx[in]    : Index to insert at. Must not be greater than length of children vector.
///
/// SUCCESS : `tn`
/// FAILURE : NULL
///
#define TreeNodeInsert(tn, t, data, idx)                                                           \
    ((__typeof__((t)))insert_into_tree_node(                                                       \
        GENERIC_TREE_NODE(tn),                                                                     \
        GENERIC_TREE(t),                                                                           \
        sizeof(TREE_DATA_TYPE(t)),                                                                 \
        (void *)(data)                                                                             \
    ))

#define TreeNodePushBack(tn, t, data)  TreeNodeInsert(tn, t, data, (tn)->length)
#define TreeNodePushFront(tn, t, data) TreeNodeInsert(tn, t, data, 0)

GenericTree *init_tree(
    GenericTree      *tree,
    size_t            item_size,
    GenericCopyInit   copy_init,
    GenericCopyDeinit copy_deinit
);
GenericTree     *deinit_tree(GenericTree *tree, size_t item_size);
GenericTreeNode *insert_into_tree_node(
    GenericTreeNode *node,
    GenericTree     *tree,
    size_t           item_size,
    void            *data,
    size_t           idx
);

#endif // BEAM_CONTAINER_TREE_H
