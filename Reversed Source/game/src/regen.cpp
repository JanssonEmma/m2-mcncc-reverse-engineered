#include "regen.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "dungeon.h"
#include "mob_manager.h"
#include "../../libthecore/include/utils.h"
#include <algorithm>
#include "../../libthecore/include/log.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

regen* regen_list = nullptr;
regen_exception* regen_exception_list = nullptr;

bool get_word(FILE* file, char* word)
{
    int size=0;
    bool quoted=false;
    int ch;
    while ((ch=std::fgetc(file))!=EOF) {
        bool space=ch==' ' || ch=='\t' || ch=='\n';
        if (!size) {
            if (ch=='"') {
                quoted=true;
                continue;
            }
            if (space) continue;
        }
        if (quoted ? ch=='"' : space) {
            word[size]=0;
            return true;
        }
        word[size++]=static_cast<char>(ch);
        if (size==2 && word[0]=='/' && word[1]=='/') {
            word[size]=0;
            return true;
        }
    }
    word[size]=0;
    return size!=0;
}

void next_line(FILE* file)
{
    int ch;
    do ch=std::fgetc(file); while (ch!=EOF && ch!='\n');
}

namespace {
int RegenCoordinate(int center, int radius, bool upper)
{
    unsigned int bits=static_cast<unsigned int>(center);
    if (upper) bits+=static_cast<unsigned int>(radius);
    else bits-=static_cast<unsigned int>(radius);
    bits*=100u;
    int result;
    std::memcpy(&result,&bits,sizeof(result));
    return result;
}

std::uint32_t RegenTime(const char* token)
{
    std::uint32_t amount=0;
    std::uint32_t time=0;
    for (;*token;++token) {
        unsigned int scale;
        switch (*token) {
            case 'h': scale=3600; break;
            case 'm': scale=60; break;
            case 's': scale=1; break;
            default:
                if (*token>='0' && *token<='9') amount=amount*10+(*token-'0');
                continue;
        }
        time+=amount*scale;
        amount=0;
    }
    return time;
}

std::int32_t RegenEventDelay(std::uint32_t seconds, std::uint32_t jitter = 0)
{
    return static_cast<std::int32_t>((seconds + jitter) * static_cast<std::uint32_t>(passes_per_sec));
}
}

bool read_line(FILE* file, regen* record)
{
    char token[256];
    int field=0;
    while (get_word(file,token)) {
        if (token[0]=='/' && token[1]=='/') {
            next_line(file);
            continue;
        }
        switch (field++) {
            case 0:
                switch (token[0]) {
                    case 'm': record->type=0; break;
                    case 'g':
                        record->type=1;
                        if (token[1]=='a') record->is_aggressive=true;
                        break;
                    case 'e': record->type=2; break;
                    case 'r': record->type=3; break;
                    case 's': record->type=4; break;
                    default:
                        _sys_err("read_line",0x7e,"read_line: unknown regen type %c",token[0]);
                        std::exit(1);
                }
                break;
            case 1: record->sx=std::atoi(token); break;
            case 2: record->sy=std::atoi(token); break;
            case 3: {
                int radius=std::atoi(token);
                record->ex=RegenCoordinate(record->sx,radius,true);
                record->sx=RegenCoordinate(record->sx,radius,false);
                break;
            }
            case 4: {
                int radius=std::atoi(token);
                record->ey=RegenCoordinate(record->sy,radius,true);
                record->sy=RegenCoordinate(record->sy,radius,false);
                break;
            }
            case 5:
                record->z_section=std::atoi(token);
                if (record->type==2) return true;
                break;
            case 6: record->direction=std::atoi(token); break;
            case 7: record->time=RegenTime(token); break;
            case 8: record->percent=std::atoi(token); break;
            case 9:
                record->count=0;
                record->max_count=std::atoi(token);
                break;
            case 10:
                record->vnum=std::atoi(token);
                return true;
        }
    }
    return false;
}

