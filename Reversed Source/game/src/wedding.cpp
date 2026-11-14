#include "desc_client.h"
#include "locale_service.h"
#include "regen.h"
#include "../../common/tables.h"
#include "config.h"
#include "locale.hpp"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cstring>
#include "wedding.h"
#include "char.h"
#include <cstdio>
#include <cstdint>

namespace marriage {
void WeddingMap::IncMember(CHARACTER* character)
{
    m_set_pkChr.insert(character);
    SendLocalEvent(character);
    if (character->GetLevel()<10) character->SetObserverMode(true);
}

void WeddingMap::DecMember(CHARACTER* character)
{
    m_set_pkChr.erase(character);
    if (character->GetLevel()<10) character->SetObserverMode(false);
}

void WeddingMap::SendLocalEvent(CHARACTER* character)
{
    if (m_isDark) character->ChatPacket(5,"DayMode dark");
    if (m_isSnow) character->ChatPacket(5,"xmas_snow 1");
    if (m_isMusic) {
        char command[256];
        const char* text=__BuildCommandPlayMusic(command,255,1,m_stMusicFileName.c_str());
        character->ChatPacket(5, "%s", text);
    }
}

const char* WeddingMap::__BuildCommandPlayMusic(char* command, int length, BYTE enabled, const char* filename)
{
    if (length<1) {
        *command=0;
        return "PlayMusic 0 CommandLengthError";
    }
    std::snprintf(command,length-1,"PlayMusic %d %s",enabled,filename);
    return command;
}
}

namespace marriage {

WeddingMap::WeddingMap(DWORD map, DWORD first, DWORD second)
    : m_dwMapIndex(static_cast<std::uint32_t>(map)), m_pEndEvent(0), m_isDark(false), m_isSnow(false), m_isMusic(false),
      dwPID1(static_cast<std::uint32_t>(first)), dwPID2(static_cast<std::uint32_t>(second))
{
}

WeddingMap::~WeddingMap()
{
    event_cancel(&m_pEndEvent);
}

WeddingManager::WeddingManager() {}
WeddingManager::~WeddingManager() {}

WeddingMap* WeddingManager::Find(DWORD map)
{
    const auto it = m_mapWedding.find(static_cast<std::uint32_t>(map));
    return it==m_mapWedding.end() ? 0 : it->second;
}

void WeddingManager::DestroyWeddingMap(WeddingMap* map)
{
    m_mapWedding.erase(static_cast<std::uint32_t>(map->GetMapIndex()));
    SECTREE_MANAGER::instance().DestroyPrivateMap(static_cast<std::int32_t>(map->GetMapIndex()));
    delete map;
}

struct FNotice
{
    const char* text;
    void operator()(CHARACTER* character) const
    {
        character->ChatPacket(2, "%s", text);
    }
};

struct FExitWeddingMap
{
    void operator()(CHARACTER* character) const
    {
        if (character->GetDesc())
            character->ExitToSavedLocation();
    }
};

void WeddingMap::Notice(const char* text)
{
    std::for_each(m_set_pkChr.begin(), m_set_pkChr.end(), FNotice{text});
}

void WeddingMap::ExitAll()
{
    std::for_each(m_set_pkChr.begin(), m_set_pkChr.end(), FExitWeddingMap{});
}

namespace {
struct WeddingMapEventInfo {
    WeddingMap* map;
    int step;
};

long WeddingEndDelay()
{
    unsigned int bits=static_cast<unsigned int>(passes_per_sec)*15u;
    int delay;
    std::memcpy(&delay,&bits,sizeof(delay));
    return delay;
}
}

long wedding_end_event(LPEVENT event)
{
    WeddingMapEventInfo* info=static_cast<WeddingMapEventInfo*>(event->info);
    const int step = info->step;
    WeddingMap* map = info->map;
    if (!step) {
        info->step=1;
        map->ExitAll();
        return WeddingEndDelay();
    }
    WeddingManager::instance().DestroyWeddingMap(map);
    return 0;
}

void WeddingMap::SetEnded()
{
    if (m_pEndEvent) {
        const auto eventAddress = static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(m_pEndEvent));
        _sys_err("SetEnded", 0x38, "WeddingMap::SetEnded - ALREADY EndEvent(m_pEndEvent=%x)", eventAddress);
        return;
    }
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(WeddingMapEventInfo), &rawInfo);
    auto* info = static_cast<WeddingMapEventInfo*>(rawInfo);
    info->map=this;
    m_pEndEvent=event_create_ex(wedding_end_event,id,WeddingEndDelay(),const_cast<char*>("wedding.cpp"),0x40);
    Notice(locale_find("\260\341\310\245\275\304\300\314\040\301\276\267\341\265\313\264\317\264\331\056",g_iUseLocale));
    Notice(locale_find("\300\332\265\277\300\270\267\316\040\263\252\260\241\260\324\265\313\264\317\264\331\056",g_iUseLocale));
    for (std::set<CHARACTER*>::iterator it=m_set_pkChr.begin();it!=m_set_pkChr.end();++it) {
        CHARACTER* character=*it;
        const auto pid = static_cast<std::uint32_t>(character->GetPlayerID());
        if (pid != dwPID1 &&
            pid != dwPID2 && character->GetLevel() >= 10)
            character->AutoGiveItem(27002,5,-1,true);
    }
}
}

