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
struct __attribute__((packed)) GenericTreeNode {
    GenericTreeNode *parent;
    Vec(GenericTreeNode *) children;
};

///
/// Get pointer to data inside a generic node.
///
#define GENERIC_TREE_NODE_DATA_PTR(node)                                                           \
    ((void *)((char *)(node) + sizeof(GenericTreeNode *) + sizeof(Vec(GenericTreeNode *))))

#define TreeNode(T)                                                                                \
    struct __attribute__((packed)) {                                                               \
        GenericTreeNode *parent;                                                                   \
        Vec(GenericTreeNode *) chidlren;                                                           \
        T data;                                                                                    \
    }

#define Tree(T)                                                                                    \
    struct {                                                                                       \
        TreeNode(T) * root;                                                                        \
        size_t            height;                                                                  \
        GenericCopyInit   copy_init;                                                               \
        GenericCopyDeinit copy_deinit                                                              \
    }

#endif // BEAM_CONTAINER_TREE_H
