#ifndef MCNCC_GAME_MAP_LOCATION_H
#define MCNCC_GAME_MAP_LOCATION_H
#include <cstdint>
#include <map>
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"

class CMapLocation : public singleton<CMapLocation> {
public:
    struct SLocation {
        std::int32_t addr;
        WORD port;
    };
    bool Get(long x, long y, long& index, long& addr, WORD& port);
    bool Get(int index, long& addr, WORD& port);
    void Insert(BYTE index, const char* host, WORD port);

protected:
    std::map<BYTE, SLocation> m_map_address;
};
#endif
