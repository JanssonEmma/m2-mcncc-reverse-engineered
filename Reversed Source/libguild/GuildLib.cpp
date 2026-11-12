#include "GuildLib.h"
#include <cassert>

namespace Guild {
IUtil* IUtil::ms_pkUtilSingleton = 0;

IUtil& IUtil::GetSingleton()
{
    assert(ms_pkUtilSingleton);
    return *ms_pkUtilSingleton;
}

IUtil::IUtil()
{
    assert(!ms_pkUtilSingleton);
    ms_pkUtilSingleton = this;
}

IUtil::~IUtil()
{
    ms_pkUtilSingleton = 0;
}

void SMark::Clear()
{
    for (unsigned int i = 0; i < 192; ++i)
        m_apxBuf[i] = 0xff000000UL;
}

SBlock::CRC SBlock::CalculateCRC() const
{
    return IUtil::GetSingleton().CalculateCRC32(this, 0x4000);
}

SCompressedBlock::dword SCompressedBlock::GetCRC() const
{
    return m_crc;
}

bool SCompressedBlock::IsDiff(dword crc) const
{
    return m_crc != crc;
}

void SCompressedBlock::Uncompress(SBlock* block) const
{
    unsigned int size = 0x4000;
    IUtil::GetSingleton().Uncompress(this, m_dwCompressedSize, block, size);
}

void SCompressedBlock::Compress(const SBlock& block)
{
    IUtil& util = IUtil::GetSingleton();
    unsigned int size = 0x4113;
    util.Compress(&block, 0x4000, this, size);
    m_dwCompressedSize = size;
    m_crc = block.CalculateCRC();
}

void IMarkManager::SBlockCRCTable::SetBlockCRC(unsigned int row, unsigned int col, CRC crc)
{
    assert(row < 8);
    assert(col < 8);
    m_aacrcBlock[row][col] = crc;
}

CRC IMarkManager::SBlockCRCTable::GetBlockCRC(unsigned int row, unsigned int col) const
{
    assert(row < 8);
    assert(col < 8);
    return m_aacrcBlock[row][col];
}

void IMarkManager::SDiffBlockIDTable::Clear()
{
    m_dwIDNum = 0;
}

void IMarkManager::SDiffBlockIDTable::AppendBlockID(unsigned long id)
{
    assert(m_dwIDNum < 64);
    m_aBlockID[m_dwIDNum++] = id;
}
}
