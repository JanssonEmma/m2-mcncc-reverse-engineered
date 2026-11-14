#ifndef __INC_METIN_II_PLAYERHB_H__
#define __INC_METIN_II_PLAYERHB_H__

#include <cstdint>

class PlayerHB : public singleton<PlayerHB>
{
public:
    PlayerHB();
    virtual ~PlayerHB();

    bool Initialize();
    void Put(DWORD id);

private:
    bool Query(DWORD id);

    std::map<DWORD, std::uint32_t> m_map_data;
    std::string m_stCreateTableQuery;
    std::string m_stTableName;
    int m_iExpireTime;
};

#endif
