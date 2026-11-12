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
        character->ChatPacket(5,text);
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
    : m_dwMapIndex(map), m_pEndEvent(0), m_isDark(false), m_isSnow(false), m_isMusic(false),
      dwPID1(first), dwPID2(second)
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
    std::map<DWORD,WeddingMap*>::iterator it=m_mapWedding.find(map);
    return it==m_mapWedding.end() ? 0 : it->second;
}

void WeddingManager::DestroyWeddingMap(WeddingMap* map)
{
    m_mapWedding.erase(map->GetMapIndex());
    SECTREE_MANAGER::instance().DestroyPrivateMap(map->GetMapIndex());
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
    if (!info->step) {
        info->step=1;
        info->map->ExitAll();
        return WeddingEndDelay();
    }
    WeddingManager::instance().DestroyWeddingMap(info->map);
    return 0;
}

void WeddingMap::SetEnded()
{
    if (m_pEndEvent) {
        _sys_err("SetEnded",0x38,"WeddingMap::SetEnded - ALREADY EndEvent(m_pEndEvent=%x)",m_pEndEvent);
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
        DWORD pid=character->GetPlayerID();
        if (pid!=dwPID1 && pid!=dwPID2 && character->GetLevel()>=10)
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
    return map==81 || map/10000==81;
}
}

namespace marriage {
DWORD WeddingManager::__CreateWeddingMap(DWORD first, DWORD second)
{
    SECTREE_MANAGER& sectors=SECTREE_MANAGER::instance();
    const DWORD index=sectors.CreatePrivateMap(81);
    if (!index) {
        _sys_err("__CreateWeddingMap",0x100,"CreateWeddingMap(pid1=%d, pid2=%d) / CreatePrivateMap(%d) FAILED",
                 first,second,81);
        return 0;
    }
    m_mapWedding.insert(std::make_pair(index,new WeddingMap(index,first,second)));
    SECTREE_MAP* map=sectors.GetMap(index);
    std::string filename;
    filename.reserve(64);
    filename=LocaleService_GetMapPath();
    filename+="/metin2_map_wedding_01/npc.txt";
    if (regen_do(filename.c_str(),index,map->m_setting.iBaseX,map->m_setting.iBaseY,0,true)) {
        sys_log(0,"CreateWeddingMap(pid1=%d, pid2=%d) / regen_do(fileName=%s, mapIndex=%d, basePos=(%d, %d)) ok",
                first,second,filename.c_str(),index,map->m_setting.iBaseX,map->m_setting.iBaseY);
    } else {
        _sys_err("__CreateWeddingMap",0x110,
                 "CreateWeddingMap(pid1=%d, pid2=%d) / regen_do(fileName=%s, mapIndex=%d, basePos=(%d, %d)) FAILED",
                 first,second,filename.c_str(),index,map->m_setting.iBaseX,map->m_setting.iBaseY);
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
    for (CHARACTER* character : m_set_pkChr)
        character->ChatPacket(5, "%s", message);
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
        char command[256];
        ShoutInMap(5, __BuildCommandPlayMusic(command, 255, enabled,
            enabled ? m_stMusicFileName.c_str() : "default"));
    }
}
}
