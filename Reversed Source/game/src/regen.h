#ifndef MCNCC_GAME_REGEN_H
#define MCNCC_GAME_REGEN_H

#include "event.h"
#include <cstdint>
class CDungeon;
struct regen {
    regen* prev;
    regen* next;
    long lMapIndex;
    int type;
    int sx, sy, ex, ey;
    BYTE z_section;
    BYTE direction;
    std::uint32_t time;
    BYTE percent;
    int max_count;
    int count;
    int vnum;
    bool is_aggressive;
    LPEVENT event;
};

struct regen_exception {
    regen_exception* prev;
    regen_exception* next;
    int sx, sy, ex, ey;
    BYTE z_section;
};

bool regen_do(const char* filename, long map, int baseX, int baseY, CDungeon* dungeon, bool once);

bool regen_load(const char* filename, long map, int baseX, int baseY);
bool regen_load_in_file(const char* filename, long map, int baseX, int baseY);
bool is_regen_exception(long x, long y);
void regen_reset(int x, int y);
void regen_free();

#endif
