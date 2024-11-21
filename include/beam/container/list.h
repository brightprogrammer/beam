/// file      : container/list.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Provides a type-safe list implementation in C

#ifndef BEAM_CONTAINER_LIST_H
#define BEAM_CONTAINER_LIST_H

#include <stddef.h>
#include <stdint.h>

// beam
#include <beam/container/common.h>

typedef struct __attribute__((packed)) GenericListNode GenericListNode;

struct __attribute__((packed)) GenericListNode {
    GenericListNode *next;
    GenericListNode *prev;
};

///
/// Get pointer to data inside a generic node.
///
#define GENERIC_LIST_NODE_DATA_PTR(node) ((void *)((char *)(node) + 2 * sizeof(GenericListNode *)))

typedef struct {
    GenericListNode  *head;
    GenericListNode  *tail;
    GenericCopyInit   copy_init;
    GenericCopyDeinit copy_deinit;
    size_t            length;
} GenericList;

///
/// Cast any list to a generic list
///
#define GENERIC_LIST(list) ((GenericList *)(void *)(list))

///
/// Cast any list node to a generic list node
///
#define GENERIC_LIST_NODE(node) ((GenericListNode *)(void *)(node))

///
/// Doubly-linked list
///
#define ListNode(T)                                                                                \
    struct __attribute__((packed)) {                                                               \
        GenericListNode *next;                                                                     \
        GenericListNode *prev;                                                                     \
        T                data;                                                                     \
    }

///
/// Get data type stored by this list
///
#define LIST_DATA_TYPE(list) __typeof__((list)->head->data)

///
/// Get node type stored by this list
///
#define LIST_NODE_TYPE(list) ListNode(LIST_DATA_TYPE(list))

///
/// Get item after given list item
///
#define ListItemNext(item) ((__typeof__(item))((item) ? (item)->next : NULL))

///
/// Single linked list
///
#define List(T)                                                                                    \
    struct {                                                                                       \
        ListNode(T) * head;                                                                        \
        ListNode(T) * tail;                                                                        \
        GenericCopyInit   copy_init;                                                               \
        GenericCopyDeinit copy_deinit;                                                             \
        size_t            length;                                                                  \
    }

///
/// Initialize given list.
///
/// l[in]  : Pointer to list memory that needs to be initialized.
/// ci[in] : Copy init method.
/// cd[in] : Copy deinit method.
///
/// SUCCESS : Returns `v` on success
/// FAILURE : Returns NULL otherwise
///
#define ListInit(l, ci, cd)                                                                        \
    (__typeof__(l))(init_list(                                                                     \
        GENERIC_LIST(l),                                                                           \
        sizeof(LIST_DATA_TYPE(l)),                                                                 \
        (GenericCopyInit)(void *)(ci),                                                             \
        (GenericCopyDeinit)(void *)(cd)                                                            \
    ))

///
/// Deinit list by freeing all allocations.
///
/// v : Pointer to list to be deinited
///
#define ListDeinit(l) deinit_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)))

///
/// Insert item into list of it's type.
/// Insertion index must not exceed list length.
///
/// l[in]   : List to insert item into
/// val[in] : Pointer to value to be inserted
/// idx[in] : Index to insert item at.
///
/// SUCCESS : Returns `v` the list itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define ListInsert(l, val, idx)                                                                    \
    ((__typeof__(l))(insert_into_list(GENERIC_LIST(l), (val), sizeof(LIST_DATA_TYPE(l)), (idx))))

///
/// Remove item from list at given index and store in given pointer.
/// Order of elements is guaranteed to be preserved.
///
/// l[in,out] : List to remove item from.
/// val[out]  : Where removed item will be stored. If not provided then it's equivalent to
///             deleting the item at specified index.
/// idx[in]   : Index in list to remove item from.
///
/// SUCCESS : Returns `v` on success.
/// FAILURE : Returns NULL otherwise.
///
#define ListRemove(l, val, idx) ((__typeof__(l))remove_range_list(GENERIC_LIST(l), (val), sizeof(LIST_DATA_TYPE(l)), ((l)->length - 1), 1)

