#include "../include/targa.h"

#include <cstddef>
#include <cstdio>
#include <limits>
#include <new>

static_assert(sizeof(TGA_HEADER) == 18, "Targa wire header size");

// 081f5aa0/081f5ab0
CTargaImage::CTargaImage() : m_header{}, m_pbuf(nullptr), m_x(0), m_y(0) {}

// 081f5ac0/081f5ae0
CTargaImage::~CTargaImage()
{
    delete[] m_pbuf;
}

// 081f5b20
void CTargaImage::Create(int width, int height)
{
    delete[] m_pbuf;
    m_pbuf = nullptr;
    if (width < 0 || height < 0 || width > 65535 || height > 65535)
        return;

    const std::uint64_t bytes = static_cast<std::uint64_t>(width) * height * 4;
    if (bytes > std::numeric_limits<std::size_t>::max())
        return;
    m_pbuf = new (std::nothrow) char[static_cast<std::size_t>(bytes)]{};
    if (!m_pbuf)
        return;

    m_x = width;
    m_y = height;
    m_header = {};
    m_header.imgType = 2;
    m_header.width = static_cast<std::uint16_t>(width);
    m_header.height = static_cast<std::uint16_t>(height);
    m_header.colorBits = 32;
    m_header.desc = 0x20;
}

// 081f5b00
char* CTargaImage::GetBasePointer(int line)
{
    return m_pbuf ? m_pbuf + static_cast<std::size_t>(m_x) * line * 4 : nullptr;
}

// 081f5ba0
bool CTargaImage::Save(const char* filename)
{
    if (!m_pbuf || !filename)
        return false;
    std::FILE* output = std::fopen(filename, "wb");
    if (!output)
        return false;
    const std::size_t bytes = static_cast<std::size_t>(m_x) * m_y * 4;
    const bool written = std::fwrite(&m_header, 1, sizeof(m_header), output) == sizeof(m_header) &&
                         std::fwrite(m_pbuf, 1, bytes, output) == bytes;
    return std::fclose(output) == 0 && written;
}
