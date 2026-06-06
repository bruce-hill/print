// This file defines a String(...) macro that works with the Boehm GC
// to allocate formatted strings.
#include <gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

typedef struct {
    char **buffer;
    size_t *size;
    size_t position;
} gc_stream_t;

static ssize_t _gc_stream_write(void *cookie, const char *buf, size_t size) {
    gc_stream_t *stream = (gc_stream_t *)cookie;
    if (stream->position + size + 1 > *stream->size)
        *stream->buffer = (char*)GC_REALLOC(*stream->buffer, (*stream->size += MAX(MAX(16, *stream->size/2), size + 1)));
    memcpy(&(*stream->buffer)[stream->position], buf, size);
    stream->position += size;
    (*stream->buffer)[stream->position] = '\0';
    return size;
}

static FILE *gc_memory_stream(char **buf, size_t *size) {
    gc_stream_t *stream = (gc_stream_t*)GC_MALLOC(sizeof(gc_stream_t));
    stream->size = size;
    stream->buffer = buf;
    *stream->size = 16;
    *stream->buffer = (char*)GC_MALLOC_ATOMIC(*stream->size);
    (*stream->buffer)[0] = '\0';
    stream->position = 0;
    cookie_io_functions_t functions = {.write = _gc_stream_write};
    return fopencookie(stream, "w", functions);
}

#define String(...) ({ \
    char *_buf = NULL; \
    size_t _size = 0; \
    FILE *_stream = gc_memory_stream(&_buf, &_size); \
    assert(_stream); \
    _fprint(_stream, __VA_ARGS__); \
    fflush(_stream); \
    _buf; })
