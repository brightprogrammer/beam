/// file      : container/list.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Generic list implementation

#include <stdlib.h>

// beam
#include <beam/container/list.h>
#include <beam/log.h>

GenericList *init_list(
    GenericList      *list,
    size_t            item_size,
    GenericCopyInit   copy_init,
    GenericCopyDeinit copy_deinit
) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    list->copy_init   = copy_init;
    list->copy_deinit = copy_deinit;
    deinit_list(list, item_size);

    return list;
}


void deinit_list(GenericList *list, size_t item_size) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments");
        return;
    }

    clear_list(list, item_size);

    memset(list, 0, sizeof(GenericList));
}


GenericList *insert_into_list(GenericList *list, void *item_data, size_t item_size, size_t idx) {
    if(!list || !item_size || !item_data) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    GenericListNode *new_node = malloc(sizeof(GenericListNode) + item_size);
    if(!new_node) {
        LOG_ERROR("malloc() failed : %s.", strerror(errno));
        return NULL;
    }

    if(idx < list->length) {
        // get node where insertion will take place
        GenericListNode *node = node_at_list(list, item_size, idx);
        if(!node) {
            LOG_ERROR("failed to get node.\n");
            free(new_node);
            return NULL;
        }

        // shift data from current node to new node
        memcpy(GENERIC_LIST_NODE_DATA_PTR(new_node), GENERIC_LIST_NODE_DATA_PTR(node), item_size);

        // create dual link
        // add new node after current node
        new_node->next = node->next;
        new_node->prev = node;
        node->next     = new_node;

        // insert data new data into current node
        if(list->copy_init) {
            memset(GENERIC_LIST_NODE_DATA_PTR(node), 0, item_size);
            list->copy_init(GENERIC_LIST_NODE_DATA_PTR(node), item_data);
        } else {
            memcpy(GENERIC_LIST_NODE_DATA_PTR(node), item_data, item_size);
        }
    } else if(idx >= list->length) {
        idx            = list->length;
        new_node->next = NULL;

        GenericListNode *tail = node_at_list(list, item_size, -1);

        // if there is no tail, current list length is 0
        if(!tail) {
            list->head     = new_node;
            list->tail     = new_node;
            new_node->prev = NULL;
            new_node->next = NULL;

            if(list->copy_init) {
                memset(GENERIC_LIST_NODE_DATA_PTR(new_node), 0, item_size);
                list->copy_init(GENERIC_LIST_NODE_DATA_PTR(new_node), item_data);
            } else {
                memcpy(GENERIC_LIST_NODE_DATA_PTR(new_node), item_data, item_size);
            }
            return list;
        } else {
            // create dual link & update tail
            list->tail->next = new_node;
            new_node->prev   = list->tail;
            new_node->next   = NULL;
            list->tail       = new_node;

            if(list->copy_init) {
                memset(GENERIC_LIST_NODE_DATA_PTR(new_node), 0, item_size);
                list->copy_init(GENERIC_LIST_NODE_DATA_PTR(new_node), item_data);
            } else {
                memcpy(GENERIC_LIST_NODE_DATA_PTR(new_node), item_data, item_size);
            }
            tail->next = new_node;
        }
    }

    list->length += 1;

    return list;
}

GenericList *remove_range_list(
    GenericList *list,
    void        *removed_data,
    size_t       item_size,
    size_t       start,
    size_t       count
) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(start + count > list->length) {
        LOG_ERROR("listtor range out of bounds.");
        return NULL;
    }

    // if a buffer is provided, move data there
    if(removed_data) {
        GenericListNode *node = node_at_list(list, item_size, start);
        for(size_t c = 0; (c < count) && node; c++) {
            memcpy(removed_data + c * item_size, GENERIC_LIST_NODE_DATA_PTR(node), item_size);
            node = node->next;
        }
    } else {
        // else destroy all data one by one
        GenericListNode *node = node_at_list(list, item_size, start);
        for(size_t c = 0; (c < count) && node; c++) {
            if(list->copy_deinit) {
                list->copy_deinit(GENERIC_LIST_NODE_DATA_PTR(node));
            } else {
                memset(GENERIC_LIST_NODE_DATA_PTR(node), 0, item_size);
            }
            node = node->next;
        }
    }

    // remove nodes
    GenericListNode *node = node_at_list(list, item_size, start);
    while(node && count-- && list->length--) {
        // update link
        GenericListNode *next = node->next;
        GenericListNode *prev = node->prev;
        prev->next            = next;
        next->prev            = prev;

        // remove link
        node->next = NULL;
        node->prev = NULL;

        // destroy and move ahead
        free(node);
        node = next;
    }

    return list;
}


