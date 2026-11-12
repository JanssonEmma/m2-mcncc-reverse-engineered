#ifndef MCNCC_GUILD_LIB_H
#define MCNCC_GUILD_LIB_H

#include <cstdint>
#include <vector>
class DESC;

namespace Guild {
struct IImage;
struct IMarkManager;
typedef unsigned long CRC;

struct IUtil
{
    static IUtil& GetSingleton();
    IUtil();
    virtual ~IUtil();

    virtual IImage* NewImage() = 0;
    virtual void DeleteImage(IImage* image) = 0;
    virtual CRC CalculateCRC32(const void* data, unsigned int size) = 0;
    virtual bool Compress(const void* source, unsigned int sourceSize,
                          void* destination, unsigned int& destinationSize) = 0;
    virtual bool Uncompress(const void* source, unsigned int sourceSize,
                            void* destination, unsigned int& destinationSize) = 0;
protected:
    static IUtil* ms_pkUtilSingleton;
};

struct IImage
{
    virtual ~IImage();
    virtual bool Save(const char* fileName) = 0;
    virtual bool Load(const char* fileName) = 0;
    virtual void PutData(unsigned int x, unsigned int y, unsigned int width,
                         unsigned int height, const void* data) = 0;
    virtual void GetData(unsigned int x, unsigned int y, unsigned int width,
                         unsigned int height, void* data) = 0;
};

struct SMark
{
    typedef std::uint32_t Pixel;
    Pixel m_apxBuf[192];
    void Clear();
};

struct SBlock
{
    typedef unsigned long CRC;
    SMark::Pixel m_apxBuf[4096];
    CRC CalculateCRC() const;
};

struct SCompressedBlock
{
    typedef unsigned char byte;
    typedef std::uint32_t dword;
    byte m_abBuf[16659];
    dword m_dwCompressedSize;
    dword m_crc;
    void Uncompress(SBlock* block) const;
    dword GetCRC() const;
    bool IsDiff(dword crc) const;
    void Compress(const SBlock& block);
};

struct IMarkManager
{
    struct SCompressedMarkIndices {
        char data[5376];
        std::uint32_t size;
        std::uint32_t crc;
    };
    struct SBlockCRCTable {
        std::uint32_t m_aacrcBlock[8][8];
        void SetBlockCRC(unsigned int row, unsigned int col, CRC crc);
        CRC GetBlockCRC(unsigned int row, unsigned int col) const;
    };
    struct SDiffBlockIDTable {
        std::uint32_t m_aBlockID[64];
        std::uint32_t m_dwIDNum;
        void Clear();
        void AppendBlockID(unsigned long id);
    };

    virtual ~IMarkManager();
    virtual bool LoadMarkData(const char* fileName, const std::vector<int>& guildIDs) = 0;
    virtual bool SaveMarkData(const char* fileName) = 0;
    virtual unsigned long GetMarkID(unsigned long guildID) = 0;
    virtual bool IsMarkIndicesCRC(unsigned long crc) = 0;
    virtual const SCompressedMarkIndices* GetCompressedMarkIndices() = 0;
    virtual bool LoadSymbol(const char* fileName) = 0;
    virtual void SaveSymbol(const char* fileName) = 0;
    virtual void UploadSymbol(unsigned long guildID, int size, const unsigned char* data) = 0;
    virtual void SendGuildSymbol(DESC* desc, unsigned long guildID, unsigned long crc, unsigned long size) = 0;
    virtual bool UploadMark(unsigned long guildID, const SMark& mark) = 0;
    virtual bool UploadBlock(unsigned long blockID, const SBlock& block) = 0;
    virtual void GetBlockCRCTable(SBlockCRCTable* table) = 0;
    virtual void GetDiffBlockIDTable(const SBlockCRCTable& table, SDiffBlockIDTable* diff) = 0;
    virtual const SCompressedBlock* GetCompressedBlockPtr(unsigned long blockID) = 0;
};

IImage* NewImage();
void DeleteImage(IImage* image);

IMarkManager* NewMarkManager();
void DeleteMarkManager(IMarkManager* manager);
}
#endif
