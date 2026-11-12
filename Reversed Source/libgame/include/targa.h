#ifndef MCNCC_LIBGAME_TARGA_H
#define MCNCC_LIBGAME_TARGA_H

#include <cstdint>

#pragma pack(push, 1)
struct TGA_HEADER
{
    std::uint8_t idLen, palType, imgType;
    std::uint16_t colorBegin, colorCount;
    std::uint8_t palEntrySize;
    std::uint16_t left, top, width, height;
    std::uint8_t colorBits, desc;
};
#pragma pack(pop)

class CTargaImage
{
public:
    CTargaImage();
    ~CTargaImage();

    void Create(int width, int height);
    char* GetBasePointer(int line = 0);
    bool Save(const char* filename);

private:
    CTargaImage(const CTargaImage&) = delete;
    CTargaImage& operator=(const CTargaImage&) = delete;

    TGA_HEADER m_header;
    char* m_pbuf;
    int m_x, m_y;
};

#endif
