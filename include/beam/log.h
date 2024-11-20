/// file      : log.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// logging suppor

#ifndef BEAM_LOG_H
#define BEAM_LOG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define LOG_FATAL(fmt, ...)                                                                        \
    LogWrite(LOG_MESSAGE_TYPE_FATAL, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...)                                                                        \
    LogWrite(LOG_MESSAGE_TYPE_ERROR, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...)                                                                         \
    LogWrite(LOG_MESSAGE_TYPE_INFO, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

typedef enum {
    LOG_MESSAGE_TYPE_FATAL,
    LOG_MESSAGE_TYPE_ERROR,
    LOG_MESSAGE_TYPE_INFO
} LogMessageType;

///
/// Generate the log message
///
/// type[in]   : Log message type (info, error, fatal)
/// tag[in]    : Log message idenfifier, something like file, function, name etc...
/// line[in]   : Line number at which this log was generated.
/// format[in] : Format string and following variadic arguments
///
void LogWrite(LogMessageType type, const char *tag, int line, const char *format, ...);

#endif // BEAM_LOG_H
