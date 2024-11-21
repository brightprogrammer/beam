/// file      : container/string.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// String class

#ifndef BEAM_CONTAINER_STRING_H
#define BEAM_CONTAINER_STRING_H

#include <string.h>

// beam
#include <beam/container/vec.h>

typedef Vec(char) String;

#define TempStringFromCStr(str, cstr, len)                                                         \
    do {                                                                                           \
        (str)->data        = (char*)(cstr);                                                        \
        (str)->length      = (len);                                                                \
        (str)->capacity    = (len);                                                                \
        (str)->copy_init   = NULL;                                                                 \
        (str)->copy_deinit = NULL;                                                                 \
    } while(0)

#define TempStringFromZStr(str, zstr) TempStringFromCStr(str, zstr, strlen(zstr))

///
/// Initialize given string.
///
/// str : Pointer to string memory that needs to be initialized.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringInit(str) VecInit(str, NULL, NULL)

///
/// Create a new string with given cstring of given length.
///
/// str[in,out] : String to be initialized.
/// cstr[in]    : const char array to create string from.
/// len[in]     : Length to consume.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringInitFromCStr(str, cstr, len) StringPushCStr(StringInit(str), (void*)cstr, len)

///
/// Create a new string with given null-terminated string
///
/// str[in,out] : String to be initialized.
/// cstr[in]    : const char array to create string from.
/// len[in]     : Length to consume.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringInitFromZStr(str, zstr) StringPushZStr(StringInit(str), (void*)zstr)

///
/// Deinit vec by freeing all allocations.
///
/// str : Pointer to string to be destroyed
///
#define StringDeinit(str) VecDeinit(str)

///
/// Insert char into string of it's type.
/// Insertion index must not exceed string length.
///
/// str[in] : String to insert char into
/// chr[in] : Character to be inserted
/// idx[in] : Index to insert char at.
///
/// SUCCESS : Returns `str` the string itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define StringInsertCharAt(str, chr, idx) VecInsert(str, ((char[])({chr})), idx)

///
/// Push char into string.
///
/// str[in] : String to push char into
/// chr[in] : Pointer to value to be pushed
///
/// SUCCESS : Returns `str` the string itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define StringPushBack(str, chr) VecPushBack(str, ((char[])({chr}))

///
/// Pop char from string back.
///
/// str[in,out] : String to pop char from.
/// val[out]    : Popped char will be stored here. Make sure this has sufficient memory
///              to store memcopied data. If no pointer is provided, then it's equivalent
///              to deleting char from last position.
///
/// SUCCESS : Returns `str` on success
/// FAILURE : Returns NULL otherwise.
///
#define StringPopBack(str, chr) VecPopBack(str, chr)

///
/// Push char into string front.
///
/// str[in] : String to push char into
/// chr[in] : Pointer to value to be pushed
///
/// SUCCESS : Returns `str` the string itself on success.
/// FAILURE : Returns `NULL` otherwise.
///
#define StringPushFront(str, chr) VecPushFront(str, ((char[])({chr}))

///
/// Pop char from string front.
///
/// str[in,out] : String to pop char from.
/// val[out]    : Popped char will be stored here. Make sure this has sufficient memory
///              to store memcopied data. If no pointer is provided, then it's equivalent
///              to deleting char from last position.
///
/// SUCCESS : Returns `str` on success
/// FAILURE : Returns NULL otherwise.
///
#define StringPopFront(str, chr) VecPopFront(str, chr)

///
/// Remove char from string at given index and store in given pointer.
///
/// str[in,out] : String to remove char from.
/// val[out]  : Where removed char will be stored. If not provided then it's equivalent to
///             deleting the char at specified index.
/// idx[in]   : Index in string to remove char from.
///
/// SUCCESS : Returns `str` on success.
/// FAILURE : Returns NULL otherwise.
///
#define StringRemoveCharAt(str, chr, idx) VecRemove(str, chr, idx)

