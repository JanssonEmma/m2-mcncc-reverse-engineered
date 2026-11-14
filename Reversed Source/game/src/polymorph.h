#ifndef MCNCC_GAME_POLYMORPH_H
#define MCNCC_GAME_POLYMORPH_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <ext/hash_map>
class CHARACTER;
class CItem;
class CMob;

enum POLYMORPH_BONUS_TYPE {
    POLYMORPH_NO_BONUS = 0,
    POLYMORPH_ATK_BONUS = 1,
    POLYMORPH_DEF_BONUS = 2,
    POLYMORPH_SPD_BONUS = 3,
};

class CPolymorphUtils : public singleton<CPolymorphUtils> {
public:
    bool GiveBook(CHARACTER* ch, DWORD mobVnum, DWORD practice, BYTE bookLevel, BYTE levelLimit);
    bool BookUpgrade(CHARACTER* ch, CItem* item);
    bool Initialize();
    bool PolymorphCharacter(CHARACTER* ch, CItem* item, const CMob* mob);
    bool UpdateBookPracticeGrade(CHARACTER* ch, CItem* item);
    POLYMORPH_BONUS_TYPE GetBonusType(DWORD race);
private:
    __gnu_cxx::hash_map<DWORD, DWORD> m_mapSPDType;
    __gnu_cxx::hash_map<DWORD, DWORD> m_mapATKType;
    __gnu_cxx::hash_map<DWORD, DWORD> m_mapDEFType;
};
#endif