///
/// Insert item at the very beginning of list.
///
/// l[in]   : List to push item into
/// val[in] : Pointer to value to be prepended
///
/// SUCCESS : Returns `v` the list itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define ListPushFront(l, val) ListInsert((l), (val), 0);

///
/// Remove item from the very beginning of list.
///
/// l[in]   : List to push item into
/// val[in] : Pointer to value to be prepended
///
/// SUCCESS : Returns `v` the list itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define ListPopFront(l, val) ListRemove((l), (val), 0);

///
/// Push item at the back of list.
///
/// l[in]   : List to push item into
/// val[in] : Pointer to value to be pushed
///
/// SUCCESS : Returns `v` the list itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define ListPushBack(l, val) ListInsert((l), (val), ((l) ? (l)->length : -1))

///
/// Pop item from list back.
///
/// l[in,out]  : List to pop item from.
/// val[out]   : Popped item will be stored here. Make sure this has sufficient memory
///              to store memcopied data. If no pointer is provided, then it's equivalent
///              to deleting item from last position.
///
/// SUCCESS : Returns `v` on success
/// FAILURE : Returns NULL otherwise.
///
#define ListPopBack(l, val) ListRemove((l), (val), ((l) ? (l)->length - 1 : -1), -1);

///
/// Remove data from list in given range [start, start + count)
/// Order of elements is guaranteed to be preserved.
///
/// l[in,out] : List to remove item from.
/// rd[out]   : Where removed data will be stored. If not provided then it's equivalent to
///             deleting the items in specified range.
/// start[in] : Index in list to removing items from.
/// count[in] : Number of items from starting index.
///
/// SUCCESS : Returns `v` on success.
/// FAILURE : Returns NULL otherwise.
///
#define ListRemoveRange(l, rd, start, count)                                                       \
    ((__typeof__(l))remove_range_list(GENERIC_LIST(l), (rd), sizeof(*(v)->data), (start), (count)))

///
/// Delete last item from list
///
#define ListDeleteLast(l) ListPop((l), NULL)

///
/// Delete item at given index
///
#define ListDelete(l, idx) ListRemove((l), NULL, (idx))

///
/// Delete items in given range [start, start + count)
///
#define ListDeleteRange(l, start, count) ListRemoveRange((l), NULL, (start), (count))

///
/// Sort given list with given comparator using quicksort algorithm.
///
/// l[in,out]  : List to be sorted.
/// compare[in] : Compare function. Signature and behaviour must be similar to that of `strcmp`.
///
/// SUCCESS : Returns `v` on success.
/// FAILURE : Returns NULL otherwise.
///
#define ListSort(l, compare)                                                                       \
    ((__typeof__(l))qsort_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), (compare)))

///
/// Swap items at given indices.
///
/// l[in,out] : List to swap items in.
/// idx1[in]  : Index/Position of first item.
/// idx1[in]  : Index/Position of second item.
///
/// SUCCESS : `v` on success
/// FAILURE : NULL
///
#define ListSwapItems(l, idx1, idx2)                                                               \
    ((__typeof__(l))swap_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), (idx1), (idx2)))

///
/// Set list length to 0.
///
/// list[in,out] : List to be cleared.
///
/// SUCCESS :
/// FAILURE : NULL
///
#define ListClear(l) ((__typeof__(l))clear_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l))))

///
/// Item at given index in list
///
#define ListAt(l, idx)                                                                             \
    *((LIST_DATA_TYPE(l) *)item_ptr_at_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), (idx)))

///
/// Value at first node in list
///
#define ListFirst(l) ListAt(l, 0)

///
/// Value at last node in list
///
#define ListLast(l) ListAt(l, ((l) ? (l)->length : -1))

///
/// Node at given index in list
///
#define ListNodeAt(l, idx)                                                                         \
    ((LIST_NODE_TYPE(l) *)(node_at_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), (idx))))

