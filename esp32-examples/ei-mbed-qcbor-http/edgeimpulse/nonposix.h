#ifndef _NONPOSIX_H_
#define _NONPOSIX_H_

#include <stdint.h>

// Override the stream
#define EI_SENSOR_AQ_STREAM memory_stream_t

// Holder for the stream
typedef struct {
    uint8_t buffer[2048];
    size_t length;
    size_t current_position;
} memory_stream_t;

// fwrite function for the stream
size_t ms_fwrite(const void *ptr, size_t size, size_t count, memory_stream_t *stream) {
    memcpy(stream->buffer + stream->current_position, ptr, size * count);
    stream->current_position += size * count;

    if (stream->current_position > stream->length) {
        stream->length = stream->current_position;
    }

    return count;
}

// set current position in the stream
int ms_fseek(memory_stream_t *stream, long int offset, int origin) {
    if (origin == 0 /* SEEK_SET */) {
        stream->current_position = offset;
    }
    else if (origin == 1 /* SEEK_CUR */) {
        stream->current_position += offset;
    }
    else if (origin == 2 /* SEEK_END */) {
        stream->current_position = stream->length + offset;
    }
    // @todo: do a boundary check here
    return 0;
}

#endif // _NONPOSIX_H_