namespace {
struct dungeon_regen_event_info {
    regen* record;
    CDungeon* dungeon;
};

void SpawnRegenRecord(regen* record, CDungeon* dungeon, bool once, bool dungeonRules)
{
    if (record->count >= record->max_count)
        return;
    const unsigned attempts = static_cast<unsigned>(record->max_count) - static_cast<unsigned>(record->count);
    for (unsigned i = 0; i < attempts; ++i) {
        CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
        LPCHARACTER character = 0;
        if (record->type == 4) {
            character = characters.SpawnMobRandomPosition(record->vnum, record->lMapIndex);
        } else if (record->sx == record->ex && record->sy == record->ey) {
            const int direction = record->direction ? record->direction - 1
                : number_ex(0, 7, const_cast<char*>("regen.cpp"), dungeonRules ? 0x124 : 0x165);
            character = characters.SpawnMob(record->vnum, record->lMapIndex, record->sx, record->sy,
                                           record->z_section, false, direction * 45, true);
        } else if (record->type == 0) {
            character = characters.SpawnMobRange(record->vnum, record->lMapIndex, record->sx, record->sy,
                                                record->ex, record->ey, true,
                                                !dungeonRules && record->is_aggressive,
                                                !dungeonRules && record->is_aggressive);
        } else if (record->type == 1) {
            if (characters.SpawnGroup(record->vnum, record->lMapIndex, record->sx, record->sy,
                                      record->ex, record->ey, once ? 0 : record, record->is_aggressive, dungeon))
                ++record->count;
        } else if (record->type == 3) {
            if (characters.SpawnGroupGroup(record->vnum, record->lMapIndex, record->sx, record->sy,
                                           record->ex, record->ey, once ? 0 : record, record->is_aggressive, dungeon))
                ++record->count;
        }
        if (character) {
            ++record->count;
            if (dungeonRules)
                character->SetDungeon(dungeon);
            if (!once)
                character->SetRegen(record);
            else if (!dungeonRules)
                character->SetRegen(nullptr);
        }
    }
}
}

void regen_spawn_dungeon(regen* record, CDungeon* dungeon, bool once)
{
    SpawnRegenRecord(record, dungeon, once, true);
}

EVENTFUNC(dungeon_regen_event)
{
    dungeon_regen_event_info* info = static_cast<dungeon_regen_event_info*>(event->info);
    regen* record = info->record;
    if (!record->time)
        record->event = 0;
    regen_spawn_dungeon(record, info->dungeon, false);
    return RegenEventDelay(record->time);
}

namespace {
void PlaceRegenRecord(regen& record, long map, int baseX, int baseY);
}

bool regen_do(const char* filename, long map, int baseX, int baseY, CDungeon* dungeon, bool once)
{
    if (g_bNoRegen || static_cast<unsigned long>(map) - 114 < 4)
        return true;
    FILE* file = std::fopen(filename, "rb");
    if (!file) {
        _sys_err("regen_do", 0x1a5, "SYSTEM: regen_do: %s: file not found", filename);
        return false;
    }

    regen temporary = {};
    while (read_line(file, &temporary)) {
        if (temporary.type != 0 && temporary.type != 1 && temporary.type != 3 && temporary.type != 4)
            continue;
        regen* record = once ? &temporary : new regen(temporary);
        if (dungeon)
            record->is_aggressive = true;
        PlaceRegenRecord(*record, map, baseX, baseY);

        if (record->type == 0 && !CMobManager::instance().Get(static_cast<DWORD>(record->vnum)))
            _sys_err("regen_do", 0x1de, "No mob data by vnum %lu", static_cast<unsigned long>(static_cast<DWORD>(record->vnum)));

        regen_spawn_dungeon(record, dungeon, once);
        if (!once) {
            void* data;
            const DWORD infoId = __AllocEventInfo(sizeof(dungeon_regen_event_info), &data);
            dungeon_regen_event_info* info = static_cast<dungeon_regen_event_info*>(data);
            info->record = record;
            info->dungeon = dungeon;
            char source[] = "regen.cpp";
            const int jitter = number_ex(0, 16, source, 0x1ec);
            record->event = event_create_ex(dungeon_regen_event, infoId,
                                            RegenEventDelay(record->time, jitter), source, 0x1ec);
            if (dungeon)
                dungeon->AddRegen(record);
        }
    }
    std::fclose(file);
    return true;
}

namespace {
void PlaceRegenRecord(regen& record, long map, int baseX, int baseY)
{
    record.count = 0;
    record.lMapIndex = map;
    record.sx = static_cast<std::int32_t>(static_cast<std::uint32_t>(record.sx) + baseX);
    record.ex = static_cast<std::int32_t>(static_cast<std::uint32_t>(record.ex) + baseX);
    record.sy = static_cast<std::int32_t>(static_cast<std::uint32_t>(record.sy) + baseY);
    record.ey = static_cast<std::int32_t>(static_cast<std::uint32_t>(record.ey) + baseY);
    if (record.ex < record.sx)
        std::swap(record.sx, record.ex);
    if (record.ey < record.sy)
        std::swap(record.sy, record.ey);
}

bool IsSpawnRecord(const regen& record)
{
    return record.type == 0 || record.type == 1 || record.type == 3 || record.type == 4;
}

void PushRegenException(const regen& record)
{
    regen_exception* exception = new regen_exception;
    exception->prev = 0;
    exception->next = regen_exception_list;
    exception->sx = record.sx;
    exception->sy = record.sy;
    exception->ex = record.ex;
    exception->ey = record.ey;
    exception->z_section = record.z_section;
    if (regen_exception_list)
        regen_exception_list->prev = exception;
    regen_exception_list = exception;
}
}

