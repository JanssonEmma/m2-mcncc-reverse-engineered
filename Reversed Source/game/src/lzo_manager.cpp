#include "lzo_manager.h"
#include <cstdio>
#include <cstdlib>

#include "minilzo.h"

LZOManager::LZOManager()
{
    if (lzo_init() != 0) {
        std::fprintf(stderr, "lzo_init() failed\n");
        std::abort();
    }
    m_workmem = static_cast<BYTE*>(std::malloc(LZO1X_1_MEM_COMPRESS));
}

LZOManager::~LZOManager()
{
    std::free(m_workmem);
    m_workmem = 0;
}

bool LZOManager::Compress(BYTE* source, unsigned int sourceSize, BYTE* destination,
                          unsigned int* destinationSize)
{
    return lzo1x_1_compress(source, sourceSize, destination, destinationSize, m_workmem) == 0;
}

bool LZOManager::Decompress(BYTE* source, unsigned int sourceSize, BYTE* destination,
                            unsigned int* destinationSize)
{
    return lzo1x_decompress(source, sourceSize, destination, destinationSize, 0) == 0;
}

std::size_t LZOManager::GetMaxCompressedSize(std::size_t original)
{
    return (original >> 6) + 19 + original;
}
