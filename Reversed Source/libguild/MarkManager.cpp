#include "GuildLib.h"
#include "../game/src/crc32.h"
#include "../game/src/desc.h"
#include "../game/src/packet.h"
#include "../libthecore/include/log.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

namespace Guild {
class CMarkManager : public IMarkManager
{
public:
    CMarkManager();
    virtual ~CMarkManager();
    virtual bool LoadMarkData(const char* fileName, const std::vector<int>& guildIDs);
    virtual bool SaveMarkData(const char* fileName);
    virtual unsigned long GetMarkID(unsigned long guildID);
    virtual bool IsMarkIndicesCRC(unsigned long crc);
    virtual const SCompressedMarkIndices* GetCompressedMarkIndices();
    virtual bool LoadSymbol(const char* fileName);
    virtual void SaveSymbol(const char* fileName);
    virtual void UploadSymbol(unsigned long guildID, int size, const unsigned char* data);
    virtual void SendGuildSymbol(DESC* desc, unsigned long guildID, unsigned long crc, unsigned long size);
    virtual bool UploadMark(unsigned long guildID, const SMark& mark);
    virtual bool UploadBlock(unsigned long blockID, const SBlock& block);
    virtual void GetBlockCRCTable(SBlockCRCTable* table);
    virtual void GetDiffBlockIDTable(const SBlockCRCTable& table, SDiffBlockIDTable* diff);
    virtual const SCompressedBlock* GetCompressedBlockPtr(unsigned long blockID);

private:
    SCompressedBlock& __GetCompressBlockRef(unsigned int x, unsigned int y);
    IImage& __GetImageRef();
    void __SetCompressBlock(unsigned int x, unsigned int y, const SCompressedBlock& block);
    void __BuildCompressedBlocks();
    IImage* __NewImage();
    void __DeleteImage(IImage* image);
    unsigned long __BlockPosToBlockID(unsigned int x, unsigned int y);
    bool __LoadMarkIndices(const char* fileName);
    bool __SaveMarkIndices(const char* fileName);
    void __CompressMarkIndices();
    void __BuildFreeMarkList();
    bool __IsUsedMark(unsigned long markID);
    unsigned long __AllocMarkID(unsigned long guildID);

    struct TGuildSymbol {
        unsigned long crc;
        std::vector<unsigned char> raw;
    };

