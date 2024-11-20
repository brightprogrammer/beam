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
// All init methods can expect to get dst memsetted to 0. So all fields will be
// invalid by default and using invalid pointers will directly trigger segfault.

typedef void *(*GenericCopyInit)(void *dst, void *src);
typedef void *(*GenericCopyDeinit)(void *copy);

#endif // BEAM_CONTAINERS_COMMON_H
