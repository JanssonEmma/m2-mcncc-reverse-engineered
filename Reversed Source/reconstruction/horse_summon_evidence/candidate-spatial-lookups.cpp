// Closed spatial LOOKUP methods; full old class/data/API in staged header.
// Actual manager/map lifecycle and map loading remain separate unresolved methods.
#include "sectree_manager.h"
SECTREE_MAP* SECTREE_MANAGER::GetMap(long index) {
    std::map<DWORD,SECTREE_MAP*>::iterator it=m_map_pkSectree.find(static_cast<DWORD>(index));
    return it==m_map_pkSectree.end()?0:it->second;
}
LPSECTREE SECTREE_MAP::Find(DWORD package) {
    std::map<DWORD,SECTREE*>::iterator it=find(package);
    return it==end()?0:it->second;
}
LPSECTREE SECTREE_MANAGER::Get(DWORD index,DWORD package) {
    SECTREE_MAP* map=GetMap(static_cast<long>(index));
    return map?map->Find(package):0;
}
LPSECTREE SECTREE_MANAGER::Get(DWORD index,DWORD x,DWORD y) {
    unsigned int package=(static_cast<unsigned int>(x)/6400U&0xffffU)|
                         (static_cast<unsigned int>(y)/6400U<<16);
    return Get(index,static_cast<DWORD>(package));
}
BYTE SECTREE_MANAGER::GetEmpireFromMapIndex(long index) {
    unsigned int value=static_cast<unsigned int>(index);
    if(value-1U<=19U) return 1;
    if(value-21U<=19U) return 2;
    if(value-41U<=19U) return 3;
    if(value-184U<=1U) return 1;
    if(value-186U<=1U) return 2;
    if(value-188U<=1U) return 3;
    return 0;
}