///
/// Push a complete array into this list.
///
/// l[in,out] : List to insert array items into.
/// arr[in]   : Array to be inserted.
/// count[in] : Number (non-zero) of items in array.
///
/// SUCCESS : `v`
/// FAILURE : NULL
///
#define ListPushArr(l, arr, count)                                                                 \
    ((__typeof__(l))push_arr_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), (arr), (count)))

///
/// Merge two lists and store the result in first list.
///
/// l[in,out] : List to insert array items into.
/// l2[in]   : Array to be inserted.
///
/// SUCCESS : `v`
/// FAILURE : NULL
///
#define ListMerge(l, l2) merge_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)), GENERIC_LIST(l2))

///
/// Reverse contents of this list.
///
/// l[in,out] : List to be reversed.
///
/// SUCCESS : `v`
/// FAILURE : NULL
///
#define ListReverse(l) reverse_list(GENERIC_LIST(l), sizeof(LIST_DATA_TYPE(l)))

#define ListForeach(l, var, iter)                                                                  \
    (iter) = 0;                                                                                    \
    if((l) && (l)->length > 0)                                                                     \
        for(GenericListNode *__node = GENERIC_LIST_NODE((l)->head);                                \
            __node != NULL && (((var) = ((LIST_NODE_TYPE(l) *)__node)->data), 1);                  \
            ++(iter), __node = __node->next)

#define ListForeachReverse(l, var, iter)                                                           \
    (iter) = 0;                                                                                    \
    if((l) && (l)->length > 0)                                                                     \
        for(GenericListNode *__node = GENERIC_LIST_NODE((l)->tail);                                \
            __node != NULL && (((var) = ((LIST_NODE_TYPE(l) *)__node)->data), 1);                  \
            ++(iter), __node = __node->prev)

#define ListForeachPtr(l, var, iter)                                                               \
    (iter) = 0;                                                                                    \
    if((l) && (l)->length > 0)                                                                     \
        for(GenericListNode *__node = GENERIC_LIST_NODE((l)->head);                                \
            __node != NULL && (((var) = &((LIST_NODE_TYPE(l) *)__node)->data), 1);                 \
            ++(iter), __node = __node->next)

#define ListForeachPtrReverse(l, var, iter)                                                        \
    (iter) = 0;                                                                                    \
    if((l) && (l)->length > 0)                                                                     \
        for(GenericListNode *__node = GENERIC_LIST_NODE((l)->tail);                                \
            __node != NULL && (((var) = &((LIST_NODE_TYPE(l) *)__node)->data), 1);                 \
            ++(iter), __node = __node->prev)

GenericList *init_list(
    GenericList      *list,
    size_t            item_size,
    GenericCopyInit   copy_init,
    GenericCopyDeinit copy_deinit
);
void         deinit_list(GenericList *list, size_t item_size);
GenericList *insert_into_list(GenericList *list, void *item_data, size_t item_size, size_t idx);
GenericList *remove_range_list(
    GenericList *list,
    void        *removed_data,
    size_t       item_size,
    size_t       start,
    size_t       count
);
GenericList *
    qsort_list(GenericList *list, size_t item_size, int (*comp)(const void *, const void *));
GenericList     *swap_list(GenericList *list, size_t item_size, size_t idx1, size_t idx2);
GenericList     *reverse_list(GenericList *list, size_t item_size);
GenericList     *push_arr_list(GenericList *list, size_t item_size, void *arr, size_t count);
GenericList     *merge_list(GenericList *list1, size_t item_size, GenericList *list2);
GenericList     *resize_list(GenericList *list, size_t item_size, size_t new_size);
GenericList     *clear_list(GenericList *list, size_t item_size);
GenericListNode *node_at_list(GenericList *list, size_t item_size, size_t idx);
void            *item_ptr_at_list(GenericList *list, size_t item_size, size_t idx);

#endif // BEAM_CONTAINER_LIST_H