///
/// Remove data from string in given range [start, start + count)
///
/// str[in,out] : String to remove char from.
/// rd[out]   : Where removed data will be stored. If not provided then it's equivalent to
///             deleting the chars in specified range.
/// start[in] : Index in string to removing chars from.
/// count[in] : Number of chars from starting index.
///
/// SUCCESS : Returns `str` on success.
/// FAILURE : Returns NULL otherwise.
///
#define StringRemoveRange(str, rd, start, count) VecRemoveRange(v, rd, start, count)

///
/// Delete last char from vec
///
#define StringDeleteLastChar(str) VecDeleteLast(str)

///
/// Delete char at given index
///
#define StringDeleteCharAt(str, idx) VecDelete(str, idx)

///
/// Delete chars in given range [start, start + count)
///
#define StringDeleteRange(str, start, count) VecRemoveRange(str, NULL, start, count)

///
/// Try reducing memory footprint of string.
/// This is to be used when we know actual allocated memory for vec is large,
/// and we won't need it in future, so we can reduce it to whatever's required at
/// the moment.
///
/// str[in,out] : String
///
/// SUCCESS : `str` on success
/// FAILURE : NULL
///
#define StringTryReduceSpace(str) VecTryReduceSpace(str)

///
/// Swap chars at given indices.
///
/// str[in,out] : String to swap chars in.
/// idx1[in]  : Index/Position of first char.
/// idx1[in]  : Index/Position of second char.
///
/// SUCCESS : `str` on success
/// FAILURE : NULL
///
#define StringSwapCharAt(str, idx1, idx2) VecSwapchars(str, idx1, idx2)

///
/// Resize string.
/// If length is smaller than current capacity, string length is shrinked.
/// If length is greater than current capacity, space is reserved and string is expanded.
///
/// vec[in,out] : String to be resized.
/// len[in]     : New length of string.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///

///
/// Resize string.
/// If length is smaller than current capacity, string length is shrinked.
/// If length is greater than current capacity, space is reserved and string is expanded.
///
/// vec[in,out] : String to be resized.
/// len[in]     : New length of string.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringResize(str, len) VecResize(str, len)

///
/// Set string length to 0.
///
/// vec[in,out] : String to be cleared.
///
/// SUCCESS :
/// FAILURE : NULL
///
#define StringClear(str) VecClear(str)

#define StringFirst(str)  VecFirst(str)
#define StringLast(str)   VecLast(str)
#define StringBegin(str)  VecBegin(str)
#define StringEnd(str)    VecEnd(str)
#define StringIter(str)   VecIter(str, idx)
#define StringCharAt(str) VecAt(str, idx)

///
/// Reserve space for string.
///
/// vec[in,out] : String to be resized.
/// len[in]     : New capacity of string.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringReserve(str, n) VecReserve(v, n)

///
/// Push a c-style-string into this string.
///
/// str[in,out] : String to insert array chars into.
/// cstr[in]    : C-style-string to be inserted.
/// len [in]    : Number of characters to be appended.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringPushCStr(str, cstr, len) VecPushArr(str, cstr, count)

///
/// Push a null-terminated string to this string.
///
/// str[in,out] : String to insert array chars into.
/// zstr[in]    : Null-terminated string to be appended.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringPushZStr(str, zstr) zstr ? VecPushArr(str, zstr, strlen(zstr)) : NULL;

///
/// Merge two strings and store the result in first string.
///
/// str[in,out] : String to insert array chars into.
/// str2[in]    : String to be inserted.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringMerge(str, str2) VecMerge(str, str2)

///
/// Reverse contents of this string.
///
/// str[in,out] : String to be reversed.
///
/// SUCCESS : `str`
/// FAILURE : NULL
///
#define StringReverse(str) VecReverse(str)

#define StringForeach(str, chr, iter) VecForeach(str, chr, iter)

#define StrForeachReverse(str, chr, iter) VecForeachReverse(str, chr, iter)

#define StrForeachPtr(str, chrptr, iter) VecForeachPtr(str, chrptr, iter)

#define StrForeachPtrReverse(str, chrptr, iter) VecForeachPtrReverse(str, chrptr, iter)

String* StringInitCopy(String* dst, String* src);
String* StringDeinitCopy(String* copy);

#endif // BEAM_CONTAINER_STRING_H
