#ifndef MCNCC_GAME_WEDDING_H
#define MCNCC_GAME_WEDDING_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <cstdint>
#include <map>
#include "event.h"
#include <set>
#include <string>
class CHARACTER;
namespace marriage {
class WeddingMap {
public:
    WeddingMap(DWORD map, DWORD first, DWORD second);
    ~WeddingMap();
    void IncMember(CHARACTER* character);
    void DecMember(CHARACTER* character);
    void SendLocalEvent(CHARACTER* character);
    DWORD GetMapIndex() { return m_dwMapIndex; }
    void Notice(const char* text);
    void ShoutInMap(BYTE type, const char* message);
    bool IsPlayingMusic();
    void SetDark(bool enabled);
    void SetSnow(bool enabled);
    void SetMusic(bool enabled, const char* filename);
    void ExitAll();
    void SetEnded();
private:
    const char* __BuildCommandPlayMusic(char* command, int length, BYTE enabled, const char* filename);

    std::uint32_t m_dwMapIndex;
    LPEVENT m_pEndEvent;
    std::set<CHARACTER*> m_set_pkChr;
    bool m_isDark, m_isSnow, m_isMusic;
    std::uint32_t dwPID1, dwPID2;
    std::string m_stMusicFileName;
};
class WeddingManager : public singleton<WeddingManager> {
public:
    WeddingManager();
    virtual ~WeddingManager();
    WeddingMap* Find(DWORD map);
    bool End(DWORD map);
    bool IsWeddingMap(DWORD map);
    void Request(DWORD first, DWORD second);
    void DestroyWeddingMap(WeddingMap* map);
private:
    DWORD __CreateWeddingMap(DWORD first, DWORD second);
    std::map<std::uint32_t, WeddingMap*> m_mapWedding;
};
}
#endif
