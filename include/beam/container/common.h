/// file      : container/common.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Common definitions for all containers

#ifndef BEAM_CONTAINERS_COMMON_H
#define BEAM_CONTAINERS_COMMON_H

// All deinit methods are expected to properly deinitialize all pointers
// to NULL. It's better if data is memset to 0.
//
// All init methods must expect to get a pre-initialized dst object.
// If that is the case then they must properly de-initialize the dst object
// or attempt to reuse any resources if possible and then initialize the copy
// from src to dst.

typedef void *(*GenericCopyInit)(void *dst, void *src);
typedef void *(*GenericCopyDeinit)(void *copy);
typedef int (*GenericCompare)(const void *first, const void *second);

#endif // BEAM_CONTAINERS_COMMON_H
