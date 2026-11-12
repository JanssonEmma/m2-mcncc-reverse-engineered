// Partial old protocol API, inline behavior corroborated in DESC::Packet.
#ifndef MCNCC_GAME_PROTOCOL_H
#define MCNCC_GAME_PROTOCOL_H
#include <assert.h>
#include <cstring>
// Inlined static buffers corroborated by DBPacketHeader0813fb40.
inline const char* encode_byte(char value) { static char bytes[8]; bytes[0]=value; return bytes; }
inline const char* encode_4bytes(int value) { static char bytes[8]; std::memcpy(bytes,&value,4); return bytes; }
#include "../../libthecore/include/buffer.h"
#define packet_encode(buf, data, len) __packet_encode(buf, data, len, __FILE__, __LINE__)
inline bool __packet_encode(LPBUFFER buffer, const void* data, int length,
                            const char* file, int line)
{
    (void)file; (void)line;
    assert(buffer != 0);
    assert(data != 0);
    if (buffer_has_space(buffer) < length)
        return false;
    buffer_write(buffer, data, length);
    return true;
}
#endif
