// Reconstructed old buffer allocator and I/O; see buffer-recovery.md.
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/log.h"
#include "../include/memcpy.h"
#include "../include/utils.h"

static LPBUFFER buffer_poll;
static LPBUFFER buffer_poll_big;
static LPBUFFER buffer_nodelete_poll;

LPBUFFER buffer_new(void* destination, int size)
{
    LPBUFFER buffer;
    LPBUFFER* pool;
    if (size < 0)
        return NULL;
    if (destination)
    {
        buffer = buffer_nodelete_poll;
        if (buffer)
            buffer_nodelete_poll = buffer->next;
        else
            buffer = (LPBUFFER)calloc(1, sizeof(BUFFER));
        if (!buffer)
            goto allocation_failure;
        buffer->flag |= 1;
        buffer->mem_size = size;
        buffer->mem_data = (char*)destination;
    }
    else
    {
        pool = size <= 8192 ? &buffer_poll : &buffer_poll_big;
        buffer = *pool;
        if (buffer)
            *pool = buffer->next;
        else
            buffer = (LPBUFFER)calloc(1, sizeof(BUFFER));
        if (!buffer)
            goto allocation_failure;
        if (!buffer->mem_data)
        {
            buffer->mem_size = size;
            buffer->mem_data = (char*)calloc((size_t)size, 1);
            if (!buffer->mem_data)
                goto allocation_failure;
        }
        else
            buffer_realloc(buffer, size);
    }
    buffer_reset(buffer);
    return buffer;
allocation_failure:
    perror("malloc failure");
    abort();
    return NULL; // Unreachable with the real abort implementation.
}

void buffer_delete(LPBUFFER buffer)
{
    buffer_reset(buffer);
    if (buffer->flag & 1)
    {
        buffer->mem_data = NULL;
        buffer->next = buffer_nodelete_poll;
        buffer_nodelete_poll = buffer;
    }
    else if (buffer->mem_size > 131072)
    {
        free(buffer->mem_data);
        free(buffer);
    }
    else if (buffer->mem_size <= 8192)
    {
        buffer->next = buffer_poll;
        buffer_poll = buffer;
    }
    else
    {
        buffer->next = buffer_poll_big;
        buffer_poll_big = buffer;
    }
}

DWORD buffer_size(LPBUFFER buffer) { return (DWORD)buffer->length; }

void buffer_reset(LPBUFFER buffer)
{
    buffer->write_point_pos = 0;
    buffer->length = 0;
    buffer->read_point = buffer->mem_data;
    buffer->write_point = buffer->mem_data;
}

void buffer_write(LPBUFFER buffer, const void* source, int length)
{
    if (buffer->write_point_pos + length >= buffer->mem_size)
        buffer_realloc(buffer, buffer->mem_size + length + MIN(1048576, length * 2));
    thecore_memcpy(buffer->write_point, source, (size_t)length);
    buffer_write_proceed(buffer, length);
}

void buffer_read(LPBUFFER buffer, void* destination, int bytes)
{
    thecore_memcpy(destination, buffer->read_point, (size_t)bytes);
    buffer_read_proceed(buffer, bytes);
}

BYTE buffer_byte(LPBUFFER buffer)
{
    BYTE value = *(const BYTE*)buffer->read_point;
    buffer_read_proceed(buffer, 1);
    return value;
}

WORD buffer_word(LPBUFFER buffer)
{
    WORD value = *(const WORD*)buffer->read_point;
    buffer_read_proceed(buffer, 2);
    return value;
}

DWORD buffer_dword(LPBUFFER buffer)
{
    DWORD value = *(const DWORD*)buffer->read_point;
    buffer_read_proceed(buffer, 4);
    return value;
}

const void* buffer_read_peek(LPBUFFER buffer) { return buffer->read_point; }

void buffer_read_proceed(LPBUFFER buffer, int length)
{
    if (length == 0)
        return;
    if (length < 0)
        _sys_err("buffer_read_proceed", 176,
                 "buffer_proceed: length argument lower than zero (length: %d)", length);
    else if (length > buffer->length)
    {
        _sys_err("buffer_read_proceed", 179,
                 "buffer_proceed: length argument bigger than buffer (length: %d, buffer: %d)",
                 length, buffer->length);
        length = buffer->length;
    }
    if (length < buffer->length)
    {
        if (buffer->read_point + length - buffer->mem_data > buffer->mem_size)
        {
            _sys_err("buffer_read_proceed", 189,
                     "buffer_read_proceed: buffer overflow! length %d read_point %d",
                     length, (int)(buffer->read_point - buffer->mem_data));
            abort();
        }
        buffer->read_point += length;
        buffer->length -= length;
    }
    else
        buffer_reset(buffer);
}

void* buffer_write_peek(LPBUFFER buffer) { return buffer->write_point; }

void buffer_write_proceed(LPBUFFER buffer, int length)
{
    buffer->length += length;
    buffer->write_point += length;
    buffer->write_point_pos += length;
}

int buffer_has_space(LPBUFFER buffer)
{
    return buffer->mem_size - buffer->write_point_pos;
}

void buffer_adjust_size(LPBUFFER buffer, int add_size)
{
    if (buffer->write_point_pos + add_size <= buffer->mem_size)
        return;
    sys_log(0, "buffer_adjust %d current %d/%d", add_size, buffer->length, buffer->mem_size);
    buffer_realloc(buffer, buffer->mem_size + add_size);
}

void buffer_realloc(LPBUFFER buffer, int length)
{
    int delta, read_offset;
    char* replacement;
    assert(length >= 0 && "buffer_realloc: length is lower than zero");
    if (buffer->mem_size >= length)
        return;
    if (buffer->flag & 1)
    {
        _sys_err("buffer_realloc", 240, "this buffer cannot be reallocated.");
        return;
    }
    delta = length - buffer->mem_size;
    if (delta <= 0)
        return;
    sys_log(0, "reallocating buffer to %d, current %d", length, buffer->mem_size);
    replacement = (char*)malloc((size_t)length);
    thecore_memcpy(replacement, buffer->mem_data, (size_t)buffer->mem_size);
    read_offset = (int)(buffer->read_point - buffer->mem_data);
    free(buffer->mem_data);
    buffer->mem_data = replacement;
    buffer->mem_size = length;
    buffer->write_point = replacement + buffer->write_point_pos;
    buffer->read_point = replacement + read_offset;
}
