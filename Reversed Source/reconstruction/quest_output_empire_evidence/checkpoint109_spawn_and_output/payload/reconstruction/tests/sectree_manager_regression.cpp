#include <map>
#include <string>
#include <vector>
#include "sectree.h"
#include "../../common/singleton.h"
#define private public
#include "sectree_manager.h"
#undef private
#include <cassert>
#include <climits>

static void AddRegion(SECTREE_MANAGER& manager, int index, int x, int y)
{
    SMapRegion region = SMapRegion();
    region.index = index;
    region.sx = x;
    region.sy = y;
    manager.m_vec_mapRegion.push_back(region);
}

static void CheckEmpireFromMapIndex(SECTREE_MANAGER& manager)
{
    const struct { long map; BYTE empire; } cases[] = {
        {LONG_MIN, 0}, {-1, 0}, {0, 0}, {1, 1}, {20, 1}, {21, 2},
        {40, 2}, {41, 3}, {60, 3}, {61, 0}, {183, 0}, {184, 1},
        {185, 1}, {186, 2}, {187, 2}, {188, 3}, {189, 3}, {190, 0},
        {10000, 0}, {10001, 0}, {LONG_MAX, 0}
    };
    for (std::size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        assert(manager.GetEmpireFromMapIndex(cases[i].map) == cases[i].empire);
}

int main()
{
    assert(sizeof(DWORD) == 4);
    assert(SECTREE_MANAGER::instance_ptr() == 0);
    {
        SECTREE_MANAGER manager;
        assert(SECTREE_MANAGER::instance_ptr() == &manager);
        CheckEmpireFromMapIndex(manager);
        assert(manager.m_map_pkSectree.empty() && manager.m_map_pkArea.empty());
        assert(manager.m_vec_mapRegion.empty() && manager.m_mapNPCPosition.empty());
        assert(manager.GetMap(12) == 0);
        assert(manager.Get(12, 0x12345678UL) == 0);
        assert(manager.Get(12, 6401, 12801) == 0);
        manager.m_map_pkSectree[12] = 0;
        manager.m_map_pkArea[12];
        manager.m_mapNPCPosition[12];
        assert(manager.GetMap(12) == 0);
        assert(manager.Get(12, 0x12345678UL) == 0);

        PIXEL_POSITION position = { 10, 20, 30 };
        assert(!manager.GetMapBasePositionByMapIndex(1, position));
        assert(position.x == 10 && position.y == 20 && position.z == 30);
        AddRegion(manager, 1, 100, 200);
        AddRegion(manager, 1, 500, 600);
        AddRegion(manager, 10000, 300, 400);
        AddRegion(manager, -7, -150, -250);
        assert(manager.GetMapBasePositionByMapIndex(10000, position));
        assert(position.x == 300 && position.y == 400 && position.z == 0);
        assert(manager.GetMapBasePositionByMapIndex(10001, position));
        assert(position.x == 100 && position.y == 200 && position.z == 0);
        assert(manager.GetMapBasePositionByMapIndex(-7, position));
        assert(position.x == -150 && position.y == -250 && position.z == 0);
        assert(!manager.GetMapBasePositionByMapIndex(20001, position));
        assert(position.x == -150 && position.y == -250 && position.z == 0);
    }
    assert(SECTREE_MANAGER::instance_ptr() == 0);

    singleton<SECTREE_MANAGER>* owner = new SECTREE_MANAGER;
    assert(SECTREE_MANAGER::instance_ptr() == owner);
    delete owner;
    assert(SECTREE_MANAGER::instance_ptr() == 0);
}
