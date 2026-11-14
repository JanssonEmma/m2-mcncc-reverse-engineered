#include "refine.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <utility>

CRefineManager::CRefineManager() {}
CRefineManager::~CRefineManager() {}

bool CRefineManager::Initialize(TRefineTable* table, int size)
{
    for (int i = 0; i < size; ++i) {
        const TRefineTable& row = table[i];
        const int cost = static_cast<int>(row.cost);
        const int probability = static_cast<int>(row.prob);
        const int id = static_cast<int>(row.id);
        sys_log(0, "REFINE %d prob %d cost %d", id, probability, cost);
        m_map_RefineRecipe.insert(std::make_pair(static_cast<DWORD>(row.id), row));
    }
    sys_log(0, "REFINE: COUNT %d", static_cast<int>(m_map_RefineRecipe.size()));
    return true;
}

// 0806a3f0
const TRefineTable* CRefineManager::GetRefineRecipe(DWORD id)
{
    const auto key = static_cast<DWORD>(static_cast<std::uint32_t>(id));
    if (!key)
        return nullptr;
    const auto it = m_map_RefineRecipe.find(key);
    const bool found = it != m_map_RefineRecipe.end();
    sys_log(0, "REFINE: FIND %lu %s", key, found ? "TRUE" : "FALSE");
    return it != m_map_RefineRecipe.end() ? &it->second : nullptr;
}
