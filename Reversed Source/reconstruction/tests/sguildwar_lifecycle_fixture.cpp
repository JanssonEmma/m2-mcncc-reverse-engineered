// TEST ONLY SGuildWar lifecycle boundary. Not original constructor recovery.
// Original default state was optimized away; tests overwrite EVERY field before lookup.
#include "../../game/src/guild.h"
SGuildWar::SGuildWar(BYTE inputType)
    :war_start_time(0),score(0),state(0),type(inputType),map_index(0) {}
