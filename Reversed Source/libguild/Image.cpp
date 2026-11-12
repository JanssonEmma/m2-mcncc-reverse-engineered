#include "GuildLib.h"
#include "../libthecore/include/memcpy.h"
#include <cassert>
#include <cstdio>
#include <cstring>

namespace Guild {
class CImage : public IImage
{
public:
    CImage();
    virtual ~CImage();
    virtual bool Save(const char* fileName);
    virtual bool Load(const char* fileName);
    virtual void PutData(unsigned int x, unsigned int y, unsigned int width,
                         unsigned int height, const void* data);
    virtual void GetData(unsigned int x, unsigned int y, unsigned int width,
                         unsigned int height, void* data);
private:
    SMark::Pixel m_aapxBuf[512][512];
};

IImage::~IImage() {}
CImage::CImage() {}
CImage::~CImage() {}

IImage* NewImage()
{
    return new CImage;
}

void DeleteImage(IImage* image)
{
    delete image;
}

bool CImage::Save(const char* fileName)
{
    std::FILE* file = std::fopen(fileName, "wb");
    if (!file)
        return false;
    std::fwrite(m_aapxBuf, sizeof(m_aapxBuf), 1, file);
    std::fclose(file);
    return true;
}

bool CImage::Load(const char* fileName)
{
    std::FILE* file = std::fopen(fileName, "rb");
    if (file) {
        std::fread(m_aapxBuf, sizeof(m_aapxBuf), 1, file);
        std::fclose(file);
    } else {
        std::memset(m_aapxBuf, 0, sizeof(m_aapxBuf));
    }
    return true;
}

void CImage::PutData(unsigned int x, unsigned int y, unsigned int width,
                     unsigned int height, const void* data)
{
    assert(width + x <= 512);
    assert(height + y <= 512);
    const unsigned char* source = static_cast<const unsigned char*>(data);
    const unsigned int rowSize = width * 4;
    for (unsigned int row = 0; row < height; ++row) {
        thecore_memcpy(m_aapxBuf[y + row] + x, source, rowSize);
        source += rowSize;
    }
}

void CImage::GetData(unsigned int x, unsigned int y, unsigned int width,
                     unsigned int height, void* data)
{
    assert(width + x <= 512);
    assert(height + y <= 512);
    unsigned char* destination = static_cast<unsigned char*>(data);
    const unsigned int rowSize = width * 4;
    for (unsigned int row = 0; row < height; ++row) {
        thecore_memcpy(destination, m_aapxBuf[y + row] + x, rowSize);
        destination += rowSize;
    }
}
}
