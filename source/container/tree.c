/// file      : container/tree.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Generic tree implementation

// beam
#include <beam/log.h>
#include <beam/container/tree.h>

GenericTreeNode *deinit_tree_node(GenericTreeNode *node, GenericTree *tree, size_t item_size) {
    if(!node || !tree || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(tree->copy_deinit) {
        tree->copy_deinit(node);
    } else {
        memset(node->data, 0, item_size);
    }

    free(node->data);
    node->data   = NULL;
    node->parent = NULL;

    GenericTreeNode *child;
    size_t           iter;
    VecForeach(&node->children, child, iter) {
        deinit_tree_node(child, tree, item_size);
    }
    VecDeinit(&node->children);

    return node;
}


GenericTree *init_tree(
    GenericTree      *tree,
    size_t            item_size,
    GenericCopyInit   copy_init,
    GenericCopyDeinit copy_deinit
) {
    if(!tree || !item_size || !copy_init || !copy_deinit) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    deinit_tree(tree, item_size);

    tree->copy_init   = copy_init;
    tree->copy_deinit = copy_deinit;

    return tree;
}


GenericTree *deinit_tree(GenericTree *tree, size_t item_size) {
    if(!tree || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    deinit_tree_node(tree->root, tree, item_size);
    free(tree->root);
    memset(tree, 0, sizeof(GenericTree));

    return tree;
}


GenericTreeNode *insert_into_tree_node(
    GenericTreeNode *node,
    GenericTree     *tree,
    size_t           item_size,
    void            *data,
    size_t           idx
) {
    if(!node || !tree || !item_size || !data) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    GenericTreeNode tmpnode = {0};

    tmpnode.data = calloc(item_size, 1);
    if(!tmpnode.data) {
        LOG_ERROR("malloc() failed : %s.", strerror(errno));
        return NULL;
    }

    if(tree->copy_init) {
        tree->copy_init(tmpnode.data, node->data);
    } else {
        memcpy(tmpnode.data, node->data, item_size);
    }

    tmpnode.parent = node;

    // will just be a memcpy because no copier is specified
    if(!VecInsert(&node->children, &tmpnode, idx)) {
        LOG_ERROR("child node insertion failed.");

        if(tree->copy_deinit) {
            tree->copy_deinit(tmpnode.data);
        } else {
            memset(tmpnode.data, 0, item_size);
        }
        free(tmpnode.data);
        tmpnode.parent = NULL;

        return NULL;
    }

    memset(&tmpnode, 0, sizeof(GenericTreeNode));

    return node;
}
