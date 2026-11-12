#ifndef MCNCC_GAME_ROULETTE_H
#define MCNCC_GAME_ROULETTE_H

#include "../../common/singleton.h"
#include <vector>

class CHARACTER;
class CItem;
struct SItemTable;

int RandomRangeModulo(int low, int high);

struct tRouletteInfo {
    int m_nAvgPrice[14]{};
    std::vector<int> m_vec_vnum[14];
};

class CRoulette : public singleton<CRoulette> {
public:
    CRoulette();
    virtual ~CRoulette();

    bool LoadRouletteTableData(const char* filename);
    bool Roulette(CHARACTER* character, int money);
    bool Roulette(CHARACTER* character, CItem* item);

private:
    int GetSourceLevel(int price);
    int GetNewSourceLevel(int sourceLevel);
    SItemTable* GetRandomItem(int sourceLevel);
    SItemTable* GetItem(int vnum);
    void SetItem(int sourceLevel, int vnum);
    CItem* CreateItem(int vnum, int count);
    int CheckEmptyInventory(CHARACTER* character, CItem* item);

    tRouletteInfo m_Info;
};

#endif
