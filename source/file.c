/// file      : file.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// File helper utilities implementation

#include <stdlib.h>

// beam
#include <beam/file.h>
#include <beam/log.h>

// platform
#include <sys/stat.h>


int64_t GetFileSize(const char *filename) {
    if(!filename) {
        LOG_ERROR("invalid arguments.");
        return -1;
    }

    struct stat file_stat;
    if(0 == stat(filename, &file_stat)) {
        return file_stat.st_size;
    } else {
        LOG_ERROR("failed to get file size : %s.", strerror(errno));
        return -1;
    }
}


void *ReadCompleteFile(const char *filename, void **data, size_t *file_size, size_t *capacity) {
    if(!filename || !data || !file_size || !capacity) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // get actual size of file
    int64_t size = GetFileSize(filename);
    if(-1 == size) {
        LOG_ERROR("failed to get file size");
        return NULL;
    }

    // allocate memory to hold the file contents if required
    void *buffer = *data;
    if(*capacity < (size_t)size) {
        buffer = realloc(buffer, size + 1);
        if(!buffer) {
            LOG_ERROR("malloc() failed : %s.", strerror(errno));
            return NULL;
        }

        *capacity = size + 1;
    }

    // Open the file in binary mode
    FILE *file = fopen(filename, "rb");
    if(!file) {
        LOG_ERROR("fopen() failed : %s.", strerror(errno));
        free(buffer);
        return NULL;
    }

    // Read the entire file into the buffer
    fread(buffer, 1, size, file);

    // Close the file and return the buffer
    fclose(file);

    ((char *)buffer)[size] = 0; // null-termination for just in case.
    *data                  = buffer;
    *file_size             = size;
    return buffer;
}