GenericList *
    qsort_list(GenericList *list, size_t item_size, int (*comp)(const void *, const void *)) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    void *data = malloc(item_size * list->length);
    if(!data) {
        LOG_ERROR("malloc() failed : %s.", strerror(errno));
        return NULL;
    }
    size_t item_count = list->length;
    remove_range_list(list, data, item_size, 0, -1);
    qsort(data, item_count, item_size, comp);
    push_arr_list(list, item_size, data, item_count);
    free(data);

    return list;
}


GenericList *swap_list(GenericList *list, size_t item_size, size_t idx1, size_t idx2) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    GenericListNode *n1 = node_at_list(list, item_size, idx1);
    if(!n1) {
        LOG_ERROR("failed to get node at specified index");
        return NULL;
    }

    GenericListNode *n2 = node_at_list(list, item_size, idx2);
    if(!n2) {
        LOG_ERROR("failed to get node at specified index");
        return NULL;
    }

    unsigned char *a, *b, tmp;
    a = GENERIC_LIST_NODE_DATA_PTR(n1);
    b = GENERIC_LIST_NODE_DATA_PTR(n2);
    while(item_size--) {
        tmp = *a;
        *a  = *b;
        *b  = tmp;
        a++, b++;
    }

    return list;
}


GenericList *reverse_list(GenericList *list, size_t item_size) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    size_t i = list->length / 2;
    while(i--) {
        swap_list(list, item_size, i, list->length - (i + 1));
    }

    return list;
}


GenericList *push_arr_list(GenericList *list, size_t item_size, void *arr, size_t count) {
    if(!list || !arr || !count || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    while(count-- && list->length++) {
        GenericListNode *old_tail = list->tail;

        // new tail
        GenericListNode *new_tail = malloc(sizeof(GenericListNode) + item_size);
        if(!new_tail) {
            LOG_ERROR("malloc() failed : %s", strerror(errno));
            return NULL;
        }

        // create new dual link
        old_tail->next = new_tail;
        new_tail->prev = old_tail;
        new_tail->next = NULL;
        list->tail     = new_tail;

        // insert data
        if(list->copy_init) {
            list->copy_init(GENERIC_LIST_NODE_DATA_PTR(new_tail), arr);
        } else {
            memcpy(GENERIC_LIST_NODE_DATA_PTR(new_tail), arr, item_size);
        }

        arr += item_size;
    }

    return list;
}


GenericList *merge_list(GenericList *list1, size_t item_size, GenericList *list2) {
    if(!list1 || !item_size || !list2) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    GenericListNode *node = list2->head;
    while(node) {
        if(!insert_into_list(list1, GENERIC_LIST_NODE_DATA_PTR(node), item_size, list1->length)) {
            LOG_ERROR("failed into insert into list.");
            return NULL;
        }
        node = node->next;
    }

    return list1;
}


GenericList *clear_list(GenericList *list, size_t item_size) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    remove_range_list(list, NULL, item_size, 0, list->length);

    return list;
}


GenericListNode *node_at_list(GenericList *list, size_t item_size, size_t idx) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(idx >= list->length) {
        LOG_ERROR("list index out of range.");
        return NULL;
    }

    GenericListNode *node = list->head;
    while(node && idx--) {
        if(!node->next) {
            return node;
        } else {
            node = node->next;
        }
    }

    return NULL;
}


void *item_ptr_at_list(GenericList *list, size_t item_size, size_t idx) {
    if(!list || !item_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(idx >= list->length) {
        LOG_ERROR("list index out of bounds.");
        return NULL;
    }

    GenericListNode *node = node_at_list(list, item_size, idx);
    return GENERIC_LIST_NODE_DATA_PTR(node);
}