void regen_spawn(regen* record)
{
    SpawnRegenRecord(record, 0, false, false);
}

EVENTFUNC(regen_event)
{
    regen* record = *static_cast<regen**>(event->info);
    if (!record->time)
        record->event = 0;
    regen_spawn(record);
    return RegenEventDelay(record->time);
}

bool regen_load(const char* filename, long map, int baseX, int baseY)
{
    if (g_bNoRegen)
        return true;

    FILE* file = std::fopen(filename, "rb");
    if (!file) {
        sys_log(0, "SYSTEM: regen_load: %s: file not found", filename);
        return false;
    }

    for (;;) {
        regen input = {};
        if (!read_line(file, &input))
            break;
        if (input.type == 2) {
            PushRegenException(input);
            continue;
        }
        if (!IsSpawnRecord(input))
            continue;

        CMobManager& mobs = CMobManager::instance();
        if (test_server)
            mobs.IncRegenCount(input.type, input.vnum, input.max_count, input.time);

        regen* record = new regen(input);
        record->next = regen_list;
        if (regen_list)
            regen_list->prev = record;
        regen_list = record;
        PlaceRegenRecord(*record, map, baseX, baseY);

        if (record->type == 0) {
            if (const CMob* mob = mobs.Get(static_cast<DWORD>(record->vnum))) {
                const TMobTable& proto = mob->m_table;
                if (proto.bType == 1 || proto.bType == 3) {
                    const auto localCenter = [](int start, int end, int base) {
                        const auto sum = static_cast<std::int32_t>(
                            static_cast<std::uint32_t>(start) + static_cast<std::uint32_t>(end));
                        return static_cast<std::int32_t>(static_cast<std::uint32_t>(sum / 2) - base);
                    };
                    SECTREE_MANAGER::instance().InsertNPCPosition(map, proto.bType, proto.szLocaleName,
                        localCenter(record->sx, record->ex, baseX),
                        localCenter(record->sy, record->ey, baseY));
                }
            } else {
                _sys_err("regen_load", 0x29a, "No mob data by vnum %lu", static_cast<DWORD>(record->vnum));
            }
        }

        if (record->time) {
            regen_spawn(record);
            void* data;
            const DWORD id = __AllocEventInfo(sizeof(regen*), &data);
            *static_cast<regen**>(data) = record;
            char source[] = "regen.cpp";
            const int jitter = number_ex(0, 16, source, 0x2b2);
            record->event = event_create_ex(regen_event, id, RegenEventDelay(record->time, jitter), source, 0x2b2);
        }
    }

    std::fclose(file);
    return true;
}

bool regen_load_in_file(const char* filename, long map, int baseX, int baseY)
{
    if (g_bNoRegen)
        return true;

    FILE* file = std::fopen(filename, "rb");
    if (!file) {
        _sys_err("regen_load_in_file", 0x205, "SYSTEM: regen_do: %s: file not found", filename);
        return false;
    }

    for (;;) {
        regen record = {};
        if (!read_line(file, &record))
            break;
        if (!IsSpawnRecord(record))
            continue;
        record.is_aggressive = true;
        PlaceRegenRecord(record, map, baseX, baseY);
        if (record.type == 0 && !CMobManager::instance().Get(static_cast<DWORD>(record.vnum)))
            _sys_err("regen_load_in_file", 0x238, "No mob data by vnum %lu", static_cast<DWORD>(record.vnum));
        // Capture the ordinary spawn origin without retaining the stack record.
        SpawnRegenRecord(&record, 0, true, false);
    }

    std::fclose(file);
    return true;
}

bool is_regen_exception(long x, long y)
{
    for (regen_exception* it = regen_exception_list; it; it = it->next) {
        if (it->sx <= x && x <= it->ex && it->sy <= y && y <= it->ey)
            return true;
    }
    return false;
}

void regen_reset(int x, int y)
{
    for (regen* record = regen_list; record; record = record->next) {
        if (record->event && ((!x && !y) ||
            (record->sx <= x && x <= record->ex && record->sy <= y && y <= record->ey)))
            event_reset_time(record->event, 1);
    }
}

void regen_free()
{
    while (regen_list) {
        regen* record = regen_list;
        regen_list = record->next;
        event_cancel(&record->event);
        CHARACTER::InvalidateRegen(record);
        delete record;
    }
    while (regen_exception_list) {
        regen_exception* exception = regen_exception_list;
        regen_exception_list = exception->next;
        delete exception;
    }
}
