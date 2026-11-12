#ifndef MCNCC_GAME_LZO_MANAGER_H
#define MCNCC_GAME_LZO_MANAGER_H

#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <cstddef>

class LZOManager : public singleton<LZOManager>
{
public:
    LZOManager();
    virtual ~LZOManager();

    bool Compress(BYTE* source, unsigned int sourceSize, BYTE* destination, unsigned int* destinationSize);
    bool Decompress(BYTE* source, unsigned int sourceSize, BYTE* destination, unsigned int* destinationSize);
    std::size_t GetMaxCompressedSize(std::size_t original);
private:
    BYTE* m_workmem;
};

#endif
