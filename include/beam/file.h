/// file      : file.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// File helper utilities

#ifndef BEAM_FILE_H
#define BEAM_FILE_H

#include <stdint.h>
#include <stddef.h>

///
/// Get size of file without opening it.
///
/// filename[in] : Name/path of file.
///
/// SUCCESS : Non-negative value representing size of file in bytes.
/// FAILURE : -1
///
int64_t GetFileSize(const char *filename);

///
/// Read complete contents of file at once.
///
/// Pointer returned is malloc'd and hence must be freed after use.
/// The returned pointer can also be reused by providing pointer to it
/// in `data` parameter.
///
/// `realloc` is called on `*data` in order to expand it's size.
/// If `*capacity` exceeds the size of file to be loaded, then no reallocation
/// is performed. This means the provided buffer will automatically be expanded
/// if required.
///
/// The returned buffer is null-terminated just-in-case.
///
/// The implementation and API is designed in such a way that it can be used
/// with containers like Vec and String.
///
/// filename[in]     : Name/path of file to be read.
/// data[in,out]     : Memory buffer where loaded file will be stored.
/// file_size[out]   : Complete size of file in bytes will be stored here.
/// capacity[in,out] : Hints towards current capacity of `data` buffer.
///                    New capacity of `data` buffer is automatically stored here if
///                    realloc is performed.
///
/// SUCCESS : Returns a malloc'd array with read file contents.
/// FAILURE : NULL
///
void *ReadCompleteFile(const char *filename, void **data, size_t *file_size, size_t *capacity);

#endif // BEAM_FILE_H
