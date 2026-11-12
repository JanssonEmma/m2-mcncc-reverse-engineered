#include "sectree_manager.h"

SECTREE_MANAGER::SECTREE_MANAGER()
{
}

SECTREE_MANAGER::~SECTREE_MANAGER()
{
}

SECTREE_MAP* SECTREE_MANAGER::GetMap(long index)
{
    const std::map<DWORD, SECTREE_MAP*>::iterator found =
        m_map_pkSectree.find(static_cast<DWORD>(index));
    return found == m_map_pkSectree.end() ? 0 : found->second;
}

LPSECTREE SECTREE_MAP::Find(DWORD package)
{
    const std::map<DWORD, SECTREE*>::iterator found = find(package);
    return found == end() ? 0 : found->second;
}

LPSECTREE SECTREE_MAP::Find(DWORD x, DWORD y)
{
    const unsigned int package = ((static_cast<unsigned int>(x) / 6400U) & 0xffffU) |
                                 ((static_cast<unsigned int>(y) / 6400U) << 16);
    return Find(static_cast<DWORD>(package));
}

LPSECTREE SECTREE_MANAGER::Get(DWORD index, DWORD package)
{
    SECTREE_MAP* map = GetMap(static_cast<long>(index));
    return map ? map->Find(package) : 0;
}

LPSECTREE SECTREE_MANAGER::Get(DWORD index, DWORD x, DWORD y)
{
    const unsigned int package = ((static_cast<unsigned int>(x) / 6400U) & 0xffffU) |
                                 ((static_cast<unsigned int>(y) / 6400U) << 16);
    return Get(index, static_cast<DWORD>(package));
}

bool SECTREE_MANAGER::GetMapBasePositionByMapIndex(long index, PIXEL_POSITION& position)
{
    if (index > 10000)
        index /= 10000;

    for (std::vector<SMapRegion>::iterator it = m_vec_mapRegion.begin();
         it != m_vec_mapRegion.end(); ++it)
    {
        if (index == it->index)
        {
            position.z = 0;
            position.x = it->sx;
            position.y = it->sy;
            return true;
        }
    }
    return false;
}
