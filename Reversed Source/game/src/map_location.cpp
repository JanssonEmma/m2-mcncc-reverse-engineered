#include "map_location.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include <arpa/inet.h>

CMapLocation g_mapLocations;

bool CMapLocation::Get(long x, long y, long& index, long& addr, WORD& port)
{
    int map=SECTREE_MANAGER::instance().GetMapIndex(x,y);
    index=map;
    return Get(map,addr,port);
}

bool CMapLocation::Get(int index, long& addr, WORD& port)
{
    if (index!=0) {
        std::map<BYTE,SLocation>::iterator it=m_map_address.find(static_cast<BYTE>(index));
        if (it!=m_map_address.end()) {
            addr=it->second.addr;
            port=it->second.port;
            return true;
        }
    }
    sys_log(0,"CMapLocation::Get - Error MapIndex[%d]",index);
    if (index!=0) {
        for (std::map<BYTE,SLocation>::iterator it=m_map_address.begin();it!=m_map_address.end();++it)
            sys_log(0,"Map(%d): Server(%x:%d)",it->first,static_cast<unsigned int>(it->second.addr),it->second.port);
    }
    return false;
}

void CMapLocation::Insert(BYTE index, const char* host, WORD port)
{
    SLocation location;
    location.addr=static_cast<std::int32_t>(inet_addr(host));
    location.port=port;
    m_map_address.insert(std::make_pair(index,location));
    sys_log(0,"MapLocation::Insert : %d %s %d",index,host,port);
}