namespace marriage {
bool WeddingManager::End(DWORD map)
{
    WeddingMap* wedding=Find(map);
    if (!wedding) return false;
    wedding->SetEnded();
    return true;
}

bool WeddingManager::IsWeddingMap(DWORD map)
{
    const auto mapIndex = static_cast<unsigned int>(map);
    return mapIndex == 81u || mapIndex / 10000u == 81u;
}
}

namespace marriage {
DWORD WeddingManager::__CreateWeddingMap(DWORD first, DWORD second)
{
    SECTREE_MANAGER& sectors=SECTREE_MANAGER::instance();
    const auto index = static_cast<std::uint32_t>(sectors.CreatePrivateMap(81));
    if (!index) {
        _sys_err("__CreateWeddingMap",0x100,"CreateWeddingMap(pid1=%d, pid2=%d) / CreatePrivateMap(%d) FAILED",
                 static_cast<int>(static_cast<std::int32_t>(first)), static_cast<int>(static_cast<std::int32_t>(second)), 81);
        return 0;
    }
    m_mapWedding.insert(std::make_pair(index,new WeddingMap(index,first,second)));
    SECTREE_MAP* map=sectors.GetMap(index);
    std::string filename;
    filename.reserve(64);
    filename=LocaleService_GetMapPath();
    filename+="/metin2_map_wedding_01/npc.txt";
    const auto baseY = map->m_setting.iBaseY;
    const auto baseX = map->m_setting.iBaseX;
    const bool loaded = regen_do(filename.c_str(), index, baseX, baseY, 0, true);
    const auto diagnosticY = map->m_setting.iBaseY;
    const auto diagnosticX = map->m_setting.iBaseX;
    if (loaded) {
        sys_log(0,"CreateWeddingMap(pid1=%d, pid2=%d) / regen_do(fileName=%s, mapIndex=%d, basePos=(%d, %d)) ok",
                static_cast<int>(static_cast<std::int32_t>(first)), static_cast<int>(static_cast<std::int32_t>(second)), filename.c_str(),static_cast<int>(index),diagnosticX,diagnosticY);
    } else {
        _sys_err("__CreateWeddingMap",0x110,
                 "CreateWeddingMap(pid1=%d, pid2=%d) / regen_do(fileName=%s, mapIndex=%d, basePos=(%d, %d)) FAILED",
                 static_cast<int>(static_cast<std::int32_t>(first)), static_cast<int>(static_cast<std::int32_t>(second)), filename.c_str(),static_cast<int>(index),diagnosticX,diagnosticY);
    }
    return index;
}

void WeddingManager::Request(DWORD first, DWORD second)
{
    if (!map_allow_find(81)) return;
    TPacketWeddingReady packet;
    packet.dwMapIndex=__CreateWeddingMap(first,second);
    if (!packet.dwMapIndex) {
        _sys_err("Request",0x134,"cannot create wedding map for %lu, %lu",first,second);
        return;
    }
    packet.dwPID1=first;
    packet.dwPID2=second;
    db_clientdesc->DBPacket(74,0,&packet,sizeof(packet));
}
}

namespace marriage {
void WeddingMap::ShoutInMap(BYTE, const char* message)
{
    for (auto it = m_set_pkChr.begin(); it != m_set_pkChr.end(); ++it)
        (*it)->ChatPacket(5, "%s", message);
}

bool WeddingMap::IsPlayingMusic()
{
    return m_isMusic;
}

void WeddingMap::SetDark(bool enabled)
{
    if (m_isDark != enabled) {
        m_isDark = enabled;
        ShoutInMap(5, enabled ? "DayMode dark" : "DayMode light");
    }
}

void WeddingMap::SetSnow(bool enabled)
{
    if (m_isSnow != enabled) {
        m_isSnow = enabled;
        ShoutInMap(5, enabled ? "xmas_snow 1" : "xmas_snow 0");
    }
}

void WeddingMap::SetMusic(bool enabled, const char* filename)
{
    if (m_isMusic != enabled) {
        m_isMusic = enabled;
        m_stMusicFileName = filename;
        const bool playing = m_isMusic;
        char command[256];
        ShoutInMap(5, __BuildCommandPlayMusic(command, 255, playing,
            playing ? m_stMusicFileName.c_str() : "default"));
    }
}
}