    SCompressedBlock m_aakCompressBlock[8][8];
    IImage* m_pkImage;
    std::map<unsigned long, TGuildSymbol> m_mapSymbol;
    std::map<unsigned long, unsigned long> m_map_usedMark;
    std::vector<int> m_vec_freeMark;
    SCompressedMarkIndices m_compressedMarkIndices;
};

IMarkManager::~IMarkManager() {}

CMarkManager::CMarkManager()
{
    m_pkImage = __NewImage();
}

CMarkManager::~CMarkManager()
{
    __DeleteImage(m_pkImage);
}

// 0807ab10
void DeleteMarkManager(IMarkManager* manager)
{
    delete manager;
}

IMarkManager* NewMarkManager()
{
    return new CMarkManager;
}

IImage* CMarkManager::__NewImage()
{
    return IUtil::GetSingleton().NewImage();
}

void CMarkManager::__DeleteImage(IImage* image)
{
    IUtil::GetSingleton().DeleteImage(image);
}

bool CMarkManager::LoadMarkData(const char* fileName, const std::vector<int>& guildIDs)
{
    sys_log(1, "MarkManager::LoadMarkData %s", fileName);
    if (!__GetImageRef().Load(fileName))
        return false;
    for (std::vector<int>::const_iterator i = guildIDs.begin(); i != guildIDs.end(); ++i) {
        if (*i < 1344) {
            const auto guildID = static_cast<std::uint32_t>(*i);
            m_map_usedMark.insert(std::make_pair(guildID, guildID));
        }
    }
    if (!__LoadMarkIndices("guild_mark.idx"))
        return false;
    __BuildCompressedBlocks();
    return true;
}

bool CMarkManager::SaveMarkData(const char* fileName)
{
    return __GetImageRef().Save(fileName);
}

IImage& CMarkManager::__GetImageRef()
{
    assert(m_pkImage);
    return *m_pkImage;
}

void CMarkManager::__BuildCompressedBlocks()
{
    IImage& image = __GetImageRef();
    for (unsigned int row = 0; row < 8; ++row) {
        for (unsigned int col = 0; col < 8; ++col) {
            SBlock block;
            image.GetData(col << 6, row << 6, 64, 64, &block);
            __GetCompressBlockRef(row, col).Compress(block);
        }
    }
}

bool CMarkManager::UploadBlock(unsigned long id, const SBlock& block)
{
    const unsigned int row = id >> 3;
    if (row >= 8)
        return false;
    const unsigned int col = id & 7;
    __GetImageRef().PutData(col << 6, row << 6, 64, 64, &block);
    __GetCompressBlockRef(row, col).Compress(block);
    return true;
}

void CMarkManager::SendGuildSymbol(DESC* desc, unsigned long guildID, unsigned long crc, unsigned long size)
{
    std::map<unsigned long, TGuildSymbol>::iterator it = m_mapSymbol.find(guildID);
    if (it == m_mapSymbol.end())
        return;
    TGuildSymbol& symbol = it->second;
    sys_log(0, "SendGuildSymbol GID %lu CRC %lu Size %d on server CRC %lu Size %lu",
            guildID, crc, static_cast<int>(size), symbol.crc, static_cast<unsigned long>(symbol.raw.size()));
    if (symbol.raw.size() == size && symbol.crc == crc)
        return;

    TPacketGCGuildSymbolData packet;
    packet.header = 133;
    packet.size = static_cast<WORD>(symbol.raw.size() + sizeof(packet));
    packet.guild_id = static_cast<std::uint32_t>(guildID);
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->Packet(symbol.raw.data(), static_cast<int>(symbol.raw.size()));
    const unsigned char* bytes = symbol.raw.data();
    sys_log(0, "SendGuildSymbolHead %02X%02X%02X%02X Size %d",
            static_cast<unsigned int>(bytes[0]), static_cast<unsigned int>(bytes[1]),
            static_cast<unsigned int>(bytes[2]), static_cast<unsigned int>(bytes[3]), static_cast<int>(symbol.raw.size()));
}

bool CMarkManager::LoadSymbol(const char* fileName)
{
    std::FILE* file = std::fopen(fileName, "rb");
    if (file) {
        std::uint32_t count;
        std::fread(&count, sizeof(count), 1, file);
        for (unsigned long i = 0; i < count; ++i) {
            std::uint32_t guildID, size;
            std::fread(&guildID, sizeof(guildID), 1, file);
            std::fread(&size, sizeof(size), 1, file);
            TGuildSymbol symbol;
            symbol.raw.resize(size);
            std::fread(symbol.raw.data(), 1, size, file);
            symbol.crc = get_crc32(reinterpret_cast<const char*>(symbol.raw.data()), size);
            m_mapSymbol.insert(std::make_pair(guildID, symbol));
        }
        std::fclose(file);
    }
    return true;
}

void CMarkManager::SaveSymbol(const char* fileName)
{
    std::FILE* file = std::fopen(fileName, "wb");
    if (!file) {
        _sys_err("SaveSymbol", 0xc3, "Cannot open Symbol file (name: %s)", fileName);
        return;
    }
    const auto count = static_cast<std::uint32_t>(m_mapSymbol.size());
    std::fwrite(&count, sizeof(count), 1, file);
    for (std::map<unsigned long, TGuildSymbol>::iterator it = m_mapSymbol.begin(); it != m_mapSymbol.end(); ++it) {
        const auto guildID = static_cast<std::uint32_t>(it->first);
        const auto size = static_cast<std::uint32_t>(it->second.raw.size());
        std::fwrite(&guildID, sizeof(guildID), 1, file);
        std::fwrite(&size, sizeof(size), 1, file);
        std::fwrite(it->second.raw.data(), 1, size, file);
    }
    std::fclose(file);
}

void CMarkManager::UploadSymbol(unsigned long guildID, int size, const unsigned char* data)
{
    sys_log(0, "GuildSymbolUpload GID %lu Size %d", guildID, size);
    std::map<unsigned long, TGuildSymbol>::iterator it = m_mapSymbol.find(guildID);
    if (it == m_mapSymbol.end()) {
        TGuildSymbol symbol;
        it = m_mapSymbol.insert(std::make_pair(guildID, symbol)).first;
    }
    TGuildSymbol& symbol = it->second;
    symbol.raw.clear();
    if (size > 0) {
        symbol.raw.reserve(size);
        for (int i = 0; i < size; ++i)
            symbol.raw.push_back(data[i]);
        symbol.crc = get_crc32(reinterpret_cast<const char*>(data), size);
    }
}

unsigned long CMarkManager::GetMarkID(unsigned long guildID)
{
    if (guildID < 1344)
        return guildID;
    std::map<unsigned long, unsigned long>::iterator it = m_map_usedMark.find(guildID);
    if (it != m_map_usedMark.end())
        return it->second;
    const unsigned long markID = __AllocMarkID(guildID);
    if (markID) {
        m_map_usedMark.insert(std::make_pair(guildID, markID));
        sys_log(0, "MarkManager: GuildIDToMarkID(guild_id(%d), NEW_MARK_ID(%d))",
                static_cast<int>(guildID), static_cast<int>(markID));
        __SaveMarkIndices("guild_mark.idx");
    }
    return markID;
}

unsigned long CMarkManager::__AllocMarkID(unsigned long guildID)
{
    if (guildID < 1344)
        return guildID;
    if (m_vec_freeMark.empty()) {
        _sys_err("__AllocMarkID", 0x108, "MarkManager: __AllocMarkID(guild_id=%d) is EMPTY", static_cast<int>(guildID));
        return 0;
    }
    const int markID = m_vec_freeMark.back();
    m_vec_freeMark.pop_back();
    return markID;
}

bool CMarkManager::__LoadMarkIndices(const char* fileName)
{
    std::FILE* file = std::fopen(fileName, "r");
    if (file) {
        char line[256];
        unsigned long guildID, markID;
        while (std::fgets(line, 255, file)) {
            std::sscanf(line, "%lu %lu", &guildID, &markID);
            line[0] = '\0';
            m_map_usedMark.insert(std::make_pair(static_cast<std::uint32_t>(guildID),
                                                 static_cast<std::uint32_t>(markID)));
            sys_log(0, "MarkManager: InsertGuildMark(guild_id=%d, mark_id=%d)",
                    static_cast<int>(guildID), static_cast<int>(markID));
        }
        std::fclose(file);
        sys_log(0, "MarkManager::LoadMarkIndices(%s) : count(%d)", fileName, static_cast<int>(m_map_usedMark.size()));
    }
    __BuildFreeMarkList();
    __CompressMarkIndices();
    return true;
}

bool CMarkManager::__IsUsedMark(unsigned long markID)
{
    for (std::map<unsigned long, unsigned long>::iterator it = m_map_usedMark.begin(); it != m_map_usedMark.end(); ++it) {
        if (it->second == markID)
            return true;
    }
    return false;
}

void CMarkManager::__BuildFreeMarkList()
{
    m_vec_freeMark.clear();
    m_vec_freeMark.reserve(1344);
    for (int id = 1; id < 1344; ++id) {
        if (!__IsUsedMark(id)) {
            m_vec_freeMark.push_back(id);
            sys_log(0, "MarkManager: free_mark_id(%d)", id);
        }
    }
    sys_log(0, "MarkManager::__BuildFreeMarkList() : free(%d)", static_cast<int>(m_vec_freeMark.size()));
}

void CMarkManager::__CompressMarkIndices()
{
    struct SMarkIndex {
        unsigned short guild_id;
        unsigned short mark_id;
    };
    SMarkIndex indices[1344] = {};
    unsigned int index = 0;
    for (std::map<unsigned long, unsigned long>::iterator it = m_map_usedMark.begin();
         it != m_map_usedMark.end() && index < 1344; ++it, ++index) {
        indices[index].guild_id = static_cast<unsigned short>(it->first);
        indices[index].mark_id = static_cast<unsigned short>(it->second);
    }

    IUtil& util = IUtil::GetSingleton();
    m_compressedMarkIndices.crc = util.CalculateCRC32(indices, 0x1500);
    unsigned int size = 0x1500;
    util.Compress(indices, 0x1500, &m_compressedMarkIndices, size);
    m_compressedMarkIndices.size = size;
    sys_log(0, "MarkManager::__CompressMarkIndices() : size(%d) crc(%x)",
            static_cast<int>(m_compressedMarkIndices.size), static_cast<unsigned int>(m_compressedMarkIndices.crc));
}

bool CMarkManager::__SaveMarkIndices(const char* fileName)
{
    char tempName[256], command[256];
    std::sprintf(tempName, "%s.tmp", fileName);
    std::FILE* file = std::fopen(tempName, "w");
    if (!file) {
        _sys_err("__SaveMarkIndices", 0x177, "MarkManager::SaveMarkIndices(%s) FAILED", fileName);
        return false;
    }
    for (std::map<unsigned long, unsigned long>::iterator it = m_map_usedMark.begin(); it != m_map_usedMark.end(); ++it)
        std::fprintf(file, "%d %d\n", static_cast<int>(it->first), static_cast<int>(it->second));
    std::fclose(file);
    std::sprintf(command, "mv %s %s", tempName, fileName);
    std::system(command);
    sys_log(0, "MarkManager::SaveMarkIndices(%s) : count(%d)", fileName, static_cast<int>(m_map_usedMark.size()));
    __CompressMarkIndices();
    return true;
}

bool CMarkManager::UploadMark(unsigned long markID, const SMark& mark)
{
    const unsigned int markRow = markID >> 5;
    if (markRow >= 42) {
        _sys_err("UploadMark", 0x193, "MarkManager::UploadMark - MARK_ROW_IS_OVER");
        return false;
    }

    const unsigned int y = markRow * 12;
    const unsigned int x = (markID & 31) << 4;
    IImage& image = __GetImageRef();
    image.PutData(x, y, 16, 12, &mark);
    const unsigned int blockCol = (markID & 31) >> 2;
    sys_log(0, "MarkManager::UploadMark - PutData(%d, %d)", x, y);
    const unsigned int firstRow = y >> 6;
    SCompressedBlock& first = __GetCompressBlockRef(firstRow, blockCol);
    if (firstRow >= 8) {
        _sys_err("UploadMark", 0x1b9, "MarkManager::UploadMark - BLOCK_ROW_IS_OVER");
        return false;
    }

    SBlock block;
    const unsigned int blockX = blockCol << 6;
    image.GetData(blockX, firstRow << 6, 64, 64, &block);
    first.Compress(block);
    sys_log(0, "MarkManager::UploadMark - CompressBlock(%d/%d, %d/%d)", blockX, 64, firstRow << 6, 64);
    const unsigned int lastRow = (y + 12) >> 6;
    if (lastRow != firstRow) {
        SCompressedBlock& last = __GetCompressBlockRef(lastRow, blockCol);
        if (lastRow >= 8) {
            _sys_err("UploadMark", 0x1d4, "MarkManager::UploadMark - BLOCK_ROW_IS_OVER");
            return false;
        }
        image.GetData(blockX, lastRow << 6, 64, 64, &block);
        last.Compress(block);
        sys_log(0, "MarkManager::UploadMark - CompressBlockRest(%d/%d, %d/%d)", blockX, 64, lastRow << 6, 64);
    }
    return true;
}

const IMarkManager::SCompressedMarkIndices* CMarkManager::GetCompressedMarkIndices()
{
    return &m_compressedMarkIndices;
}

bool CMarkManager::IsMarkIndicesCRC(unsigned long crc)
{
    return m_compressedMarkIndices.crc == crc;
}

void CMarkManager::GetBlockCRCTable(SBlockCRCTable* table)
{
    for (unsigned int row = 0; row < 8; ++row) {
        for (unsigned int col = 0; col < 8; ++col)
            table->SetBlockCRC(row, col, __GetCompressBlockRef(row, col).GetCRC());
    }
}

void CMarkManager::GetDiffBlockIDTable(const SBlockCRCTable& table, SDiffBlockIDTable* diff)
{
    diff->Clear();
    for (unsigned int row = 0; row < 8; ++row) {
        for (unsigned int col = 0; col < 8; ++col) {
            if (__GetCompressBlockRef(row, col).IsDiff(table.GetBlockCRC(row, col)))
                diff->AppendBlockID(__BlockPosToBlockID(row, col));
        }
    }
}

unsigned long CMarkManager::__BlockPosToBlockID(unsigned int row, unsigned int col)
{
    return row * 8 + col;
}

void CMarkManager::__SetCompressBlock(unsigned int row, unsigned int col, const SCompressedBlock& block)
{
    std::memcpy(&m_aakCompressBlock[row][col], &block, sizeof(block));
}

SCompressedBlock& CMarkManager::__GetCompressBlockRef(unsigned int row, unsigned int col)
{
    return m_aakCompressBlock[row][col];
}

const SCompressedBlock* CMarkManager::GetCompressedBlockPtr(unsigned long id)
{
    const unsigned int row = id >> 3;
    if (row >= 8)
        return 0;
    return &__GetCompressBlockRef(row, id & 7);
}
}
