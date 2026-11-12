#ifndef MCNCC_GAME_REFINE_H
#define MCNCC_GAME_REFINE_H
#include "../../common/singleton.h"
#include "../../common/tables.h"
#include <map>
class CRefineManager : public singleton<CRefineManager> {
public:
    CRefineManager();
    virtual ~CRefineManager();
    bool Initialize(TRefineTable* table, int size);
    const TRefineTable* GetRefineRecipe(DWORD id);
private:
    std::map<DWORD, TRefineTable> m_map_RefineRecipe;
};
#endif
